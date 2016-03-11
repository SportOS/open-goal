// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"

class AFootballerController;

#include "DefaultGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OSF_API ADefaultGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    ADefaultGameMode();
    
//    virtual void InitGameState() override;
    
    virtual void BeginPlay() override;
    
    virtual void StartPlay() override;
	
    virtual void StartMatch() override;
    
    bool NextControllerIsHome;
    
    bool IsSetup;
    
    void PlaceController(AFootballerController* controller);
	
};
