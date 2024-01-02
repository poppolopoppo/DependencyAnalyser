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
	static void CacheConfig();
	static bool IsWarningSize(const UClass* Class, const SIZE_T Size, int32& WarningSize);
	static bool IsErrorSize(const UClass* Class, const SIZE_T Size, int32& ErrorSize);
	static bool IsOverMBSize(const SIZE_T Size, int32 SizeMB);

	inline static int32 CachedDefaultWarningSize = 50;
	inline static int32 CachedDefaultErrorSize = 500;
	inline static bool CachedFailForWarnings;

private:
	static void GetDependenciesRecursive(const FAssetRegistryModule& AssetRegistryModule, FName PackageName,
		UE::AssetRegistry::EDependencyQuery QueryType, bool IgnoreDevFolders, TArray<FName>& Dependencies);
	
	inline static TMap<UClass*, int32> CachedWarningSizePerType;
	inline static TMap<UClass*, int32> CachedErrorSizePerType;
};
