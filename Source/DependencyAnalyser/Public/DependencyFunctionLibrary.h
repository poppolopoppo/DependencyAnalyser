// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DependencyFunctionLibrary.generated.h"

struct DEPENDENCYANALYSER_API DependenciesData
{
	int32 Amount;
	SIZE_T TotalSize;
};

UCLASS()
class DEPENDENCYANALYSER_API UDependencyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static TArray<FAssetData> RunAssetAudit(const FAssetRegistryModule& AssetRegistryModule);
	static DependenciesData GetDependencies(const FAssetRegistryModule& AssetRegistryModule, FName PackageName,
		bool IncludeSoftReferences, bool IgnoreDevFolders);
	static bool IsOverMBSize(const SIZE_T Size, int32 SizeMB);

private:
	static void GetDependenciesRecursive(const FAssetRegistryModule& AssetRegistryModule, FName PackageName,
		UE::AssetRegistry::EDependencyQuery QueryType, bool IgnoreDevFolders, TArray<FName>& Dependencies);
};
