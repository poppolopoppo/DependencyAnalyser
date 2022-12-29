// Copyright 2019 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "YDependencyFunctionLibrary.generated.h"

struct YDEPENDENCYANALYSER_API DependenciesData
{
	int32 Amount;
	SIZE_T TotalSize;
};

UCLASS()
class YDEPENDENCYANALYSER_API UYDependencyFunctionLibrary : public UBlueprintFunctionLibrary
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
