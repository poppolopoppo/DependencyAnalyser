// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyFunctionLibrary.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ConfigCacheIni.h"


TArray<FAssetData> UDependencyFunctionLibrary::RunAssetAudit(const FAssetRegistryModule& AssetRegistryModule)
{
	CacheConfig();
	
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath("/Game", AssetData, true);

	return AssetData;
}

FDependenciesData UDependencyFunctionLibrary::GetDependencies(const FAssetRegistryModule& AssetRegistryModule,
	const FAssetData& AssetData, const bool IncludeSoftReferences, const bool IgnoreDevFolders)
{
	FDependenciesData Data = {0, 0};
	TArray<FName> Dependencies;

	GetDependenciesRecursive(AssetRegistryModule, AssetData.PackageName, UE::AssetRegistry::EDependencyQuery::Hard, IgnoreDevFolders, Dependencies);

	if (IncludeSoftReferences)
	{
		GetDependenciesRecursive(AssetRegistryModule, AssetData.PackageName, UE::AssetRegistry::EDependencyQuery::Soft, IgnoreDevFolders, Dependencies);
	}
	
	for (const FName& Dependency : Dependencies)
	{
		const TOptional<FAssetPackageData> PackageData = AssetRegistryModule.Get().GetAssetPackageDataCopy(Dependency);

		Data.Amount++;
		Data.DiskSize += PackageData->DiskSize;

		if (UDependencyFunctionLibrary::bEnableMemorySizeCalculation)
		{
			TArray<FAssetData> OutAssetData;
			AssetRegistryModule.Get().GetAssetsByPackageName(Dependency, OutAssetData);
			for (const FAssetData& DependencyAssetData : OutAssetData)
			{
				// Resource size can currently only be calculated for loaded assets, so load and check
				if (UObject* DependencyAsset = DependencyAssetData.GetAsset())
				{
					Data.MemorySize += DependencyAsset->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);
				}
			}
		}
	}

	if (UDependencyFunctionLibrary::bEnableMemorySizeCalculation)
	{
		// Resource size can currently only be calculated for loaded assets, so load and check
		if (UObject* Asset = AssetData.GetAsset())
		{
			Data.MemorySize += Asset->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);
		}
	}
	
	
	return Data;
}

void UDependencyFunctionLibrary::CacheConfig()
{
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("DefaultWarningSizeInMB"), CachedDefaultWarningSize, GEngineIni);
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("DefaultErrorSizeInMB"), CachedDefaultErrorSize, GEngineIni);
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("DefaultWarningReferenceCount"), CachedDefaultWarningCount, GEngineIni);
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("DefaultErrorReferenceCount"), CachedDefaultErrorCount, GEngineIni);
	GConfig->GetBool(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("bFailForWarnings"), bCachedFailForWarnings, GEngineIni);
	GConfig->GetBool(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("bEnableMemorySizeCalculation"), bEnableMemorySizeCalculation, GEngineIni);

	FString SingleStringFromConfig;
	TArray<FString> ExtensionTypes;

	GConfig->GetString(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("WarningLimitsPerAssetType"), SingleStringFromConfig, GEngineIni);
	SingleStringFromConfig.ParseIntoArray(ExtensionTypes, TEXT("),("));
	
	for (const FString& Type : ExtensionTypes)
	{
		FString ClassStr, DepStr, PackageStr, NameStr, DiskSizeStr, MemorySizeStr, CountStr;
		Type.Split(TEXT(", "), &ClassStr, &DepStr);
		ClassStr.Split(TEXT("\'\""), &PackageStr, &NameStr);
		NameStr.RemoveFromEnd(TEXT("\"\'"));
		DepStr.Split(TEXT(","), &DiskSizeStr, &MemorySizeStr);
		MemorySizeStr.Split(TEXT(","), &MemorySizeStr, &CountStr);
		DiskSizeStr.ReplaceInline(TEXT("(DiskSizeLimit="), TEXT(""));
		MemorySizeStr.ReplaceInline(TEXT("MemorySizeLimit="), TEXT(""));
		CountStr.ReplaceInline(TEXT("ReferenceCountLimit="), TEXT(""));

		if (UClass* ParsedClass = FindObject<UClass>(nullptr, *ClassStr))
		{
			int32 ParsedDiskSize = FCString::Atoi(*DiskSizeStr);
			CachedWarningDiskSizePerType.Add(ParsedClass, ParsedDiskSize);
			int32 ParsedMemorySize = FCString::Atoi(*MemorySizeStr);
			CachedWarningMemorySizePerType.Add(ParsedClass, ParsedMemorySize);
			int32 ParsedCount = FCString::Atoi(*CountStr);
			CachedWarningCountPerType.Add(ParsedClass, ParsedCount);
		}
	}

	GConfig->GetString(TEXT("/Script/DependencyAnalyser.DependencyAnalyserTestSettings"), TEXT("ErrorLimitsPerAssetType"), SingleStringFromConfig, GEngineIni);
	SingleStringFromConfig.ParseIntoArray(ExtensionTypes, TEXT("),("));
	
	for (const FString& Type : ExtensionTypes)
	{
		FString ClassStr, DepStr, PackageStr, NameStr, DiskSizeStr, MemorySizeStr, CountStr;
		Type.Split(TEXT(", "), &ClassStr, &DepStr);
		ClassStr.Split(TEXT("\'\""), &PackageStr, &NameStr);
		NameStr.RemoveFromEnd(TEXT("\"\'"));
		DepStr.Split(TEXT(","), &DiskSizeStr, &MemorySizeStr);
		MemorySizeStr.Split(TEXT(","), &MemorySizeStr, &CountStr);
		DiskSizeStr.ReplaceInline(TEXT("(DiskSizeLimit="), TEXT(""));
		MemorySizeStr.ReplaceInline(TEXT("MemorySizeLimit="), TEXT(""));
		CountStr.ReplaceInline(TEXT("ReferenceCountLimit="), TEXT(""));
	
		if (UClass* ParsedClass = FindObject<UClass>(nullptr, *ClassStr))
		{
			int32 ParsedDiskSize = FCString::Atoi(*DiskSizeStr);
			CachedErrorDiskSizePerType.Add(ParsedClass, ParsedDiskSize);
			int32 ParsedMemorySize = FCString::Atoi(*MemorySizeStr);
			CachedErrorMemorySizePerType.Add(ParsedClass, ParsedMemorySize);
			int32 ParsedCount = FCString::Atoi(*CountStr);
			CachedErrorCountPerType.Add(ParsedClass, ParsedCount);
		}
	}
}

