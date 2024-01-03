// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

/* Responsible for loading and unloading module and spawning relevant windows */
class DEPENDENCYANALYSER_API FDependencyAnalyserModule final : public IModuleInterface, public TSharedFromThis<FDependencyAnalyserModule>
{
public:

	// IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	// This function will be bound to Command (by default it will bring up plugin window)
	void PluginButtonClicked();
	
private:

	void RegisterMenus();
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<FUICommandList> PluginCommands;
};
