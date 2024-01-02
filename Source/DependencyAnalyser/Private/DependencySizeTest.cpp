// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyAnalyserWidget.h"
#include "DependencyFunctionLibrary.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FDependencySizeTest, "DependencyAnalyser.DependencySizeTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FDependencySizeTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> Results = UDependencyFunctionLibrary::RunAssetAudit(AssetRegistryModule);
	for (int i = 0; i < Results.Num(); i++)
	{
		const FString& Filename = Results[i].PackageName.ToString();
		OutBeautifiedNames.Add(FPaths::GetBaseFilename(Filename));
		OutTestCommands.Add(Filename);
	}
}

bool FDependencySizeTest::RunTest(const FString& Parameters)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	int32 ErrorSize = 500;
	int32 WarningSize = 50;
	bool bFailForWarnings = false;
	
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("WarningSizeInMB"), WarningSize, GEngineIni);
	GConfig->GetInt(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("ErrorSizeInMB"), ErrorSize, GEngineIni);
	GConfig->GetBool(TEXT("/Script/DependencyAnalyser.DependencySizeTestSettings"), TEXT("bFailForWarnings"), bFailForWarnings, GEngineIni);

	const DependenciesData Dependencies = UDependencyFunctionLibrary::GetDependencies(
		AssetRegistryModule,
		FName(Parameters),
		true,
		true);

	if (UDependencyFunctionLibrary::IsOverMBSize(Dependencies.TotalSize, ErrorSize))
	{
		const FString& ErrorMsg = FString::Printf(TEXT("Asset size %llu MB is larger than max %d MB size"), Dependencies.TotalSize / 1000000, ErrorSize); 
		AddError(ErrorMsg);
		return false;
	}

	if (UDependencyFunctionLibrary::IsOverMBSize(Dependencies.TotalSize, WarningSize))
	{
		const FString& WarningMsg = FString::Printf(TEXT("Asset size %llu MB is larger than recommended %d MB size"), Dependencies.TotalSize / 1000000, WarningSize); 

		if (bFailForWarnings)
		{
			AddError(WarningMsg);
			return false;
		}
		
		AddWarning(WarningMsg);
		return true;
	}
	
	return true;
}
