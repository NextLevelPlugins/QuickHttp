// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

#include "Async/HttpJsonRequestAsync.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HAL/PlatformProcess.h"
#include "Async/Async.h"

#include "Runtime/Launch/Resources/Version.h"

TMap<int32, TSet<TWeakObjectPtr<UHttpJsonRequestAsync>>> UHttpJsonRequestAsync::TokenMap;

UHttpJsonRequestAsync* UHttpJsonRequestAsync::HttpJsonRequest(
    UObject* InWorldContextObject,
    EQuickHttpMethod InMethod,
    const FString& InUrl,
    const TArray<FQuickHttpHeader>& InHeaders,
    int32 InTimeoutSeconds,
    int32 InMaxRetries,
    float InBackoffBaseSeconds,
    int32 InMaxPayloadKB,
    int32 InCancelToken,
    const FString& InBodyJsonOptional)
{
    UHttpJsonRequestAsync* Node = NewObject<UHttpJsonRequestAsync>();
    Node->WorldContextObject = InWorldContextObject;
    Node->Method = InMethod;
    Node->Url = InUrl;
    Node->Headers = InHeaders;
    Node->TimeoutSeconds = FMath::Max(1, InTimeoutSeconds);
    Node->MaxRetries = FMath::Max(0, InMaxRetries);
    Node->BackoffBaseSeconds = FMath::Max(0.01f, InBackoffBaseSeconds);
    Node->MaxPayloadKB = FMath::Clamp(InMaxPayloadKB, 1, 8 * 1024);
    Node->CancelToken = InCancelToken;
    Node->BodyJsonOptional = InBodyJsonOptional;

    if (Node->CancelToken != 0)
    {
        TokenMap.FindOrAdd(Node->CancelToken).Add(Node);
    }
    return Node;
}

void UHttpJsonRequestAsync::CancelByToken(int32 InCancelToken)
{
    if (TSet<TWeakObjectPtr<UHttpJsonRequestAsync>>* SetPtr = TokenMap.Find(InCancelToken))
    {
        for (TWeakObjectPtr<UHttpJsonRequestAsync> WeakNode : *SetPtr)
        {
            if (UHttpJsonRequestAsync* Node = WeakNode.Get())
            {
                if (Node->ActiveRequest.IsValid())
                {
                    Node->ActiveRequest->CancelRequest();
                }
                Node->bCompleted = true;
            }
        }
        TokenMap.Remove(InCancelToken);
    }
}

void UHttpJsonRequestAsync::Activate()
{
    AttemptIndex = 0;
    Attempt();
}

void UHttpJsonRequestAsync::Attempt()
{
    if (bCompleted) { return; }

    FHttpModule& Http = FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> Req = Http.CreateRequest();
    ActiveRequest = Req;

    Req->SetURL(Url);
    switch (Method)
    {
        case EQuickHttpMethod::GET:     Req->SetVerb(TEXT("GET")); break;
        case EQuickHttpMethod::POST:    Req->SetVerb(TEXT("POST")); break;
        case EQuickHttpMethod::PUT:     Req->SetVerb(TEXT("PUT")); break;
        case EQuickHttpMethod::PATCH:   Req->SetVerb(TEXT("PATCH")); break;
        case EQuickHttpMethod::DELETE_: Req->SetVerb(TEXT("DELETE")); break;
    }

    Req->SetHeader(TEXT("Accept"), TEXT("application/json"));
    for (const FQuickHttpHeader& H : Headers)
    {
        Req->SetHeader(H.Name, H.Value);
    }

    if (!BodyJsonOptional.IsEmpty() && Req->GetVerb() != TEXT("GET"))
    {
        Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        Req->SetContentAsString(BodyJsonOptional);
    }

    // Manual timeout watchdog (UE4.25-safe)
    TWeakPtr<IHttpRequest, ESPMode::NotThreadSafe> WeakReq = Req;
    const int32 LocalTimeout = TimeoutSeconds;
    bCompleted = false;
    Async(EAsyncExecution::ThreadPool, [this, WeakReq, LocalTimeout](){
        float Elapsed = 0.f;
        const float Step = 0.05f;
        while (!bCompleted && Elapsed < LocalTimeout)
        {
            FPlatformProcess::Sleep(Step);
            Elapsed += Step;
        }
        if (!bCompleted)
        {
            if (auto Locked = WeakReq.Pin())
            {
                Locked->CancelRequest();
            }
        }
    });

    Req->OnProcessRequestComplete().BindWeakLambda(this, [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
    {
        if (bCompleted) { return; }
        bCompleted = true;

        const int32 Status = Response.IsValid() ? Response->GetResponseCode() : 0;
        const FString Body = Response.IsValid() ? Response->GetContentAsString() : FString();

        if (!bSucceeded || !Response.IsValid())
        {
            if (AttemptIndex < MaxRetries)
            {
                const float Delay = ComputeBackoffSeconds(AttemptIndex);
                AttemptIndex++;
                ScheduleRetry(Delay);
                return;
            }
            FinishFailure(Status, TEXT("Network failure or timeout"), Body);
            return;
        }

        if (Body.Len() / 1024 > MaxPayloadKB)
        {
            FinishFailure(Status, TEXT("Payload too large"), Body);
            return;
        }

        if (Status < 200 || Status >= 300)
        {
            if (Status >= 500 && AttemptIndex < MaxRetries)
            {
                const float Delay = ComputeBackoffSeconds(AttemptIndex);
                AttemptIndex++;
                ScheduleRetry(Delay);
                return;
            }
            FinishFailure(Status, TEXT("HTTP error"), Body);
            return;
        }

        FinishSuccess(Body, Status);
    });

    Req->ProcessRequest();
}

void UHttpJsonRequestAsync::ScheduleRetry(float DelaySeconds)
{
    Async(EAsyncExecution::ThreadPool, [this, DelaySeconds](){
        FPlatformProcess::Sleep(FMath::Max(0.0f, DelaySeconds));
        AsyncTask(ENamedThreads::GameThread, [this](){
            Attempt();
        });
    });
}

float UHttpJsonRequestAsync::ComputeBackoffSeconds(int32 Index) const
{
    const float Exp = FMath::Pow(2.f, FMath::Clamp(Index, 0, 10));
    const float Jitter = FMath::FRandRange(0.f, 0.25f);
    return (BackoffBaseSeconds * Exp) + Jitter;
}

void UHttpJsonRequestAsync::FinishSuccess(const FString& Response, int32 StatusCode)
{
    if (CancelToken != 0)
    {
        if (TSet<TWeakObjectPtr<UHttpJsonRequestAsync>>* SetPtr = TokenMap.Find(CancelToken))
        {
            SetPtr->Remove(this);
            if (SetPtr->Num() == 0) { TokenMap.Remove(CancelToken); }
        }
    }
    OnSuccess.Broadcast(Response, StatusCode);
    SetReadyToDestroy();
}

void UHttpJsonRequestAsync::FinishFailure(int32 StatusCode, const FString& Message, const FString& RawBody)
{
    if (CancelToken != 0)
    {
        if (TSet<TWeakObjectPtr<UHttpJsonRequestAsync>>* SetPtr = TokenMap.Find(CancelToken))
        {
            SetPtr->Remove(this);
            if (SetPtr->Num() == 0) { TokenMap.Remove(CancelToken); }
        }
    }
    FQuickHttpError Err; Err.bIsError = true; Err.StatusCode = StatusCode; Err.Message = Message; Err.RawBody = RawBody;
    OnFailure.Broadcast(Err);
    SetReadyToDestroy();
}
