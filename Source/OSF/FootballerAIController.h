// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "FootballerAIController.generated.h"

/**
 * 
 */
UCLASS()
class OSF_API AFootballerAIController : public AAIController
{
	GENERATED_BODY()
	
    virtual void BeginPlay() override;
    
    virtual void Tick(float DeltaSeconds) override;
	
	
};
