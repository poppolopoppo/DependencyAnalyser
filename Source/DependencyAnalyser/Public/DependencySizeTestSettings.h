// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "DependencySizeTestSettings.generated.h"

/**
 */
UCLASS(config=Engine, defaultconfig)
class DEPENDENCYANALYSER_API UDependencySizeTestSettings : public UObject
{
	GENERATED_BODY()

public:
	// 
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultWarningSizeInMB = 50;

	// 
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultErrorSizeInMB = 500;

	//
	UPROPERTY(EditAnywhere, config, Category="Extension Limits")
	TMap<UClass*, int32> ErrorSizePerExtensionType;

	//
	UPROPERTY(EditAnywhere, config, Category="Extension Limits")
	TMap<UClass*, int32> WarningSizePerExtensionType;
	
	// 
	UPROPERTY(EditAnywhere, config, Category="Automated Testing")
	bool bFailForWarnings;
};
