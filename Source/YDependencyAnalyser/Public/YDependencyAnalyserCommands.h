// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "YDependencyAnalyserStyle.h"

class FYDependencyAnalyserCommands : public TCommands<FYDependencyAnalyserCommands>
{
public:

	FYDependencyAnalyserCommands()
		: TCommands<FYDependencyAnalyserCommands>(TEXT("Dependency Analyser"), NSLOCTEXT("Contexts", "Dependency Analyser", "YDependencyAnalyser Plugin"), NAME_None, FYDependencyAnalyserStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};