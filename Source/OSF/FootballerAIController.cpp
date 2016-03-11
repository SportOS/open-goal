// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "FootballerAIController.h"

void AFootballerAIController::BeginPlay()
{
    Super::BeginPlay();
}

void AFootballerAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    //AddMovementInput(FVector::ZeroVector);
}
