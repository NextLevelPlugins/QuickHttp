// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.


#pragma once
#include "CoreMinimal.h"
#include "QuickHttpTypes.generated.h"

USTRUCT(BlueprintType)
struct FQuickHttpHeader
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="QuickHttp")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="QuickHttp")
    FString Value;
};

UENUM(BlueprintType)
enum class EQuickHttpMethod : uint8
{
    GET     UMETA(DisplayName="GET"),
    POST    UMETA(DisplayName="POST"),
    PUT     UMETA(DisplayName="PUT"),
    PATCH   UMETA(DisplayName="PATCH"),
    DELETE_ UMETA(DisplayName="DELETE")
};

USTRUCT(BlueprintType)
struct FQuickHttpError
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="QuickHttp")
    bool bIsError = false;

    UPROPERTY(BlueprintReadOnly, Category="QuickHttp")
    int32 StatusCode = 0;

    UPROPERTY(BlueprintReadOnly, Category="QuickHttp")
    FString Message;

    UPROPERTY(BlueprintReadOnly, Category="QuickHttp")
    FString RawBody;
};