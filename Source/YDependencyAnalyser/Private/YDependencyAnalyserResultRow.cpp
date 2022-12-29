// Copyright 2019 YAGER Development GmbH All Rights Reserved.


#include "YDependencyAnalyserResultRow.h"

#include "AssetManagerEditorModule.h"

void SYDependencyAnalyserResultRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Item = InArgs._Item;
	IsWarningSize = InArgs._IsWarningSize;
	IsErrorSize = InArgs._IsErrorSize;
	
	SMultiColumnTableRow<TSharedPtr<FLineData>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SYDependencyAnalyserResultRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	FText columnText;
	
	if (ColumnName == SYDependencyAnalyserWidget::NAME_Name)
	{
		columnText = FText::FromString(*Item->Name);
	}
	else if (ColumnName == SYDependencyAnalyserWidget::NAME_DependenciesCount)
	{
		columnText = FText::FromString(FString::FromInt(Item->DependenciesCount));
	}
	else if (ColumnName == SYDependencyAnalyserWidget::NAME_TotalSize)
	{
		columnText = GetSizeText(Item->TotalSize);
	}
	else if (ColumnName == SYDependencyAnalyserWidget::NAME_Type)
	{
		columnText = FText::FromName(Item->Type);
	}
	else if (ColumnName == SYDependencyAnalyserWidget::NAME_Path)
	{
		columnText = FText::FromName(Item->Path);
	}

	if (IsErrorSize)
	{
		return SNew(SBox)
		[
			SNew(STextBlock)
			.Text(columnText)
			.ColorAndOpacity(FLinearColor(FColor::Red))
			.OnDoubleClicked(this, &SYDependencyAnalyserResultRow::OnDoubleClicked)
		];
	}
	else if (IsWarningSize)
	{
		return SNew(SBox)
		[
			SNew(STextBlock)
			.Text(columnText)
			.ColorAndOpacity(FLinearColor(FColor::Orange))
			.OnDoubleClicked(this, &SYDependencyAnalyserResultRow::OnDoubleClicked)
		];
	}
	else
	{
		return SNew(SBox)
		[
			SNew(STextBlock)
			.Text(columnText)
			.OnDoubleClicked(this, &SYDependencyAnalyserResultRow::OnDoubleClicked)
		];
	}
}

FReply SYDependencyAnalyserResultRow::OnDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent) const
{
	IAssetManagerEditorModule::Get().OpenSizeMapUI(TArray<FName> {Item->Path});

	return FReply::Handled();
}

FText SYDependencyAnalyserResultRow::GetSizeText(const SIZE_T SizeInBytes) const
{
	if (SizeInBytes < 1000)
	{
		return FText::AsMemory(SizeInBytes, EMemoryUnitStandard::SI);
	}
	else
	{
		FNumberFormattingOptions NumberFormattingOptions;
		NumberFormattingOptions.MaximumFractionalDigits = 3;
		NumberFormattingOptions.MinimumFractionalDigits = 0;
		NumberFormattingOptions.MinimumIntegralDigits = 1;

		return FText::AsMemory(SizeInBytes, &NumberFormattingOptions, nullptr, EMemoryUnitStandard::SI);
	}
}
