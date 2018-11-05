// Fill out your copyright notice in the Description page of Project Settings.

#include "UINavInputBox.h"
#include "UINavController.h"
#include "UINavInputComponent.h"
#include "UINavInputContainer.h"
#include "UINavSettings.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UUINavInputBox::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;

	BuildKeyMappings();
}

void UUINavInputBox::BuildKeyMappings()
{
	const UInputSettings* Settings = GetDefault<UInputSettings>();
	TArray<FInputActionKeyMapping> TempActions;

	ActionText->SetText(FText::FromString(ActionName));

	Settings->GetActionMappingByName(FName(*ActionName), TempActions);

	AUINavController* PC = Cast<AUINavController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (TempActions.Num() == 0)
	{
		DISPLAYERROR(TEXT("Couldn't find Action with given name."));
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		UUINavInputComponent* NewInputButton = i == 0 ? InputButton1 : (i == 1 ? InputButton2 : InputButton3);
		InputButtons.Add(NewInputButton);

		if (i < InputsPerAction)
		{
			if (i < TempActions.Num())
			{
				FInputActionKeyMapping NewAction = TempActions[TempActions.Num() - 1 - i];
				Keys.Add(NewAction.Key);

				if (UpdateKeyIconForKey(NewAction.Key, i))
				{
					bUsingKeyImage[i] = true;
					NewInputButton->InputImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					NewInputButton->NavText->SetVisibility(ESlateVisibility::Collapsed);
				}
				NewInputButton->NavText->SetText(NewAction.Key.GetDisplayName());
			}
			else NewInputButton->NavText->SetText(FText::FromName(FName(TEXT("Unbound"))));
		}
		else
		{
			NewInputButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UUINavInputBox::ResetKeyMappings()
{
	Keys.Empty();
	BuildKeyMappings();
}

void UUINavInputBox::UpdateActionKey(FInputActionKeyMapping NewAction, int Index)
{
	if (!ShouldRegisterKey(NewAction.Key))
	{
		RevertToActionText(Index);
		return;
	}

	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;

	int Found = 0;
	bool bFound = false;
	for (int i = 0; i < Actions.Num(); i++)
	{
		if (Actions[i].ActionName.IsEqual(FName(*ActionName)))
		{
			if (Found == Index)
			{
				Actions[Index].Key = NewAction.Key;
				Keys[Index] = NewAction.Key;
				InputButtons[Index]->NavText->SetText(NewAction.Key.GetDisplayName());
				bFound = true;
				break;
			}
			Found++;
		}
	}
	if (!bFound)
	{
		FInputActionKeyMapping Action = NewAction;
		Action.ActionName = FName(*ActionName);
		Settings->AddActionMapping(Action, true);
		//Settings->ActionMappings.Add(Action);
		Keys[Index] = NewAction.Key;
		InputButtons[Index]->NavText->SetText(Action.Key.GetDisplayName());
	}

	CheckKeyIcon(NewAction.Key, Index);

	Settings->SaveConfig();
	Settings->ForceRebuildKeymaps();

	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

bool UUINavInputBox::ShouldRegisterKey(FKey NewKey) const
{
	return !(Container->IsKeyBeingUsed(NewKey));
}

bool UUINavInputBox::UpdateKeyIconForKey(FKey Key, int Index)
{
	FInputIconMapping* KeyIcon = nullptr;

	if (Key.IsGamepadKey())
	{
		if (Container->GamepadKeyData != nullptr && Container->GamepadKeyData->RowMap.Contains(Key.GetFName()))
		{
			KeyIcon = (FInputIconMapping*)Container->GamepadKeyData->RowMap[Key.GetFName()];
		}
	}
	else
	{
		if (Container->KeyboardMouseKeyData != nullptr && Container->KeyboardMouseKeyData->RowMap.Contains(Key.GetFName()))
		{
			KeyIcon = (FInputIconMapping*)Container->KeyboardMouseKeyData->RowMap[Key.GetFName()];
		}
	}
	if (KeyIcon == nullptr) return false;

	UTexture2D* NewTexture = KeyIcon->InputIcon.LoadSynchronous();
	if (NewTexture != nullptr)
	{
		InputButtons[Index]->InputImage->SetBrushFromTexture(NewTexture);
		return true;
	}
	return false;
}

void UUINavInputBox::CheckKeyIcon(FKey Key, int Index)
{
	if (UpdateKeyIconForKey(Key, Index) != bUsingKeyImage[Index]) bUsingKeyImage[Index] = !bUsingKeyImage[Index];

	if (bUsingKeyImage[Index])
	{
		InputButtons[Index]->InputImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		InputButtons[Index]->NavText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUINavInputBox::RevertToActionText(int Index)
{ 
	FText OldName;
	if (Index < Keys.Num())
	{
		OldName = Keys[Index].GetDisplayName();
		CheckKeyIcon(Keys[Index], Index);
	}
	else
	{
		OldName = FText::FromName(FName(TEXT("Unbound")));
	}

	InputButtons[Index]->NavText->SetText(OldName);
}

void UUINavInputBox::NotifySelected(int Index)
{
	FName NewName = FName("Press Any Key");

	InputButtons[Index]->NavText->SetText(FText::FromName(NewName));

	if (bUsingKeyImage[Index])
	{
		InputButtons[Index]->InputImage->SetVisibility(ESlateVisibility::Collapsed);
		InputButtons[Index]->NavText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UUINavInputBox::NotifyUnbound(int Index)
{
	FName NewName = FName("Unbound");

	InputButtons[Index]->NavText->SetText(FText::FromName(NewName));
}

bool UUINavInputBox::ContainsKey(FKey CompareKey) const
{
	return Keys.Contains(CompareKey);
}

