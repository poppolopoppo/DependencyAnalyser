// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyFunctionLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"


TArray<FAssetData> UDependencyFunctionLibrary::RunAssetAudit(const FAssetRegistryModule& AssetRegistryModule)
{
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath("/Game", AssetData, true);

	return AssetData;
}

DependenciesData UDependencyFunctionLibrary::GetDependencies(const FAssetRegistryModule& AssetRegistryModule,
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
		const TOptional<FAssetPackageData> pckgData = AssetRegistryModule.Get().GetAssetPackageDataCopy(dependency);
		
		Data.Amount++;
		Data.TotalSize += pckgData->DiskSize;
	}
	
	return Data;
}

void UDependencyFunctionLibrary::GetDependenciesRecursive(const FAssetRegistryModule& AssetRegistryModule,
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

		if (!AssetRegistryModule.Get().GetAssetPackageDataCopy(Subdependency))
		{
			continue;
		}

		Dependencies.Add(FName(Subdependency));

		GetDependenciesRecursive(AssetRegistryModule, Dependencies.Last(), QueryType, IgnoreDevFolders, Dependencies);
	}
}

bool UDependencyFunctionLibrary::IsOverMBSize(const SIZE_T Size, int32 SizeMB)
{
	return Size >= SizeMB * 1000000;
}