// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Data/AxisType.h"
#include "Data/InputRestriction.h"
#include "Data/RevertRebindReason.h"
#include "Data/TargetColumn.h"
#include "Blueprint/UserWidget.h"
#include "UINavInputContainer.generated.h"

/**
* This class contains the logic for aggregating several input boxes
*/

UCLASS()
class UINAVIGATION_API UUINavInputContainer : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	void SetupInputBoxes();
	void CreateInputBoxes();

	//-----------------------------------------------------------------------

	TMap<FKey, FKey> KeyToAxisMap = {
		{EKeys::Gamepad_LeftTrigger, EKeys::Gamepad_LeftTriggerAxis},
		{EKeys::Gamepad_RightTrigger, EKeys::Gamepad_RightTriggerAxis},
		{EKeys::MotionController_Left_Trigger, EKeys::MotionController_Left_TriggerAxis},
		{EKeys::MotionController_Left_Grip1, EKeys::MotionController_Left_Grip1Axis},
		{EKeys::MotionController_Left_Grip2, EKeys::MotionController_Left_Grip2Axis},
		{EKeys::MotionController_Right_Trigger, EKeys::MotionController_Right_TriggerAxis},
		{EKeys::MotionController_Right_Grip1, EKeys::MotionController_Right_Grip1Axis},
		{EKeys::MotionController_Right_Grip2, EKeys::MotionController_Right_Grip2Axis},
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UINav Input")
		TSubclassOf<class UUINavInputBox> InputBox_BP;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "UINav Input")
		class UPanelWidget* InputBoxesPanel;
	
	UPROPERTY(BlueprintReadOnly, Category = "UINav Input")
		class UUINavWidget* ParentWidget;

public:

	void Init(class UUINavWidget* NewParent);

	/**
	*	Called when this widget completed setting up InputBoxes
	*/
	UFUNCTION(BlueprintNativeEvent, Category = UINavWidget)
		void OnSetupCompleted();
	virtual void OnSetupCompleted_Implementation();

	/**
	*	Called when a new input box is added
	*/
	UFUNCTION(BlueprintNativeEvent, Category = UINavWidget)
		void OnAddInputBox(class UUINavInputBox* NewInputBox);
	virtual void OnAddInputBox_Implementation(class UUINavInputBox* NewInputBox);

	/*
	*	Called when a rebind was cancelled, specifying the reason for the revert.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = UINavWidget)
		void OnRebindCancelled(ERevertRebindReason RevertReason, FKey PressedKey);
	virtual void OnRebindCancelled_Implementation(ERevertRebindReason RevertReason, FKey PressedKey);

	UFUNCTION(BlueprintCallable, Category = "UINav Input")
		void ResetKeyMappings();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UINav Input")
		bool CanUseKey(const class UUINavInputBox* InputBox, FKey CompareKey) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UINav Input")
		bool RespectsRestriction(FKey CompareKey, int Index);

	void ResetInputBox(FName InputName, EAxisType AxisType);

	//Fetches the index offset from the TargetColumn variable for both the top and bottom of the Input Container
	int GetOffsetFromTargetColumn(bool bTop);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UINav Input")
		FKey GetAxisFromKey(FKey Key);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UINav Input")
		FORCEINLINE ETargetColumn GetTargetColumn() const { return TargetColumn; }

	//-----------------------------------------------------------------------

	class UUINavPCComponent* UINavPC = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UINav Input")
		int NumberOfInputs = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UINav Input")
		int KeysPerInput = 0;

	//The index of the button in the top left corner of the grid
	UPROPERTY(BlueprintReadOnly, Category = "UINav Input")
		int FirstButtonIndex = -1;
	//The index of the button in the bottom right corner of the grid
	UPROPERTY(BlueprintReadOnly, Category = "UINav Input")
		int LastButtonIndex = -1;

	/*The index of the button at the top of the grid that should be navigated to
	when entering this grid*/
	UPROPERTY(BlueprintReadOnly, Category = "UINav Input")
		int TopButtonIndex = -1;
	/*The index of the button at the bottom of the grid that should be navigated to
	when entering this grid*/
	UPROPERTY(BlueprintReadOnly, Category = "UINav Input")
		int BottomButtonIndex = -1;

	/*
	The names of the desired actions and axes to allow for rebinding.
	If you want to rebind axes, you have to specify whether they're
	the positive or negative axis by suffixing the axis name with
	either "+" or "-"
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UINav Input")
		TArray<FName> InputNames;

	/*
	The restrictions for the type of input associated with each column
	in the Input Container
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UINav Input")
		TArray<EInputRestriction> InputRestrictions;

	/*
	A list of the keys that the player shouldn't be able to use for the inputs
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UINav Input")
		TArray<FKey> KeyBlacklist =
		{
			EKeys::Escape,
			EKeys::LeftCommand,
			EKeys::RightCommand,
		};

	/*
	Indicates whether unused input boxes will hidden or collapsed
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UINav Input")
		bool bCollapseInputBoxes = false;

	//Indicates which column to navigate to when navigating to this Input Container
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UINav Input")
		ETargetColumn TargetColumn = ETargetColumn::Left;

	//The text used for empty key buttons
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UINav Input")
		FText EmptyKeyText = FText::FromString(TEXT("Unbound"));

	//The text used for notifying the player to press a key
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UINav Input")
		FText PressKeyText = FText::FromString(TEXT("Press Any Key"));

};
