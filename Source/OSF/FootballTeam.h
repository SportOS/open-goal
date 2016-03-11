// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Footballer.h"
#include "FootballTeam.generated.h"

UCLASS()
class OSF_API AFootballTeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFootballTeam();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    TArray<AFootballer*> Footballers;
    
    UFUNCTION(BlueprintCallable, Category=Custom)
    AFootballer* GetClosestFootballerToBall(ABallsack* ball);
	
};
