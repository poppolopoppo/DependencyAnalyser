// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyAnalyserResultRow.h"

#include "AssetManagerEditorModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

void SDependencyAnalyserResultRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Item = InArgs._Item;
	IsWarningSize = InArgs._IsWarningSize;
	IsErrorSize = InArgs._IsErrorSize;
	
	SMultiColumnTableRow<TSharedPtr<FLineData>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SDependencyAnalyserResultRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	FText columnText;
	
	if (ColumnName == SDependencyAnalyserWidget::Name_Name)
	{
		columnText = FText::FromString(*Item->Name);
	}
	else if (ColumnName == SDependencyAnalyserWidget::Name_DependenciesCount)
	{
		columnText = FText::FromString(FString::FromInt(Item->DependenciesCount));
	}
	else if (ColumnName == SDependencyAnalyserWidget::Name_TotalSize)
	{
		columnText = GetSizeText(Item->TotalSize);
	}
	else if (ColumnName == SDependencyAnalyserWidget::Name_Type)
	{
		columnText = FText::FromString(Item->Class->GetName());
	}
	else if (ColumnName == SDependencyAnalyserWidget::Name_Path)
	{
		columnText = FText::FromName(Item->Path);
	}

	if (IsErrorSize)
	{
		return SNew(SBorder)
		[
			SNew(STextBlock)
			.Text(columnText)
			.ColorAndOpacity(FLinearColor(FColor::Red))
			.OnDoubleClicked(this, &SDependencyAnalyserResultRow::OnDoubleClicked)
		]
		.OnMouseButtonDown(this, &SDependencyAnalyserResultRow::OnClicked);
	}
	else if (IsWarningSize)
	{
		return SNew(SBorder)
		[
			SNew(STextBlock)
			.Text(columnText)
			.ColorAndOpacity(FLinearColor(FColor::Orange))
			.OnDoubleClicked(this, &SDependencyAnalyserResultRow::OnDoubleClicked)
		]
		.OnMouseButtonDown(this, &SDependencyAnalyserResultRow::OnClicked);
	}
	else
	{
		return SNew(SBorder)
		[
			SNew(STextBlock)
			.Text(columnText)
			.OnDoubleClicked(this, &SDependencyAnalyserResultRow::OnDoubleClicked)
		]
		.OnMouseButtonDown(this, &SDependencyAnalyserResultRow::OnClicked);
	}
}

FReply SDependencyAnalyserResultRow::OnDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent) const
{
	IAssetManagerEditorModule::Get().OpenSizeMapUI(TArray<FName> {Item->Path});

	return FReply::Handled();
}

FReply SDependencyAnalyserResultRow::OnClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	if (PointerEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		CreateContextMenu(PointerEvent.GetScreenSpacePosition());
	}
	
	return FReply::Unhandled();
}

void SDependencyAnalyserResultRow::CreateContextMenu(const FVector2D& MousePosition)
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	MenuBuilder.AddMenuEntry(FText::FromString("Open Size Map"),
	   FText::FromString("Open Size Map for this asset"),
	   FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetActions.OpenInExternalEditor"),
	   FUIAction(FExecuteAction::CreateSP(this, &SDependencyAnalyserResultRow::OpenSizeMap)));
	
	MenuBuilder.AddMenuEntry(FText::FromString("Edit Asset"),
	   FText::FromString("Edit this asset"),
	   FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetActions.Edit"),
	   FUIAction(FExecuteAction::CreateSP(this, &SDependencyAnalyserResultRow::EditAsset)));

	MenuBuilder.AddMenuEntry(FText::FromString("Find in Content Browser"),
	   FText::FromString("Find this asset in Content Browser"),
	   FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetActions.GenericFind"),
	   FUIAction(FExecuteAction::CreateSP(this, &SDependencyAnalyserResultRow::BrowseAsset)));

	FSlateApplication::Get().PushMenu(
	   SharedThis(this),
	   FWidgetPath(),
	   MenuBuilder.MakeWidget(),
	   MousePosition,
	   FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
}

void SDependencyAnalyserResultRow::OpenSizeMap() const
{
	IAssetManagerEditorModule::Get().OpenSizeMapUI(TArray<FName>{Item->Path});
}

void SDependencyAnalyserResultRow::EditAsset() const
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Item->Path.ToString());
}

void SDependencyAnalyserResultRow::BrowseAsset() const
{
	UObject* SelectedAsset = LoadObject<UObject>(nullptr, *Item->Path.ToString());
	const TArray<FAssetData>& Assets = {SelectedAsset};
	const FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToAssets(Assets);
}

FText SDependencyAnalyserResultRow::GetSizeText(const SIZE_T SizeInBytes) const
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
