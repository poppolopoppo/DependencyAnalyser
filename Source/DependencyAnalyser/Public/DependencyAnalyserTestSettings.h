// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "DependencyAnalyserTestSettings.generated.h"

/* Configuration variables to customise plugin behaviour */
UCLASS(config=Engine, defaultconfig)
class DEPENDENCYANALYSER_API UDependencyAnalyserTestSettings : public UObject
{
	GENERATED_BODY()

public:
	
	// Default size at which an asset throws a warning
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultWarningSizeInMB = 50;

	// Default size at which an asset throws an error
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultErrorSizeInMB = 500;
	
	// Default reference count at which an asset throws a warning
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultWarningReferenceCount = 10;

	// Default reference count at which an asset throws an error
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultErrorReferenceCount = 100;

	// A list of warning sizes per asset type that will be used instead of default
	UPROPERTY(EditAnywhere, config, Category="Type Limits")
	TMap<UClass*, int32> WarningSizePerAssetType;
	
	// A list of error sizes per asset type that will be used instead of default 
	UPROPERTY(EditAnywhere, config, Category="Type Limits")
	TMap<UClass*, int32> ErrorSizePerAssetType;
	
	// A list of warning reference counts per asset type that will be used instead of default
	UPROPERTY(EditAnywhere, config, Category="Type Limits")
	TMap<UClass*, int32> WarningReferenceCountPerAssetType;
	
	// A list of error reference counts per asset type that will be used instead of default 
	UPROPERTY(EditAnywhere, config, Category="Type Limits")
	TMap<UClass*, int32> ErrorReferenceCountPerAssetType;
	
	// Whether a test should fail in case a warning is thrown
	UPROPERTY(EditAnywhere, config, Category="Automated Testing")
	bool bFailForWarnings;
};
