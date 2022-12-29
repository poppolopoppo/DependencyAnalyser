// Copyright Epic Games, Inc. All Rights Reserved.

#include "YDependencyAnalyser.h"
#include "YDependencyAnalyserCommands.h"
#include "YDependencyAnalyserWidget.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

static const FName YDependencyAnalyserTabName("Dependency Analyser");

#define LOCTEXT_NAMESPACE "FYDependencyAnalyserModule"

void FYDependencyAnalyserModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FYDependencyAnalyserCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FYDependencyAnalyserCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FYDependencyAnalyserModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FYDependencyAnalyserModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			YDependencyAnalyserTabName,
			FOnSpawnTab::CreateRaw(this, &FYDependencyAnalyserModule::OnSpawnPluginTab)
		)
		.SetDisplayName(LOCTEXT("FYDependencyAnalyserTabTitle", "Dependency Analyser"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
		.SetIcon(FSlateIcon(FEditorStyle::Get().GetStyleSetName(), "GenericEditor.Tabs.Properties"));
}

void FYDependencyAnalyserModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FYDependencyAnalyserCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(YDependencyAnalyserTabName);
}

TSharedRef<SDockTab> FYDependencyAnalyserModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SYDependencyAnalyserWidget)
		];
}

void FYDependencyAnalyserModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(YDependencyAnalyserTabName);
}

void FYDependencyAnalyserModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window.General.Miscellaneous");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FYDependencyAnalyserCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FYDependencyAnalyserModule, YDependencyAnalyser)