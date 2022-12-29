// Copyright 2019 YAGER Development GmbH All Rights Reserved.


#include "YDependencyAnalyserWidget.h"

#include "YDependencyFunctionLibrary.h"
#include "SlateOptMacros.h"
#include "SListViewSelectorDropdownMenu.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopedSlowTask.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

DEFINE_LOG_CATEGORY(LogDependencyAnalyser);

FName SYDependencyAnalyserWidget::NAME_Name(TEXT("Name"));
FName SYDependencyAnalyserWidget::NAME_DependenciesCount(TEXT("Dependencies Count"));
FName SYDependencyAnalyserWidget::NAME_TotalSize(TEXT("Total Size"));
FName SYDependencyAnalyserWidget::NAME_Type(TEXT("Type"));
FName SYDependencyAnalyserWidget::NAME_Path(TEXT("Path"));

void SYDependencyAnalyserWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.FillWidth(1)
			.Padding(5)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SAssignNew(IncludeSoftRef, SCheckBox)
					.IsChecked(true)
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Include Soft References"))
					.ToolTipText(FText::FromString("When counting dependencies, include soft references"))
				]
				
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				.Padding(5, 0, 0, 0)
				[
					SAssignNew(IgnoreDevFolders, SCheckBox)
					.IsChecked(true)
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Ignore Dev Folders"))
					.ToolTipText(FText::FromString("When counting dependencies and listing assets, ignore dev folders"))
				]
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Asset Warning Size (MB):"))
				.ToolTipText(FText::FromString("Assets over this size (in MB) will be marked as warning"))
			]
				
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(5, 0, 0, 0)
			[
				SAssignNew(WarningSize, SEditableTextBox)
				.Text(FText::AsNumber(DefaultWarningSize))
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(10, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Asset Error Size (MB):"))
				.ToolTipText(FText::FromString("Assets over this size (in MB) will be marked as error"))
			]
				
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(5, 0, 0, 0)
			[
				SAssignNew(ErrorSize, SEditableTextBox)
				.Text(FText::AsNumber(DefaultErrorSize))
			]
			
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(10, 0, 10, 0)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Export as CSV")))
				.ToolTipText(FText::FromString("Exports data (if any is listed) and brings up directory where CSV file is stored (overriding any existing file)"))
				.OnClicked(this, &SYDependencyAnalyserWidget::OnExport)
			]
		]
		
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		.Padding(5)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Run Dependency Analysis")))
				.ToolTipText(FText::FromString("Starts running dependency analysis - it might take some time!"))
				.OnClicked(this, &SYDependencyAnalyserWidget::OnRun)
			]
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.MaxHeight(30)
		.Padding(5)
		[
			SNew(SSearchBox)
			.HintText(FText::FromString(TEXT("Search Assets")))
			.OnTextChanged(this, &SYDependencyAnalyserWidget::OnSearchBoxChanged)
			.OnTextCommitted(this, &SYDependencyAnalyserWidget::OnSearchBoxCommitted)
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.FillHeight(5)
		.Padding(5)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FLineData>>)
			.OnGenerateRow(this, &SYDependencyAnalyserWidget::OnGenerateLine)
			.ListItemsSource(&LinesData)
			.ItemHeight(14)
			.Visibility(EVisibility::Visible)
			.HeaderRow(
				SNew(SHeaderRow)
				
				+ SHeaderRow::Column(NAME_Name)
				.DefaultLabel(FText::FromName(NAME_Name))
				.OnSort(FOnSortModeChanged::CreateSP(this, &SYDependencyAnalyserWidget::OnSortColumnHeader))
				
				+ SHeaderRow::Column(NAME_DependenciesCount)
				.DefaultLabel(FText::FromName(NAME_DependenciesCount))
				.OnSort(FOnSortModeChanged::CreateSP(this, &SYDependencyAnalyserWidget::OnSortColumnHeader))

				+ SHeaderRow::Column(NAME_TotalSize)
				.DefaultLabel(FText::FromName(NAME_TotalSize))
				.OnSort(FOnSortModeChanged::CreateSP(this, &SYDependencyAnalyserWidget::OnSortColumnHeader))

				+ SHeaderRow::Column(NAME_Type)
				.DefaultLabel(FText::FromName(NAME_Type))
				.OnSort(FOnSortModeChanged::CreateSP(this, &SYDependencyAnalyserWidget::OnSortColumnHeader))
				
				+ SHeaderRow::Column(NAME_Path)
				.DefaultLabel(FText::FromName(NAME_Path))
				.OnSort(FOnSortModeChanged::CreateSP(this, &SYDependencyAnalyserWidget::OnSortColumnHeader))
			)
		]
		
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		.AutoHeight()
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.Padding(5, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Results"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			]

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.Padding(5, 0, 0, 0)
			[
				SAssignNew(ResultsTextBlock, STextBlock)
			]
		]
	];
}

