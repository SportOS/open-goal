// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "FootballTeam.h"
#include "TeamGameState.generated.h"

/**
 * 
 */
UCLASS()
class OSF_API ATeamGameState : public AGameState
{
	GENERATED_BODY()
	
public:
    ATeamGameState();
    
    virtual void BeginPlay() override;
    
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
    
    void LoadSampleState();
    
    UPROPERTY(Replicated)
    AFootballTeam* HomeTeam;
    
    UPROPERTY(Replicated)
    AFootballTeam* AwayTeam;
    
    UPROPERTY(Replicated)
    ABallsack* Ball;
    
private:
    AFootballer* PlayerFromProperties(AFootballTeam* team, int index, FString name);
	
};

UClass* GetAIControllerClass();