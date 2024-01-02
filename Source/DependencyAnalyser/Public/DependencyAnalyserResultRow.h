// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DependencyAnalyserWidget.h"

struct FLineData;

class DEPENDENCYANALYSER_API SDependencyAnalyserResultRow : public SMultiColumnTableRow<TSharedPtr<FLineData>>
{
public:
	SLATE_BEGIN_ARGS(SDependencyAnalyserResultRow) {}
	SLATE_ARGUMENT(TSharedPtr<FLineData>, Item)
	SLATE_ARGUMENT(bool, IsWarningSize)
	SLATE_ARGUMENT(bool, IsErrorSize)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView);
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	

private:
	FReply OnDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent) const;
	FReply OnClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	void CreateContextMenu(const FVector2D& MousePosition);
	void OpenSizeMap() const;
	void EditAsset() const;
	void BrowseAsset() const;
	FText GetSizeText(const SIZE_T SizeInBytes) const;
	
	TSharedPtr<FLineData> Item;
	bool IsWarningSize = false;
	bool IsErrorSize = false;
};
