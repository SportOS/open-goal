// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "Footballer.h"
#include "FootballerController.h"
#include "FootballerAIController.h"
#include "Goal.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#define _USE_MATH_DEFINES
#include <cmath>

AFootballer::FootballAttributeInfo FA_Sample_Walcott() {
    AFootballer::FootballAttributeInfo attrs;
    attrs.SprintSpeed = 95;
    return attrs;
}

AFootballer::FootballAttributeInfo FA_Sample_Oezil() {
    AFootballer::FootballAttributeInfo attrs;
    attrs.SprintSpeed = 75;
    return attrs;
}

AFootballer::FootballAttributeInfo FA_Sample_Arteta() {
    AFootballer::FootballAttributeInfo attrs;
    attrs.SprintSpeed = 40;
    return attrs;
}

// Sets default values
AFootballer::AFootballer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    FootballAttributes = FA_Sample_Walcott();
}

void AFootballer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AFootballer, DesiredSprintStrength);
    DOREPLIFETIME(AFootballer, DesiredMovement);
    DOREPLIFETIME(AFootballer, Ball);
    DOREPLIFETIME(AFootballer, GoingForPossession);
    DOREPLIFETIME(AFootballer, ThinksHasPossession);
    DOREPLIFETIME(AFootballer, WaitingForPass);
    DOREPLIFETIME(AFootballer, JustKickedBall);
    DOREPLIFETIME(AFootballer, LastKickTime);
    DOREPLIFETIME(AFootballer, FootballerController);
//    DOREPLIFETIME(AFootballer, FootballAttributes);
    DOREPLIFETIME(AFootballer, DisplayName);
    DOREPLIFETIME(AFootballer, Team);
    DOREPLIFETIME(AFootballer, AIController);
//    DOREPLIFETIME(AFootballer, PendingAction);
    DOREPLIFETIME(AFootballer, ControlledByPlayer);
    DOREPLIFETIME(AFootballer, CachedTeammates);
    DOREPLIFETIME(AFootballer, DoneInitialSetup);
    
}

// Called when the game starts or when spawned
void AFootballer::BeginPlay()
{
	Super::BeginPlay();
    
    const FVector zero = FVector::ZeroVector;
    const FTransform transform = FTransform(zero);
//    AActor* Controller = GetWorld()->SpawnActor(AIControllerClass, &zero);
//    AIController = Cast<AFootballerAIController>(Controller);
//    AIController->Possess(this);
//    verify(AIController != nullptr);
//    AIController = GetWorld()->SpawnActor<AFootballerAIController>(GetActorLocation(), GetActorRotation());
	
    Ball = ABallsack::GetWorldBall(GetWorld());
    verify(Ball != nullptr);
    
    TargetingIndicator->SetVisibility(false);

    // GainPlayerControl is called before this BeginPlay() function in the GameMode::StartMatch() function.
    if (!ControlledByPlayer) {
        PlayerControlIndicator->SetVisibility(false);
    }
    
    GetCharacterMovement()->MaxWalkSpeed = FootballAttributes.SprintSpeed * 10;
}

// Called every frame
void AFootballer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
    
    if (!DoneInitialSetup && HasAuthority()) {
        // Things we need to do after BeginPlay() has been called on our blueprint subclass
        // If we aren't already being possessed by the player, we'll have our AI Controller possess us
        // TODO: Make this work again
//        if (GetController() == nullptr) {
//            AIController->Possess(this);
//        }
    }
    
    // Prevents spamming shots, passes and dribbles.
    // When we e.g. knock the ball on, we set JustKickedBall = true,
    // then when we are far enough away from the ball that we wouldn't trigger another touch
    // (i.e. !CanKickBall()), we set it back to false so we can touch it again.
    if (JustKickedBall && !CanKickBall()) JustKickedBall = false;

    if (ControlledByPlayer) {
        // Prevent spamming shots, passes and dribbles.
        // if (JustKickedBall) return;
        
        if (CanKickBall()) {
            GoingForPossession = true;
            ThinksHasPossession = true;
            
            if (PendingAction.Type != PendingActionType::FootballerActionNone) {
                // have a pending action: execute it
                switch (PendingAction.Type) {
					case PendingActionType::FootballerActionShot:
                        ShootBall(PendingAction.Power, PendingAction.Direction);
                        JustKickedBall = true;
                        break;
					case PendingActionType::FootballerActionPass:
                        PassBall(PendingAction.Power, PendingAction.Direction);
                        JustKickedBall = true;
                        break;
                }
            } else {//if (DesiredMovement.Size() > 0) {
                // otherwise knock the ball on
                KnockBallOn(DeltaTime, 5 + 5 * DesiredSprintStrength);
                JustKickedBall = true;
            }
            
            ClearPendingAction();
        } else {
            ThinksHasPossession = false;
        }
        
        if (GoingForPossession) {
            MoveToBallForKick(DesiredMovement, DeltaTime);
        } else if (WaitingForPass) {
            MoveToBallForPass(DesiredMovement, DeltaTime);
        } else {
            FreeMoveDesired();
        }
    } else {
        // AI tick.
        
    }
}

