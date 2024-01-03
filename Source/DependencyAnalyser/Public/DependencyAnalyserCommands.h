// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "DependencyAnalyserStyle.h"

class DEPENDENCYANALYSER_API FDependencyAnalyserCommands final : public TCommands<FDependencyAnalyserCommands>
{
public:

	FDependencyAnalyserCommands()
		: TCommands(TEXT("Dependency Analyser"), NSLOCTEXT("Contexts", "Dependency Analyser", "DependencyAnalyser Plugin"), NAME_None, FDependencyAnalyserStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};