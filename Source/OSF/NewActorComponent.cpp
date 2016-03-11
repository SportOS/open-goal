// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "NewActorComponent.h"
#include "EngineUtils.h"


// Sets default values for this component's properties
UNewActorComponent::UNewActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNewActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


//static ConstructorHelpers::FObjectFinder<AActor> BallActor(TEXT("Content/Blueprints/Ball"));

// Called every frame
void UNewActorComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
//    for (TActorIterator<BallActor> Itr; Itr; ++Itr) {
//        ClientMessage(Itr->GetName());
//        ClientMessage(Itr->GetClass()->GetDesc());
//    }
    
}

