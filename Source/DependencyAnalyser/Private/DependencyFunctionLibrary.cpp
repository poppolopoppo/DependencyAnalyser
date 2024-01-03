// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyFunctionLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"


TArray<FAssetData> UDependencyFunctionLibrary::RunAssetAudit(const FAssetRegistryModule& AssetRegistryModule)
{
	CacheConfig();
	
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath("/Game", AssetData, true);

	return AssetData;
}

FDependenciesData UDependencyFunctionLibrary::GetDependencies(const FAssetRegistryModule& AssetRegistryModule,
	const FName PackageName, const bool IncludeSoftReferences, const bool IgnoreDevFolders)
{
	FDependenciesData Data = {0, 0};
	TArray<FName> Dependencies;

	GetDependenciesRecursive(AssetRegistryModule, PackageName, UE::AssetRegistry::EDependencyQuery::Hard, IgnoreDevFolders, Dependencies);

	if (IncludeSoftReferences)
	{
		GetDependenciesRecursive(AssetRegistryModule, PackageName, UE::AssetRegistry::EDependencyQuery::Soft, IgnoreDevFolders, Dependencies);
	}
	
	for (const FName& Dependency : Dependencies)
	{
		const TOptional<FAssetPackageData> PackageData = AssetRegistryModule.Get().GetAssetPackageDataCopy(Dependency);
		
		Data.Amount++;
		Data.TotalSize += PackageData->DiskSize;
	}
	
	return Data;
}

void UDependencyFunctionLibrary::CacheConfig()
{
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("DefaultWarningSizeInMB"), CachedDefaultWarningSize, GEngineIni);
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("DefaultErrorSizeInMB"), CachedDefaultErrorSize, GEngineIni);
	GConfig->GetBool(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("bFailForWarnings"), bCachedFailForWarnings, GEngineIni);

	FString SingleStringFromConfig;
	TArray<FString> ExtensionTypes;

	GConfig->GetString(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("WarningSizePerExtensionType"), SingleStringFromConfig, GEngineIni);
	SingleStringFromConfig.ParseIntoArray(ExtensionTypes, TEXT("),("));
	
	for (const FString& Type : ExtensionTypes)
	{
		FString ClassStr, SizeStr, PackageStr, NameStr;
		Type.Split(TEXT(", "), &ClassStr, &SizeStr);
		ClassStr.Split(TEXT("\'\""), &PackageStr, &NameStr);
		NameStr.RemoveFromEnd(TEXT("\"\'"));

		if (UClass* ParsedClass = FindObject<UClass>(nullptr, *NameStr))
		{
			int32 ParsedSize = FCString::Atoi(*SizeStr);
			CachedWarningSizePerType.Add(ParsedClass, ParsedSize);
		}
	}

	GConfig->GetString(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("ErrorSizePerExtensionType"), SingleStringFromConfig, GEngineIni);
	SingleStringFromConfig.ParseIntoArray(ExtensionTypes, TEXT("),("));
	
	for (const FString& Type : ExtensionTypes)
	{
		FString ClassStr, SizeStr, PackageStr, NameStr;
		Type.Split(TEXT(", "), &ClassStr, &SizeStr);
		ClassStr.Split(TEXT("\'\""), &PackageStr, &NameStr);
		NameStr.RemoveFromEnd(TEXT("\"\'"));

		if (UClass* ParsedClass = FindObject<UClass>(nullptr, *NameStr))
		{
			int32 ParsedSize = FCString::Atoi(*SizeStr);
			CachedErrorSizePerType.Add(ParsedClass, ParsedSize);
		}
	}
}

bool UDependencyFunctionLibrary::IsWarningSize(const UClass* Class, const SIZE_T Size, int32& OutWarningSize)
{
	OutWarningSize = CachedDefaultWarningSize;
	if (CachedWarningSizePerType.Contains(Class))
	{
		OutWarningSize = CachedWarningSizePerType.FindChecked(Class);
	}
		
	return IsOverMBSize(Size, OutWarningSize);
}

bool UDependencyFunctionLibrary::IsErrorSize(const UClass* Class, const SIZE_T Size, int32& OutErrorSize)
{
	OutErrorSize = CachedDefaultErrorSize;
	if (CachedErrorSizePerType.Contains(Class))
	{
		OutErrorSize = CachedErrorSizePerType.FindChecked(Class);
	}
		
	return IsOverMBSize(Size, OutErrorSize);
}

void UDependencyFunctionLibrary::GetDependenciesRecursive(const FAssetRegistryModule& AssetRegistryModule,
                                                          const FName PackageName, const UE::AssetRegistry::EDependencyQuery QueryType, const bool IgnoreDevFolders, TArray<FName>& OutDependencies)
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
		
		if (OutDependencies.Contains(Subdependency))
		{
			continue;
		}

		if (!AssetRegistryModule.Get().GetAssetPackageDataCopy(Subdependency))
		{
			continue;
		}

		OutDependencies.Add(FName(Subdependency));

		GetDependenciesRecursive(AssetRegistryModule, OutDependencies.Last(), QueryType, IgnoreDevFolders, OutDependencies);
	}
}

bool UDependencyFunctionLibrary::IsOverMBSize(const SIZE_T Size, const int32 SizeMB)
{
	return Size >= SizeMB * 1000000;
}