// Called to bind functionality to input
void AFootballer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AFootballer::ShowTargetingIndicator()
{
    TargetingIndicator->SetVisibility(true);
}

void AFootballer::HideTargetingIndicator()
{
    TargetingIndicator->SetVisibility(false);
}

void AFootballer::Server_GainPlayerControl_Implementation(AFootballerController* newController)
{
    if (AIController != nullptr) {
        AIController->UnPossess();
    }
    FootballerController = newController;
    ControlledByPlayer = true;
    
//    Client_GainPlayerControl(newController);
}

bool AFootballer::Server_GainPlayerControl_Validate(AFootballerController* newController) {
    return true;
}

//void AFootballer::Client_GainPlayerControl_Implementation(AFootballerController* newController)
//{
//    // Make sure the server set these fields correctly:
//    hope(ControlledByPlayer);
//    hope(FootballerController == newController);
//    
//    // and show our local arrow indicating which player is being controlled.
//    PlayerControlIndicator->SetVisibility(true);
//}

void AFootballer::Server_LosePlayerControl_Implementation()
{
    if (AIController != nullptr) {
        AIController->Possess(this);
    }
    FootballerController = nullptr;
    ControlledByPlayer = false;
    
//    Client_LosePlayerControl();
}

bool AFootballer::Server_LosePlayerControl_Validate() { return true; }

//void AFootballer::Client_LosePlayerControl_Implementation() {
//    hope(!ControlledByPlayer);
//    hope(FootballerController == nullptr);
//    
//    PlayerControlIndicator->SetVisibility(false);
//}

void AFootballer::OnRep_ControlledByPlayer() {
    PlayerControlIndicator->SetVisibility(ControlledByPlayer);
}

bool AFootballer::IsControlledByPlayer()
{
    return ControlledByPlayer;
}

// MARK: QUEUE_ACTIONS

void AFootballer::ClearPendingAction()
{
    PendingAction.Type = PendingActionType::FootballerActionNone;
    PendingAction.Power = 0;
    PendingAction.Direction = FVector::ZeroVector;
}

void AFootballer::SetPendingShot(float power, FVector desiredDirection)
{
    PendingAction.Type = PendingActionType::FootballerActionShot;
    PendingAction.Power = power;
    PendingAction.Direction = desiredDirection;
}

void AFootballer::SetPendingPass(float power, FVector desiredDirection)
{
    PendingAction.Type = PendingActionType::FootballerActionPass;
    PendingAction.Power = power;
    PendingAction.Direction = desiredDirection;
}

// MARK: CONTROL

FVector AFootballer::DesiredMovementOrForwardVector()
{
    if (DesiredMovement.Size() <= FLT_EPSILON)
    {
        return GetActorForwardVector();
    } else {
        return DesiredMovement;
    }
}

static float MIN_DISTANCE_FOR_TOUCH = 10;

bool AFootballer::CanKickBall()
{
    FVector distance = GetActorLocation() - Ball->GetActorLocation();
    distance.Z = 0;
    return distance.Size() < MIN_DISTANCE_FOR_TOUCH;
}

// MARK: DRIBBLING

void AFootballer::KnockBallOn_Implementation(float deltaSeconds, float strength) {
    FVector ballVelocity = Ball->GetVelocity();
    FVector direction = DesiredMovement;
    direction.Normalize();
    FVector desiredVelocity = direction * strength * 100;
    
    FVector neededVelChange = desiredVelocity - ballVelocity;
    
    Ball->GetStaticMeshComponent()->AddImpulse(neededVelChange, NAME_None, true);
//    Ball->GetStaticMeshComponent()->ComponentVelocity += neededVelChange;
    
    FString log = FString::Printf(TEXT("Ball velocity <%0.2f, %0.2f, %0.2f>; applying impulse of %0.2f, %0.2f, %0.2f"), ballVelocity.X, ballVelocity.Y, ballVelocity.Z, neededVelChange.X, neededVelChange.Y, neededVelChange.Z);
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, log);
}

bool AFootballer::KnockBallOn_Validate(float deltaSeconds, float strength) {
//    return CanKickBall();
    return true;
}

// MARK: SHOOTING