bool UDependencyFunctionLibrary::IsWarningDiskSize(const UClass* Class, const SIZE_T Size, int32& OutWarningSize)
{
	OutWarningSize = CachedDefaultWarningSize;
	if (CachedWarningDiskSizePerType.Contains(Class))
	{
		OutWarningSize = CachedWarningDiskSizePerType.FindChecked(Class);
	}
		
	return IsOverMBSize(Size, OutWarningSize);
}

bool UDependencyFunctionLibrary::IsWarningMemorySize(const UClass* Class, const SIZE_T Size, int32& OutWarningSize)
{
	OutWarningSize = CachedDefaultWarningSize;
	if (CachedWarningMemorySizePerType.Contains(Class))
	{
		OutWarningSize = CachedWarningMemorySizePerType.FindChecked(Class);
	}
		
	return IsOverMBSize(Size, OutWarningSize);
}

bool UDependencyFunctionLibrary::IsWarningCount(const UClass* Class, const int32 Count, int32& OutWarningCount)
{
	OutWarningCount = CachedDefaultWarningCount;
	if (CachedWarningCountPerType.Contains(Class))
	{
		OutWarningCount = CachedWarningCountPerType.FindChecked(Class);
	}
	
	return Count > OutWarningCount;		
}

bool UDependencyFunctionLibrary::IsErrorDiskSize(const UClass* Class, const SIZE_T Size, int32& OutErrorSize)
{
	OutErrorSize = CachedDefaultErrorSize;
	if (CachedErrorDiskSizePerType.Contains(Class))
	{
		OutErrorSize = CachedErrorDiskSizePerType.FindChecked(Class);
	}
		
	return IsOverMBSize(Size, OutErrorSize);
}

bool UDependencyFunctionLibrary::IsErrorMemorySize(const UClass* Class, const SIZE_T Size, int32& OutErrorSize)
{
	OutErrorSize = CachedDefaultErrorSize;
	if (CachedErrorMemorySizePerType.Contains(Class))
	{
		OutErrorSize = CachedErrorMemorySizePerType.FindChecked(Class);
	}

	return IsOverMBSize(Size, OutErrorSize);
}

bool UDependencyFunctionLibrary::IsErrorCount(const UClass* Class, const int32 Count, int32& OutErrorCount)
{
	OutErrorCount = CachedDefaultErrorCount;
	if (CachedErrorCountPerType.Contains(Class))
	{
		OutErrorCount = CachedErrorCountPerType.FindChecked(Class);
	}

	return Count > OutErrorCount;
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
	const int32 TotalMB = SizeMB * 1000000;
	return Size > TotalMB;
}