// Copyright 2019 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SListViewSelectorDropdownMenu.h"
#include "Widgets/SCompoundWidget.h"
#include "YDependencyAnalyserResultRow.h"
#include "Widgets/Notifications/SProgressBar.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDependencyAnalyser, All, All);

struct YDEPENDENCYANALYSER_API FLineData
{
	FString Name;
	int32 DependenciesCount;
	SIZE_T TotalSize;
	FName Type;
	FName Path;
};

class YDEPENDENCYANALYSER_API SYDependencyAnalyserWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SYDependencyAnalyserWidget) {}
	SLATE_END_ARGS()

	static FName NAME_Name;
	static FName NAME_DependenciesCount;
	static FName NAME_TotalSize;
	static FName NAME_Type;
	static FName NAME_Path;

	void Construct(const FArguments& InArgs);

private:
	FReply OnRun();
	FReply OnExport();
	void RefreshResults();
	void GenerateResultText(int TotalAssetsCount, int ErrorAssetsCount, int WarningAssetsCount);
	void OnSearchBoxChanged(const FText& InSearchText);
	void OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo);
	void OnSortColumnHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode);

	bool DoesPassFilter(const TSharedPtr<FLineData, ESPMode::Fast>& LineData);
	
	TSharedRef<ITableRow> OnGenerateLine(TSharedPtr<FLineData> Item, const TSharedRef<STableViewBase> &myTable);

	TSharedPtr<SCheckBox> IncludeSoftRef;
	TSharedPtr<SCheckBox> IgnoreDevFolders;
	TSharedPtr<SEditableTextBox> WarningSize;
	TSharedPtr<SEditableTextBox> ErrorSize;
	TSharedPtr<SHeaderRow> HeaderRow;
	TSharedPtr<STextBlock> ResultsTextBlock;
	TSharedPtr<SListView<TSharedPtr<FLineData>>> ListView;
	TArray<TSharedPtr<FLineData>> LinesData;
	TArray<TSharedPtr<FLineData>> LastRunResult;

	FText Filter;

	int DefaultWarningSize = 50;
	int DefaultErrorSize = 500;
};
