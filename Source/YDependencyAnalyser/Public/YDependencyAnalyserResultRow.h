// Copyright 2019 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YDependencyAnalyserWidget.h"
#include "Components/Widget.h"

struct FLineData;

class YDEPENDENCYANALYSER_API SYDependencyAnalyserResultRow : public SMultiColumnTableRow<TSharedPtr<FLineData>>
{
public:
	SLATE_BEGIN_ARGS(SYDependencyAnalyserResultRow) {}
	SLATE_ARGUMENT(TSharedPtr<FLineData>, Item)
	SLATE_ARGUMENT(bool, IsWarningSize)
	SLATE_ARGUMENT(bool, IsErrorSize)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView);
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	FReply OnDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent) const;
	FText GetSizeText(const SIZE_T SizeInBytes) const;
	
	TSharedPtr<FLineData> Item;
	bool IsWarningSize = false;
	bool IsErrorSize = false;
};
