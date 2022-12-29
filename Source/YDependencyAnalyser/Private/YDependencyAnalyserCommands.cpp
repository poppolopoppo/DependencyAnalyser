// Copyright Epic Games, Inc. All Rights Reserved.

#include "YDependencyAnalyserCommands.h"

#define LOCTEXT_NAMESPACE "FYDependencyAnalyserModule"

void FYDependencyAnalyserCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Dependency Analyser", "Bring up Dependency Analyser window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
