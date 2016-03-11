// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "TestCppComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UTestCppComponent::UTestCppComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTestCppComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTestCppComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}



float MIN_DISTANCE_FOR_TOUCH = 10;
void UTestCppComponent::MoveAlongSpiralPath(ACharacter *character, AActor *target, FVector desiredEndDirection, float deltaSeconds)
{
    FVector charPos = character->GetActorLocation();
    FVector targetPos = target->GetActorLocation();

//    FVector distanceToTarget = targetPos - charPos;
//    distanceToTarget.Z = 0; // ignore z movement for now
//    
//    float distance = distanceToTarget.Size();
////    distanceToTarget *= 0.5; // move slightly closer to the ball :>
//    float currentAngleDegrees = FMath::Atan2(distanceToTarget.Y, distanceToTarget.X);
//    
//    float desiredAngleDegrees = FMath::Atan2(desiredEndDirection.Y, desiredEndDirection.X);
//    
//    // positive difference = go CCW
//    float degreesDifference = desiredAngleDegrees - currentAngleDegrees;
//    
//    FVector movementVector;
//    if (FMath::Abs(degreesDifference) <= 1) {
//        if (distance < 200) {
//            return;
//        }
//        
//        // special case; move toward the ball.
//        movementVector = distanceToTarget;
//    } else if (degreesDifference < 0) {
//        
//        // go clockwise
//        // left handed coordinate system, so positive Z is CCW, negative Z is clockwise
//        FVector negativeZAxis = FVector(0, 0, -1);
//        movementVector = FVector::CrossProduct(distanc0eToTarget, negativeZAxis);
//        movementVector = movementVector.RotateAngleAxis(degreesDifference * deltaSeconds * 90, FVector(0, 0, 1));
//        
//        movementVector += distanceToTarget /
//    } else { // difference > 0, ccw
//        FVector positiveZAxis = FVector(0, 0, 1);
//        movementVector = FVector::CrossProduct(distanceToTarget, positiveZAxis);
//        movementVector = movementVector.RotateAngleAxis(degreesDifference * deltaSeconds * 90, FVector(0, 0, 1));
//    }
//    movementVector.Normalize();
    
    FVector desiredPos = targetPos - desiredEndDirection * 100;
    desiredPos.Z = charPos.Z;
    
    FVector movementVector = desiredPos - charPos;
    movementVector.Normalize();
    
//    DrawDebugLine(GetWorld(), charPos, charPos + movementVector * 100, FColor::Red, true, 5000, 10);

    movementVector *= deltaSeconds * 300; // 2 m/s
//    UE_LOG(LogTemp, Warning, TEXT("Fuck"));
//    character->SetActorLocation(charPos + movementVector);
    character->SetActorLocation(charPos + movementVector);
    
    FRotator rot = UKismetMathLibrary::FindLookAtRotation(charPos, targetPos);
    rot.Pitch = 0;
    rot.Roll = 0; // don't want the player to levitate looking down at the ball :|
    character->SetActorRotation(rot);
    //    character->SetActorLocation(targetpos);
    //    float dotproduct = charpos | targetpos;
    //
    //    FVector direction;
    //    float length;
    //    charpos.ToDirectionAndLength(direction, length);
}

