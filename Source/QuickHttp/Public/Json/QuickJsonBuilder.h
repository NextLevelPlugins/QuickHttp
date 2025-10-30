// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuickJsonBuilder.generated.h"

/**
 * Blueprint-friendly JSON builder.
 *
 * Usage:
 *   Json = NewJsonBuilder()->SetString("user","alex")->SetInt("age",22)->ToString(true);
 * Or build nested objects / arrays via dedicated helpers.
 */
UCLASS(BlueprintType)
class QUICKHTTP_API UQuickJsonBuilder : public UObject
{
    GENERATED_BODY()
public:
    /** Create a new empty JSON object builder. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="New JSON Builder",
                    ToolTip="Create a new empty JSON object builder."))
    static UQuickJsonBuilder* NewJsonBuilder();

    /** Set a string field. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set String"))
    UQuickJsonBuilder* SetString(const FString& Name, const FString& Value);

    /** Set an integer field. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Int"))
    UQuickJsonBuilder* SetInt(const FString& Name, int32 Value);

    /** Set a number (float/double) field. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Number"))
    UQuickJsonBuilder* SetNumber(const FString& Name, float Value);

    /** Set a boolean field. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Bool"))
    UQuickJsonBuilder* SetBool(const FString& Name, bool bValue);

    /** Set null for a field. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Null"))
    UQuickJsonBuilder* SetNull(const FString& Name);

    /** Set a nested object from another builder. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Object"))
    UQuickJsonBuilder* SetObject(const FString& Name, UQuickJsonBuilder* ObjectBuilder);

    /** Set an array of strings. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Array (String)"))
    UQuickJsonBuilder* SetArrayString(const FString& Name, const TArray<FString>& Values);

    /** Set an array of ints. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Array (Int)"))
    UQuickJsonBuilder* SetArrayInt(const FString& Name, const TArray<int32>& Values);

    /** Set an array of numbers. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Array (Number)"))
    UQuickJsonBuilder* SetArrayNumber(const FString& Name, const TArray<float>& Values);

    /** Set an array of bools. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Array (Bool)"))
    UQuickJsonBuilder* SetArrayBool(const FString& Name, const TArray<bool>& Values);

    /** Set an array of objects from other builders. */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="Set Array (Objects)"))
    UQuickJsonBuilder* SetArrayObjects(const FString& Name, const TArray<UQuickJsonBuilder*>& ObjectBuilders);

    /**
     * Convert this JSON object to a string.
     * @param bPretty  If true, outputs indented JSON.
     */
    UFUNCTION(BlueprintCallable, Category="QuickHttp|JSON",
              meta=(DisplayName="To JSON String", CompactNodeTitle="To JSON"))
    FString ToString(bool bPretty);

public:
    /** Returns an internal shared pointer to the built object (for advanced C++ callers). */
    TSharedPtr<class FJsonObject> GetObject() const { return Root; }

private:
    TSharedPtr<class FJsonObject> Root;
};
