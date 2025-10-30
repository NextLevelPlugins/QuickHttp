// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

#include "QuickHttpBPLibrary.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool UQuickHttpBPLibrary::PrettyPrintJson(const FString& InJson, FString& OutPrettyJson)
{
    TSharedPtr<FJsonObject> Obj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InJson);
    if (FJsonSerializer::Deserialize(Reader, Obj) && Obj.IsValid())
    {
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutPrettyJson);
        return FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
    }
    OutPrettyJson = InJson;
    return false;
}
