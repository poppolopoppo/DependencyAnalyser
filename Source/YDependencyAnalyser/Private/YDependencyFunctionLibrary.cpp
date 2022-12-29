// Copyright 2019 YAGER Development GmbH All Rights Reserved.

#include "YDependencyFunctionLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"


TArray<FAssetData> UYDependencyFunctionLibrary::RunAssetAudit(const FAssetRegistryModule& AssetRegistryModule)
{
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath("/Game", AssetData, true);

	return AssetData;
}

DependenciesData UYDependencyFunctionLibrary::GetDependencies(const FAssetRegistryModule& AssetRegistryModule,
	FName PackageName, bool IncludeSoftReferences, bool IgnoreDevFolders)
{
	DependenciesData Data = {0, 0};
	TArray<FName> Dependencies;

	GetDependenciesRecursive(AssetRegistryModule, PackageName, UE::AssetRegistry::EDependencyQuery::Hard, IgnoreDevFolders, Dependencies);

	if (IncludeSoftReferences)
	{
		GetDependenciesRecursive(AssetRegistryModule, PackageName, UE::AssetRegistry::EDependencyQuery::Soft, IgnoreDevFolders, Dependencies);
	}
	
	for (const FName& dependency : Dependencies)
	{
		const FAssetPackageData* pckgData = AssetRegistryModule.Get().GetAssetPackageData(dependency);
		
		Data.Amount++;
		Data.TotalSize += pckgData->DiskSize;
	}
	
	return Data;
}

void UYDependencyFunctionLibrary::GetDependenciesRecursive(const FAssetRegistryModule& AssetRegistryModule,
	FName PackageName, UE::AssetRegistry::EDependencyQuery QueryType, bool IgnoreDevFolders, TArray<FName>& Dependencies)
{
	TArray<FName> Subdependencies;
	AssetRegistryModule.Get().GetDependencies(PackageName, Subdependencies,
		UE::AssetRegistry::EDependencyCategory::All, QueryType);
	
	for (const FName& Subdependency : Subdependencies)
	{
		if (IgnoreDevFolders && Subdependency.ToString().StartsWith("/Game/Developers/"))
		{
			continue;
		}
		
		if (Dependencies.Contains(Subdependency))
		{
			continue;
		}

		if (!AssetRegistryModule.Get().GetAssetPackageData(Subdependency))
		{
			continue;
		}

		Dependencies.Add(FName(Subdependency));

		GetDependenciesRecursive(AssetRegistryModule, Dependencies.Last(), QueryType, IgnoreDevFolders, Dependencies);
	}
}

bool UYDependencyFunctionLibrary::IsOverMBSize(const SIZE_T Size, int32 SizeMB)
{
	return Size >= SizeMB * 1000000;
}