static float MAX_POWER = 1.00;
static float MIN_POWER = 0.5;
void AFootballer::ShootBall_Implementation(float power, FVector desiredDirection)
{
    if (!CanKickBall()) {
        SetPendingShot(power, desiredDirection);
        return;
    }
    
    float minDistance = FLT_MAX;
    AGoal* closerGoal = nullptr;
    TActorIterator<AGoal> itr(GetWorld());
    for (; itr; ++itr) {
        AGoal* goal = *itr;
        
        float distance = GetDistanceTo(goal);
        if (distance < minDistance) {
            minDistance = distance;
            closerGoal = goal;
        }
    }
    
    FVector leftPostDirection = closerGoal->LeftPost->GetComponentLocation() - Ball->GetActorLocation();
    leftPostDirection.Normalize();
    FVector rightPostDirection = closerGoal->RightPost->GetComponentLocation() - Ball->GetActorLocation();
    rightPostDirection.Normalize();
    
    FVector target;
    if (acosf(leftPostDirection | desiredDirection) < acosf(rightPostDirection | desiredDirection)) {
        // left post is closer to desired, so shoot just inside it
        target = closerGoal->LeftPost->GetComponentLocation();
        target.Y += 100;
        target.Z = 100;
    } else {
        target = closerGoal->RightPost->GetComponentLocation();
        target.Y -= 100; // just inside the post
        target.Z = 100;
    }
    
    desiredDirection = target - GetActorLocation();
    desiredDirection.Z = 0;
    desiredDirection.Normalize();
    
    power = FMath::Clamp(power, MIN_POWER, MAX_POWER);
    float xyFactor = 5000 * power;
    float zFactor = 700 * power;
    
    desiredDirection.X *= xyFactor;
    desiredDirection.Y *= xyFactor;
    desiredDirection.Z = zFactor;
    
    Ball->GetStaticMeshComponent()->AddImpulse(desiredDirection, NAME_None, true);
    
    ThinksHasPossession = false;
    GoingForPossession = true; // TODO: re-enable?
    JustKickedBall = true;
}

bool AFootballer::ShootBall_Validate(float a, FVector b) { return true; }

TArray<AFootballer*> AFootballer::Teammates()
{
    if (CachedTeammates.Num() != 0) {
        return CachedTeammates;
    }
    
    TArray<AFootballer*> array;
    if (Team == nullptr) return array;
    
    TActorIterator<AFootballer> actorIterator(GetWorld());
    for (; actorIterator; ++actorIterator) {
        AFootballer* footballer = *actorIterator;
        if (footballer != this && footballer->Team == Team) {
            array.Add(footballer);
        }
    }
    
    CachedTeammates = array;
    return array;
}

AFootballer* AFootballer::FindPassTarget(float power, FVector desiredDirection)
{
    float angleOfDesiredDirection = FMath::Atan2(desiredDirection.Y, desiredDirection.X);
    
    // Find pass target
    AFootballer* closestFootballer = nullptr;
    float closestDistance = FLT_MAX;
    
    TArray<AFootballer*> teammates = Teammates(); // separate declaration for debugging
    for (AFootballer* otherFootballer : teammates) {
        if (otherFootballer == nullptr) continue;
        
        FVector selfToOther = otherFootballer->GetActorLocation() - GetActorLocation();
        float selfToPlayerAngle = FMath::Atan2(selfToOther.Y, selfToOther.X);
        
        float angularDistance = FMath::Abs(angleOfDesiredDirection - selfToPlayerAngle);
        float linearDistance = selfToOther.Size();
        
        // the angle should be paramount, but also it should care about how far away the potential receiver is vs. how hard you want to hit the pass
        // power is between 0 and 1 atm. so distance * (1-power) penalizes long distance on low power, as we want
        float score = angularDistance * 800 + linearDistance * (1 - power);
        
        if (score < closestDistance && otherFootballer != this) {
            closestDistance = score;
            closestFootballer = otherFootballer;
        }
    }
    
    return closestFootballer;
}