FReply SYDependencyAnalyserWidget::OnRun()
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> Results = UYDependencyFunctionLibrary::RunAssetAudit(AssetRegistryModule);

	LinesData.Empty();

	int ErrorAssetsCount = 0;
	int WarningAssetsCount = 0;

	FScopedSlowTask ExplorePathsTask(Results.Num(), FText::FromString("Running..."));
	ExplorePathsTask.MakeDialog();

	for (int i = 0; i < Results.Num(); i++)
	{
		ExplorePathsTask.EnterProgressFrame();
		
		if (IgnoreDevFolders->IsChecked() && Results[i].PackageName.ToString().StartsWith("/Game/Developers/"))
		{
			continue;
		}

		FAssetData Result = Results[i];
		const DependenciesData dependencies = UYDependencyFunctionLibrary::GetDependencies(
			AssetRegistryModule,
			Result.PackageName,
			IncludeSoftRef->IsChecked(),
			IgnoreDevFolders->IsChecked());
		FLineData data = {Result.AssetName.ToString(), dependencies.Amount, dependencies.TotalSize, Result.AssetClass, Result.PackageName};
		LinesData.Add(MakeShared<FLineData>(data));

		if (UYDependencyFunctionLibrary::IsOverMBSize(dependencies.TotalSize, FCString::Atoi(*ErrorSize.Get()->GetText().ToString())))
		{
			ErrorAssetsCount++;
		}
		else if (UYDependencyFunctionLibrary::IsOverMBSize(dependencies.TotalSize, FCString::Atoi(*WarningSize.Get()->GetText().ToString())))
		{
			WarningAssetsCount++;
		}
	}

	GenerateResultText(LinesData.Num(), ErrorAssetsCount, WarningAssetsCount);

	auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
	{
		return B.Get()->DependenciesCount - A.Get()->DependenciesCount < 0.f;
	};
	LinesData.Sort(ReferenceCountSorter);
	
	LastRunResult = LinesData;

	ListView.Get()->RequestListRefresh();

	return FReply::Handled();
}

FReply SYDependencyAnalyserWidget::OnExport()
{
	TArray<FString> Lines;
	for (auto Line : LinesData)
	{
		FString data = FString::Printf(TEXT("%s, %d, %d, %s, %s,"),
			ToCStr(Line.Get()->Name),
			Line.Get()->DependenciesCount,
			static_cast<int32>(Line.Get()->TotalSize),
			ToCStr(Line.Get()->Type.ToString()),
			ToCStr(Line.Get()->Path.ToString()));
		Lines.Add(data);
	}

	const FString path = L"DependencyAnalysis/DependencyAnalysisResults.csv";
	bool success = FFileHelper::SaveStringArrayToFile(Lines, *path);
	
	if (!success)
	{
		UE_LOG(LogDependencyAnalyser, Error, TEXT("Could not save file to %s"), *path);
	}
	else
	{
		UE_LOG(LogDependencyAnalyser, Warning, TEXT("Saved to %s"), *path);
		FWindowsPlatformProcess::ExploreFolder(*path);
	}
	
	return FReply::Handled();
}

void SYDependencyAnalyserWidget::OnSearchBoxChanged(const FText& InSearchText)
{
	if (!InSearchText.IsEmptyOrWhitespace())
	{
		Filter = InSearchText;
	
		RefreshResults();
	}
	else
	{
		LinesData = LastRunResult;
	}
	
	ListView->RequestListRefresh();
}

void SYDependencyAnalyserWidget::OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo)
{
	if (!InSearchText.IsEmptyOrWhitespace())
	{
		Filter = InSearchText;
	
		RefreshResults();
	}
	else
	{
		LinesData = LastRunResult;
	}
	
	ListView->RequestListRefresh();
}

void SYDependencyAnalyserWidget::RefreshResults()
{
	LinesData.Empty();
	
	for (const auto& LineData : LastRunResult)
	{
		if (DoesPassFilter(LineData))
		{
			LinesData.Add(LineData);
		}
	}
}

void SYDependencyAnalyserWidget::GenerateResultText(int TotalAssetsCount, int ErrorAssetsCount, int WarningAssetsCount)
{
	const FString resultStr = FString::Printf(TEXT("There are %d assets larger than %dMB %d assets larger than %dMB, out of a total of %d assets."),
		ErrorAssetsCount,
		FCString::Atoi(*ErrorSize.Get()->GetText().ToString()),
		WarningAssetsCount,
		FCString::Atoi(*WarningSize.Get()->GetText().ToString()),
		TotalAssetsCount);

	ResultsTextBlock.Get()->SetText(FText::FromString(resultStr));
}

bool SYDependencyAnalyserWidget::DoesPassFilter(const TSharedPtr<FLineData, ESPMode::Fast>& LineData)
{
	if (LineData.Get()->Name.Contains(Filter.ToString()))
	{
		return true;
	}

	if (LineData.Get()->Type.ToString().Contains(Filter.ToString()))
	{
		return true;
	}

	return false;
}

TSharedRef<ITableRow> SYDependencyAnalyserWidget::OnGenerateLine(TSharedPtr<FLineData> Item, const TSharedRef<STableViewBase> &Table)
{
	return SNew(SYDependencyAnalyserResultRow, Table)
		.Item(Item)
		.IsWarningSize(UYDependencyFunctionLibrary::IsOverMBSize(Item->TotalSize, FCString::Atoi(*WarningSize.Get()->GetText().ToString())))
		.IsErrorSize(UYDependencyFunctionLibrary::IsOverMBSize(Item->TotalSize, FCString::Atoi(*ErrorSize.Get()->GetText().ToString())));
}

void SYDependencyAnalyserWidget::OnSortColumnHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode)
{
	if (ColumnName == NAME_Name)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return B.Get()->Name > A.Get()->Name;
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == NAME_DependenciesCount)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return B.Get()->DependenciesCount - A.Get()->DependenciesCount < 0.f;
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == NAME_TotalSize)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return static_cast<int32>(B.Get()->TotalSize) - static_cast<int32>(A.Get()->TotalSize) < 0.f;
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == NAME_Type)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return B.Get()->Type.ToString() > A.Get()->Type.ToString();
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == NAME_Path)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return B.Get()->Path.ToString() > A.Get()->Path.ToString();
		};
		LinesData.Sort(ReferenceCountSorter);
	}

	ListView.Get()->RequestListRefresh();
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
