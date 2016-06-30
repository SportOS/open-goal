// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "FootballTeam.h"


// Sets default values
AFootballTeam::AFootballTeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFootballTeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFootballTeam::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

AFootballer* AFootballTeam::GetClosestFootballerToBall(ABallsack* ball)
{
    AFootballer* closestFootballer = nullptr;
	float closestDistance = FLT_MAX;
    for (AFootballer* footballer: Footballers) {
        float distance = footballer->GetDistanceTo(ball);
        if (distance < closestDistance) {
            closestFootballer = footballer;
            closestDistance = distance;
        }
    }
    return closestFootballer;
}