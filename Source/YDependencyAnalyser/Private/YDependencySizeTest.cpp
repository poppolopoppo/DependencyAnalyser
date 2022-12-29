#include "YDependencyAnalyserWidget.h"
#include "YDependencyFunctionLibrary.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(YDependencySizeTest, "YDependencyAnalyser.YDependencySizeTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool YDependencySizeTest::RunTest(const FString& Parameters)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> Results = UYDependencyFunctionLibrary::RunAssetAudit(AssetRegistryModule);
	TArray<FName> Errors;
	TArray<FName> Warnings;

	const int32 ErrorSize = 500;
	const int32 WarningSize = 50;

	for (int i = 0; i < Results.Num(); i++)
	{
		if (Results[i].PackageName.ToString().StartsWith("/Game/Developers/"))
		{
			continue;
		}

		FAssetData Result = Results[i];
		const DependenciesData dependencies = UYDependencyFunctionLibrary::GetDependencies(
			AssetRegistryModule,
			Result.PackageName,
			true,
			true);

		if (UYDependencyFunctionLibrary::IsOverMBSize(dependencies.TotalSize, ErrorSize))
		{
			Errors.Add(Result.PackageName);
		}
		else if (UYDependencyFunctionLibrary::IsOverMBSize(dependencies.TotalSize, WarningSize))
		{
			Warnings.Add(Result.PackageName);
		}
	}

	if (Warnings.Num() > 0)
	{
		for (auto WarningName : Warnings)
		{
			UE_LOG(LogDependencyAnalyser, Warning, TEXT("Larger than recommended %d MB size: %s"), WarningSize, *WarningName.ToString());
		}
	}

	if (Errors.Num() > 0)
	{
		for (auto ErrorName : Errors)
		{
			UE_LOG(LogDependencyAnalyser, Error, TEXT("Larger than max %d MB size: %s"), ErrorSize, *ErrorName.ToString());
		}
		return false;
	}
	
	return true;
}
