// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "Ballsack.h"


// Sets default values
ABallsack::ABallsack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABallsack::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABallsack::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

ABallsack* ABallsack::GetWorldBall(UWorld *world)
{
    TActorIterator<ABallsack> actorIterator(world);
    for (; actorIterator; ++actorIterator) {
        return *actorIterator;
    }
    return nullptr;
}
