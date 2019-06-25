// Fill out your copyright notice in the Description page of Project Settings.


#include "UINavHorizontalComponent.h"


void UUINavHorizontalComponent::NavigateLeft()
{
	OnNavigateLeft();
}

void UUINavHorizontalComponent::NavigateRight()
{
	OnNavigateRight();
}

void UUINavHorizontalComponent::Update()
{
}


void UUINavHorizontalComponent::UpdateTextToIndex(int NewIndex)
{
	OptionIndex = NewIndex;
	Update();
}

void UUINavHorizontalComponent::ChangeText(FText NewText)
{
	NavText->SetText(NewText);
}

void UUINavHorizontalComponent::OnNavigateLeft_Implementation()
{
}

void UUINavHorizontalComponent::OnNavigateRight_Implementation()
{
}