// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

#include "Modules/ModuleManager.h"

class FQuickHttpModule : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};
IMPLEMENT_MODULE(FQuickHttpModule, QuickHttp)
