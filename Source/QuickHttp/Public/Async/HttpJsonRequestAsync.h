// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "QuickHttpTypes.h"
#include "HttpJsonRequestAsync.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuickHttpSuccess, const FString&, ResponseJson, int32, StatusCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuickHttpFailure, const FQuickHttpError&, Error);

/**
 * Async Blueprint node for HTTP JSON requests.
 * Exposes GET/POST/PUT/PATCH/DELETE, retries with backoff, a timeout watchdog, and cancel-by-token.
 */
UCLASS()
class QUICKHTTP_API UHttpJsonRequestAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    /** Fired when the request succeeds (2xx). ResponseJson is the response body, StatusCode is the HTTP code. */
    UPROPERTY(BlueprintAssignable, Category="QuickHttp")
    FQuickHttpSuccess OnSuccess;

    /** Fired when the request fails (timeout, network error, non-2xx with no retries left). */
    UPROPERTY(BlueprintAssignable, Category="QuickHttp")
    FQuickHttpFailure OnFailure;

    /**
     * Perform an HTTP request and return the JSON response (async).
     *
     * @param WorldContextObject   Used for latent/action lifetime and GameThread dispatch.
     * @param Method               HTTP verb to use.
     * @param Url                  Absolute URL to call.
     * @param Headers              Optional request headers (e.g., Authorization, Content-Type).
     * @param TimeoutSeconds       Max seconds before request is cancelled (watchdog thread).
     * @param MaxRetries           Number of retry attempts on network failure / HTTP 5xx.
     * @param BackoffBaseSeconds   Base seconds for exponential backoff (with small random jitter).
     * @param MaxPayloadKB         Safety limit for response size (in kilobytes).
     * @param CancelToken          Any non-zero int groups requests for mass cancellation via CancelByToken.
     * @param BodyJsonOptional     Optional JSON body for non-GET verbs.
     */
    UFUNCTION(BlueprintCallable,
              Category="QuickHttp",
              meta=(BlueprintInternalUseOnly="true",
                    WorldContext="WorldContextObject",
                    DisplayName="HTTP JSON Request",
                    Keywords="http rest request json api web",
                    ToolTip="Perform an HTTP request and return the JSON response (async)."))
    static UHttpJsonRequestAsync* HttpJsonRequest(
        UObject* WorldContextObject,
        EQuickHttpMethod Method,
        const FString& Url,
        const TArray<FQuickHttpHeader>& Headers,
        int32 TimeoutSeconds,
        int32 MaxRetries,
        float BackoffBaseSeconds,
        int32 MaxPayloadKB,
        int32 CancelToken,
        const FString& BodyJsonOptional
    );

    /**
     * Cancel all in-flight QuickHttp requests that share the given CancelToken.
     * Use the same non-zero CancelToken when creating requests, then call this to abort them.
     */
    UFUNCTION(BlueprintCallable, Category="QuickHttp", meta=(DisplayName="Cancel Requests By Token", Keywords="cancel abort stop http request", ToolTip="Cancel all HTTP requests created with the same CancelToken."))
    static void CancelByToken(int32 InCancelToken);

    // UBlueprintAsyncActionBase
    virtual void Activate() override;

private:
    void Attempt();
    void ScheduleRetry(float DelaySeconds);
    float ComputeBackoffSeconds(int32 Index) const;
    void FinishSuccess(const FString& Response, int32 StatusCode);
    void FinishFailure(int32 StatusCode, const FString& Message, const FString& RawBody);

    // Parameters
    TWeakObjectPtr<UObject> WorldContextObject;
    EQuickHttpMethod Method;
    FString Url;
    TArray<FQuickHttpHeader> Headers;
    int32 TimeoutSeconds = 30;
    int32 MaxRetries = 2;
    float BackoffBaseSeconds = 0.35f;
    int32 MaxPayloadKB = 1024;
    int32 CancelToken = 0;
    FString BodyJsonOptional;
    int32 AttemptIndex = 0;

    // Request state (UE4.25 uses NotThreadSafe for IHttpRequest)
    TSharedPtr<class IHttpRequest, ESPMode::NotThreadSafe> ActiveRequest;
    FThreadSafeBool bCompleted = false;

    static TMap<int32, TSet<TWeakObjectPtr<UHttpJsonRequestAsync>>> TokenMap;
};
