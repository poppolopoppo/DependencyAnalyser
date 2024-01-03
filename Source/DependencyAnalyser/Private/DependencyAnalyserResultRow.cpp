// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyAnalyserResultRow.h"

#include "AssetManagerEditorModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

void SDependencyAnalyserResultRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView)
{
	Item = InArgs._Item;
	bIsWarningSize = InArgs._IsWarningSize;
	bIsErrorSize = InArgs._IsErrorSize;
	
	SMultiColumnTableRow::Construct(FSuperRowType::FArguments(), OwnerTableView);
}

TSharedRef<SWidget> SDependencyAnalyserResultRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	FText ColumnText;
	
	if (InColumnName == SDependencyAnalyserWidget::Name_Name)
	{
		ColumnText = FText::FromString(*Item->Name);
	}
	else if (InColumnName == SDependencyAnalyserWidget::Name_DependenciesCount)
	{
		ColumnText = FText::FromString(FString::FromInt(Item->DependenciesCount));
	}
	else if (InColumnName == SDependencyAnalyserWidget::Name_TotalSize)
	{
		ColumnText = GetSizeText(Item->TotalSize);
	}
	else if (InColumnName == SDependencyAnalyserWidget::Name_Type)
	{
		ColumnText = FText::FromString(Item->Class->GetName());
	}
	else if (InColumnName == SDependencyAnalyserWidget::Name_Path)
	{
		ColumnText = FText::FromName(Item->Path);
	}

	if (bIsErrorSize)
	{
		return SNew(SBorder)
		[
			SNew(STextBlock)
			.Text(ColumnText)
			.ColorAndOpacity(FLinearColor(FColor::Red))
			.OnDoubleClicked(this, &SDependencyAnalyserResultRow::OnDoubleClicked)
		]
		.OnMouseButtonDown(this, &SDependencyAnalyserResultRow::OnClicked);
	}
	else if (bIsWarningSize)
	{
		return SNew(SBorder)
		[
			SNew(STextBlock)
			.Text(ColumnText)
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
			.Text(ColumnText)
			.OnDoubleClicked(this, &SDependencyAnalyserResultRow::OnDoubleClicked)
		]
		.OnMouseButtonDown(this, &SDependencyAnalyserResultRow::OnClicked);
	}
}

FReply SDependencyAnalyserResultRow::OnDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent) const
{
	IAssetManagerEditorModule::Get().OpenSizeMapUI(TArray{Item->Path});

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
	IAssetManagerEditorModule::Get().OpenSizeMapUI(TArray{Item->Path});
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
		return FText::AsMemory(SizeInBytes, SI);
	}
	else
	{
		FNumberFormattingOptions NumberFormattingOptions;
		NumberFormattingOptions.MaximumFractionalDigits = 3;
		NumberFormattingOptions.MinimumFractionalDigits = 0;
		NumberFormattingOptions.MinimumIntegralDigits = 1;

		return FText::AsMemory(SizeInBytes, &NumberFormattingOptions, nullptr, SI);
	}
}
