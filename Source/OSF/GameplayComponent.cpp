// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "GameplayComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UGameplayComponent::UGameplayComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGameplayComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGameplayComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

//ABallsack* GetBall()
//{
//    for (TActorIterator<ABallsack> ballItr(GetWorld()); ballItr; ballItr++)
//    {
//        
//    }
//}

static float MIN_DISTANCE_FOR_TOUCH = 30;

bool UGameplayComponent::CanKickBall(ACharacter *character, AActor *ball) {
    FVector distance = character->GetActorLocation() - ball->GetActorLocation();
    distance.Z = 0;
    return distance.Size() < MIN_DISTANCE_FOR_TOUCH;
}

void UGameplayComponent::MoveToBallForKick(ACharacter *character, AActor *target, FVector desiredEndDirection, float deltaSeconds)
{
    FVector charPos = character->GetActorLocation();
    FVector targetPos = target->GetActorLocation();
    
    FVector desiredPos = targetPos - desiredEndDirection * 100;
    desiredPos.Z = charPos.Z;
    
    FVector movementVector = desiredPos - charPos;
    movementVector.Normalize();
    
    //    DrawDebugLine(GetWorld(), charPos, charPos + movementVector * 100, FColor::Red, true, 5000, 10);
    
    movementVector *= deltaSeconds * 300; // 2 m/s
    character->SetActorLocation(charPos + movementVector);
    
    FRotator rot = UKismetMathLibrary::FindLookAtRotation(charPos, targetPos);
    rot.Pitch = 0;
    rot.Roll = 0; // don't want the player to levitate looking down at the ball :|
    character->SetActorRotation(rot);
}

