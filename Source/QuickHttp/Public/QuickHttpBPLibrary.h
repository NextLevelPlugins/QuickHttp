// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuickHttpBPLibrary.generated.h"

/**
 * Misc Blueprint helpers for JSON/HTTP workflows.
 */
UCLASS()
class QUICKHTTP_API UQuickHttpBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /**
     * Pretty-print a JSON string for readability.
     *
     * Parses the input JSON and re-serializes it with indentation.
     * @param InJson        Raw/minified JSON.
     * @param OutPrettyJson Pretty-printed JSON (if parse succeeded; otherwise returns input).
     * @return              true if parsing succeeded.
     */
    UFUNCTION(BlueprintPure, Category="QuickHttp|JSON",
              meta=(DisplayName="Pretty Print JSON",
                    CompactNodeTitle="Pretty JSON",
                    Keywords="json pretty format indent stringify",
                    ToolTip="Format a JSON string with indentation for readability."))
    static bool PrettyPrintJson(const FString& InJson, FString& OutPrettyJson);
};
