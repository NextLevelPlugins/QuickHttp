// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

#include "Json/QuickJsonBuilder.h"
#include "Dom/JsonObject.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"

UQuickJsonBuilder* UQuickJsonBuilder::NewJsonBuilder()
{
    UQuickJsonBuilder* B = NewObject<UQuickJsonBuilder>();
    B->Root = MakeShareable(new FJsonObject());
    return B;
}

UQuickJsonBuilder* UQuickJsonBuilder::SetString(const FString& Name, const FString& Value)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetStringField(Name, Value);
    return this;
}

UQuickJsonBuilder* UQuickJsonBuilder::SetInt(const FString& Name, int32 Value)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetNumberField(Name, (double)Value);
    return this;
}

UQuickJsonBuilder* UQuickJsonBuilder::SetNumber(const FString& Name, float Value)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetNumberField(Name, (double)Value);
    return this;
}

UQuickJsonBuilder* UQuickJsonBuilder::SetBool(const FString& Name, bool bValue)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetBoolField(Name, bValue);
    return this;
}

UQuickJsonBuilder* UQuickJsonBuilder::SetNull(const FString& Name)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetField(Name, MakeShared<FJsonValueNull>());
    return this;
}

UQuickJsonBuilder* UQuickJsonBuilder::SetObject(const FString& Name, UQuickJsonBuilder* ObjectBuilder)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    if (ObjectBuilder && ObjectBuilder->Root.IsValid())
    {
        Root->SetObjectField(Name, ObjectBuilder->Root);
    }
    return this;
}

static TArray<TSharedPtr<FJsonValue>> MakeStringArray(const TArray<FString>& In)
{
    TArray<TSharedPtr<FJsonValue>> Arr;
    Arr.Reserve(In.Num());
    for (const FString& S : In) { Arr.Add(MakeShared<FJsonValueString>(S)); }
    return Arr;
}
static TArray<TSharedPtr<FJsonValue>> MakeIntArray(const TArray<int32>& In)
{
    TArray<TSharedPtr<FJsonValue>> Arr;
    Arr.Reserve(In.Num());
    for (int32 V : In) { Arr.Add(MakeShared<FJsonValueNumber>((double)V)); }
    return Arr;
}
static TArray<TSharedPtr<FJsonValue>> MakeNumberArray(const TArray<float>& In)
{
    TArray<TSharedPtr<FJsonValue>> Arr;
    Arr.Reserve(In.Num());
    for (float V : In) { Arr.Add(MakeShared<FJsonValueNumber>((double)V)); }
    return Arr;
}
static TArray<TSharedPtr<FJsonValue>> MakeBoolArray(const TArray<bool>& In)
{
    TArray<TSharedPtr<FJsonValue>> Arr;
    Arr.Reserve(In.Num());
    for (bool V : In) { Arr.Add(MakeShared<FJsonValueBoolean>(V)); }
    return Arr;
}
static TArray<TSharedPtr<FJsonValue>> MakeObjectArray(const TArray<UQuickJsonBuilder*>& In)
{
    TArray<TSharedPtr<FJsonValue>> Arr;
    Arr.Reserve(In.Num());
    for (UQuickJsonBuilder* B : In)
    {
        if (B && B->GetObject().IsValid())
        {
            Arr.Add(MakeShared<FJsonValueObject>(B->GetObject()));
        }
        else
        {
            Arr.Add(MakeShared<FJsonValueNull>());
        }
    }
    return Arr;
}

UQuickJsonBuilder* UQuickJsonBuilder::SetArrayString(const FString& Name, const TArray<FString>& Values)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetArrayField(Name, MakeStringArray(Values));
    return this;
}
UQuickJsonBuilder* UQuickJsonBuilder::SetArrayInt(const FString& Name, const TArray<int32>& Values)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetArrayField(Name, MakeIntArray(Values));
    return this;
}
UQuickJsonBuilder* UQuickJsonBuilder::SetArrayNumber(const FString& Name, const TArray<float>& Values)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetArrayField(Name, MakeNumberArray(Values));
    return this;
}
UQuickJsonBuilder* UQuickJsonBuilder::SetArrayBool(const FString& Name, const TArray<bool>& Values)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetArrayField(Name, MakeBoolArray(Values));
    return this;
}
UQuickJsonBuilder* UQuickJsonBuilder::SetArrayObjects(const FString& Name, const TArray<UQuickJsonBuilder*>& ObjectBuilders)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    Root->SetArrayField(Name, MakeObjectArray(ObjectBuilders));
    return this;
}

FString UQuickJsonBuilder::ToString(bool bPretty)
{
    if (!Root.IsValid()) Root = MakeShareable(new FJsonObject());
    FString Out;
    if (bPretty)
    {
        auto Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Out);
        FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
    }
    else
    {
        auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Out);
        FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
    }
    return Out;
}
