// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyAnalyserCommands.h"

#define LOCTEXT_NAMESPACE "FDependencyAnalyserModule"

void FDependencyAnalyserCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Dependency Analyser", "Bring up Dependency Analyser window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