void AFootballer::PassBall_Implementation(float power, FVector desiredDirection)
{
    if (!CanKickBall()) {
        SetPendingPass(power, desiredDirection);
        return;
    }
    
    power = FMath::Clamp(power, MIN_POWER, MAX_POWER);
    
    AFootballer* otherFootballer = FindPassTarget(power, desiredDirection);
    
    if (otherFootballer == nullptr) return;
    
    // Pass
    FVector passVector = otherFootballer->GetActorLocation() - Ball->GetActorLocation();
    passVector.Z = 0;
    const float size = passVector.Size();
    const float MIN_PASS_LENGTH = 500; // in cm => 5 meters
    const float MAX_PASS_LENGTH = 3000; // in cm => 30 meters
    float clampedSize = FMath::Clamp(size, MIN_PASS_LENGTH, MAX_PASS_LENGTH); // the minimum and maximum pass distance to consider, in cm, giving us a general approximation of not hitting the pass with 0 power and not hitting godly crossfield ground passes that no one could ever hit.
    passVector *= clampedSize / size;
    passVector *= 1.5; // general thing
//    passVector *= power;
    
    FVector ballVel = Ball->GetVelocity();
    FVector neededVelChange = passVector - ballVel;
    
    Ball->GetStaticMeshComponent()->AddImpulse(neededVelChange, NAME_None, true);
    
    ThinksHasPossession = false;
    GoingForPossession = false;
    JustKickedBall = true;
    
    if (FootballerController != nullptr) {
        // Let the controller know we released the ball
        FootballerController->SwitchToFootballer(otherFootballer);
    }
    
    otherFootballer->GoingForPossession = false; // important to do this after switchtofootballer
    otherFootballer->WaitingForPass = true;
}

bool AFootballer::PassBall_Validate(float power, FVector direction) { return true; }

// MARK: MOVEMENT

void AFootballer::MoveToBallForKick(FVector desiredEndDirection, float deltaSeconds)
{
    // Go around to the desired end direction unless we're really close to it
    
    FVector charPos = GetActorLocation();
    FVector targetPos = Ball->GetActorLocation();
    
    // shitty prediction of where the ball will be once we get to it -- a proper algorithm might need to use calculus
    FVector distanceVector = targetPos - charPos;
    distanceVector.Z = 0;
    float distance = distanceVector.Size();
    float approxTimeToTarget = distance / GetMovementComponent()->GetMaxSpeed(); // <- could use some legit calculus instead of just getting the velocity
    
    FVector predictedTargetPosition = targetPos + Ball->GetVelocity() * approxTimeToTarget;
    
//      PlayerControlIndicator->SetWorldTransform(FTransform(predictedTargetPosition));
    
    FVector desiredPos = predictedTargetPosition;
    desiredPos.Z = charPos.Z;
    
    FVector movementVector = desiredPos - charPos;
    if (movementVector.Size() > 1) {
        movementVector.Normalize();
    }
    
    AddMovementInput(movementVector);
}

void AFootballer::MoveToBallForPass(FVector desiredMovement, float deltaSeconds)
{
    float desiredMovementAngle = FMath::Atan2(desiredMovement.Y, desiredMovement.X);
    FVector distanceToBall = Ball->GetActorLocation() - GetActorLocation();
    distanceToBall.Z = 0;
    FVector ballMovementPrediction = Ball->GetVelocity() / 10;
    FVector predictedDistanceToBall = distanceToBall + ballMovementPrediction;
    float distanceAngle = FMath::Atan2(distanceToBall.Y, distanceToBall.X);
    
    if (desiredMovement.Size() > FLT_EPSILON && FMath::Abs(distanceAngle - desiredMovementAngle) < M_PI * 0.75) {
        // you are moving toward the ball, so move intensely and assisted toward the ball (sprint basically -- todo make this less sprinty)
        distanceToBall.Normalize();
        AddMovementInput(distanceToBall);
    } else {
        // move sluggishly toward the ball because you are trying to move away from it
        distanceToBall.Normalize();
        AddMovementInput(distanceToBall / 3);
    }
}

void AFootballer::FreeMoveDesired()
{
    FVector movementVector = DesiredMovement;
    movementVector *= (0.5 + 0.5 * DesiredSprintStrength);
    
    AddMovementInput(movementVector);
}

FVector calculateValidMovement(FVector movement, int sprintStrength) {
    movement.Normalize();
    hope(sprintStrength <= 1);
    return movement * (0.5 + 0.5 * sprintStrength);
}

void AFootballer::Server_SetDesiredMovement_Implementation(FVector movement)
{
    DesiredMovement = calculateValidMovement(movement, DesiredSprintStrength);
}

bool AFootballer::Server_SetDesiredMovement_Validate(FVector movement)
{
    return true;
}

void AFootballer::SetDesiredMovement(FVector movement)
{
    // Set it on the client side first
    DesiredMovement = calculateValidMovement(movement, DesiredSprintStrength);
    
    // Then request that the server do the same
    Server_SetDesiredMovement(movement);
}

void AFootballer::SetDesiredSprintStrength_Implementation(const float strength) {
    DesiredSprintStrength = simpleclamp(strength, 0, 1);
}

bool AFootballer::SetDesiredSprintStrength_Validate(float strength) { return true; }

void AFootballer::SetGoingForPossession_Implementation(bool going) {
    GoingForPossession = going;
}
bool AFootballer::SetGoingForPossession_Validate(bool going) { return true; }