// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Widgets/Views/SListView.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDependencyAnalyser, All, All);

class SCheckBox;
class SEditableTextBox;
class STextBlock;

// Data for each result row of an analysis
struct DEPENDENCYANALYSER_API FLineData
{
	FString Name;
	int32 DependenciesCount;
	SIZE_T DiskSize;
	SIZE_T MemorySize;
	UObject* Object;
	UClass* Class;
	FName Type;
	FName Path;
};

/* Manages the behaviour of the dependency analysis widget when responding to user input */
class DEPENDENCYANALYSER_API SDependencyAnalyserWidget final : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SDependencyAnalyserWidget) {}
	SLATE_END_ARGS()

	static FName Name_Name;
	static FName Name_DependenciesCount;
	static FName Name_DiskSize;
	static FName Name_MemorySize;
	static FName Name_Type;
	static FName Name_Path;

	void Construct(const FArguments& InArgs);

private:

	// Called when an analysis is run
	FReply OnRun();

	// Called when a request to export analysis results is made
	FReply OnExport();

	// Flushes and repopulates result lines
	void RefreshResults();

	// Generates textual overview of results of an analysis
	void GenerateResultText(const int32 TotalAssetsCount, const int32 ErrorAssetsCount, const int32 WarningAssetsCount);

	// Handle behaviour of search box input
	void OnSearchBoxChanged(const FText& InSearchText);
	void OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo);
	bool DoesPassFilter(const TSharedPtr<FLineData, ESPMode::ThreadSafe>& LineData);
	FText Filter;

	// Sorts results based on user header selection
	void OnSortColumnHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode);

	// Handles generation of each line of results
	TSharedRef<ITableRow> OnGenerateLine(TSharedPtr<FLineData> Item, const TSharedRef<STableViewBase> &Table);

	TSharedPtr<SCheckBox> IncludeSoftRef;
	TSharedPtr<SCheckBox> IgnoreDevFolders;
	TSharedPtr<SCheckBox> IgnoreExternalActors;
	TSharedPtr<SCheckBox> IgnoreExternalObjects;
	TSharedPtr<SEditableTextBox> WarningSize;
	TSharedPtr<SEditableTextBox> ErrorSize;
	TSharedPtr<SHeaderRow> HeaderRow;
	TSharedPtr<STextBlock> ResultsTextBlock;
	TSharedPtr<SListView<TSharedPtr<FLineData>>> ListView;
	TArray<TSharedPtr<FLineData>> LinesData;
	TArray<TSharedPtr<FLineData>> LastRunResult;
};
