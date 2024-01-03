// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyAnalyser.h"

#include "DependencySizeTestSettings.h"
#include "ISettingsModule.h"
#include "DependencyAnalyserCommands.h"
#include "DependencyAnalyserWidget.h"
#include "Widgets/Docking/SDockTab.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

static const FName DependencyAnalyserTabName("Dependency Analyser");

#define LOCTEXT_NAMESPACE "FDependencyAnalyserModule"

void FDependencyAnalyserModule::StartupModule()
{
	FDependencyAnalyserCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDependencyAnalyserCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FDependencyAnalyserModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDependencyAnalyserModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			DependencyAnalyserTabName,
			FOnSpawnTab::CreateRaw(this, &FDependencyAnalyserModule::OnSpawnPluginTab)
		)
		.SetDisplayName(LOCTEXT("FDependencyAnalyserTabTitle", "Dependency Analyser"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
		.SetIcon(FSlateIcon(FAppStyle::Get().GetStyleSetName(), "GenericEditor.Tabs.Properties"));

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "Advanced", "Dependency Analyser",
			LOCTEXT("DependencyAnalyserName", "Dependency Analyser"),
			LOCTEXT("DependencyAnalyserDescription", "Manage Dependency Analyser settings."),
			GetMutableDefault<UDependencySizeTestSettings>());
	}
}

void FDependencyAnalyserModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FDependencyAnalyserCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(DependencyAnalyserTabName);

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Advanced", "Automation");
	}
}

TSharedRef<SDockTab> FDependencyAnalyserModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			SNew(SDependencyAnalyserWidget)
		];
}

void FDependencyAnalyserModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(DependencyAnalyserTabName);
}

void FDependencyAnalyserModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window.General.Miscellaneous");
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
		Section.AddMenuEntryWithCommandList(FDependencyAnalyserCommands::Get().OpenPluginWindow, PluginCommands);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDependencyAnalyserModule, DependencyAnalyser)