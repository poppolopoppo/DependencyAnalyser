// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDependencyAnalyser, All, All);

struct DEPENDENCYANALYSER_API FLineData
{
	FString Name;
	int32 DependenciesCount;
	SIZE_T TotalSize;
	UClass* Class;
	FName Path;
};

class DEPENDENCYANALYSER_API SDependencyAnalyserWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDependencyAnalyserWidget) {}
	SLATE_END_ARGS()

	static FName Name_Name;
	static FName Name_DependenciesCount;
	static FName Name_TotalSize;
	static FName Name_Type;
	static FName Name_Path;

	void LoadSettings();
	void Construct(const FArguments& InArgs);

private:
	FReply OnRun();
	FReply OnExport();
	void RefreshResults();
	void GenerateResultText(int TotalAssetsCount, int ErrorAssetsCount, int WarningAssetsCount);
	void OnSearchBoxChanged(const FText& InSearchText);
	void OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo);
	void OnSortColumnHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode);

	bool DoesPassFilter(const TSharedPtr<FLineData, ESPMode::ThreadSafe>& LineData);
	
	TSharedRef<ITableRow> OnGenerateLine(TSharedPtr<FLineData> Item, const TSharedRef<STableViewBase> &MyTable);

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
};
