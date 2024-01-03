// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "DependencySizeTestSettings.generated.h"

/* Configuration variables to customise plugin behaviour */
UCLASS(config=Engine, defaultconfig)
class DEPENDENCYANALYSER_API UDependencySizeTestSettings : public UObject
{
	GENERATED_BODY()

public:
	
	// Default size at which an asset throws a warning
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultWarningSizeInMB = 50;

	// Default size at which an asset throws an error
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultErrorSizeInMB = 500;

	// A list of warning sizes per extension type that will be used instead of default
	UPROPERTY(EditAnywhere, config, Category="Extension Limits")
	TMap<UClass*, int32> WarningSizePerExtensionType;
	
	// A list of error sizes per extension type that will be used instead of default 
	UPROPERTY(EditAnywhere, config, Category="Extension Limits")
	TMap<UClass*, int32> ErrorSizePerExtensionType;
	
	// Whether a test should fail in case a warning is thrown
	UPROPERTY(EditAnywhere, config, Category="Automated Testing")
	bool bFailForWarnings;
};
