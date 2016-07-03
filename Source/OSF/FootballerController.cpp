// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "FootballerController.h"
#include "TeamGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealNetwork.h"
#include "DefaultGameMode.h"

void AFootballerController::BeginPlay()
{
    Super::BeginPlay();
    
    Ball = ABallsack::GetWorldBall(GetWorld());
    
    if (HasAuthority()) {
        ADefaultGameMode* gameMode = GetWorld()->GetAuthGameMode<ADefaultGameMode>();
        check(gameMode != nullptr);
        UE_LOG(LogTemp, Warning, TEXT("IsSetup = %d"), gameMode->IsSetup);
        gameMode->PlaceController(this);
    }
}

void AFootballerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    if (!DoneInitialSetup && HasAuthority()) {
        // Things we need to do after BeginPlay() has been called everywhere in the world
//        ATeamGameState* state = GetWorld()->GetGameState<ATeamGameState>();
//        if (GetPawn() != nullptr) {
//            GetWorld()->DestroyActor(GetPawn());
//        }
//        SwitchToFootballer(state->HomeTeam->Footballers[10]);
        
        DoneInitialSetup = true;
    }
    
    if (ControlledFootballer != nullptr) {
//        UE_LOG(LogTemp, Warning, TEXT("Controlling %s"), *ControlledFootballer->DisplayName);
        float power = 0.5;
        if (PassStartTime != 0) {
            power = FMath::Clamp(UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - PassStartTime, 0.5f, 1.0f);
        }
        
        AFootballer* footballer = ControlledFootballer->FindPassTarget(power, ControlledFootballer->DesiredMovementOrForwardVector());
        if (footballer != nullptr && footballer != TargetedFootballer) {
            // Untarget old one
            if (TargetedFootballer != nullptr) {
                TargetedFootballer->HideTargetingIndicator();
            }
            
            // Target new one
            TargetedFootballer = footballer;
            TargetedFootballer->ShowTargetingIndicator();
        }
    }
}

void AFootballerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AFootballerController, ControlledFootballer);
}

// MARK: Input

void AFootballerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    InputComponent->BindAxis("MoveForward", this, &AFootballerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AFootballerController::MoveRight);
    InputComponent->BindAxis("Sprint", this, &AFootballerController::SprintAxisChanged);
    
    InputComponent->BindAction("Pass", IE_Pressed, this, &AFootballerController::PassPressed);
    InputComponent->BindAction("Pass", IE_Released, this, &AFootballerController::PassReleased);
    InputComponent->BindAction("Kick", IE_Pressed, this, &AFootballerController::KickPressed);
    InputComponent->BindAction("Kick", IE_Released, this, &AFootballerController::KickReleased);
    InputComponent->BindAction("Switch Player", IE_Released, this, &AFootballerController::SwitchPlayerReleased);
    InputComponent->BindAction("Free Movement", IE_Released, this, &AFootballerController::FreeMoveReleased);
}

void AFootballerController::MoveForward(float axisValue)
{
    if (ControlledFootballer != nullptr) {
        if (Role >= ROLE_AutonomousProxy) {
            ControlledFootballer->SetDesiredMovement(FVector(ControlledFootballer->DesiredMovement.X, InputComponent->GetAxisValue(TEXT("MoveForward")), 0));
        }
//        ControlledFootballer->DesiredMovement.Y = InputComponent->GetAxisValue(TEXT("MoveForward"));
//        ControlledFootballer->DesiredMovement.Normalize();
    }
    
//    if (ControlledFootballer != nullptr && InputComponent->GetAxisValue(TEXT("MoveForward")) > 0.1) {
//        UE_LOG(LogTemp, Warning, TEXT("Controlling %s"), *ControlledFootballer->DisplayName);
//    }
}

void AFootballerController::MoveRight(float axisValue)
{
    if (ControlledFootballer != nullptr) {
        if (Role >= ROLE_AutonomousProxy) {
            ControlledFootballer->SetDesiredMovement(FVector(InputComponent->GetAxisValue(TEXT("MoveRight")), ControlledFootballer->DesiredMovement.Y, 0));
        }
//        ControlledFootballer->DesiredMovement.X = InputComponent->GetAxisValue(TEXT("MoveRight"));
//        ControlledFootballer->DesiredMovement.Normalize();
    }
    
//    if (ControlledFootballer != nullptr && InputComponent->GetAxisValue(TEXT("MoveRight")) > 0.1) {
//        UE_LOG(LogTemp, Warning, TEXT("Controlling %s"), *ControlledFootballer->DisplayName);
//    }
}

void AFootballerController::SprintAxisChanged(float axisValue)
{
    if (ControlledFootballer != nullptr) {
        ControlledFootballer->SetDesiredSprintStrength(InputComponent->GetAxisValue(TEXT("Sprint")));
    }
}

void AFootballerController::PassPressed()
{
    PassStartTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
}

void AFootballerController::PassReleased()
{
    float power = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - PassStartTime;
    
    if (ControlledFootballer != nullptr) {
        ControlledFootballer->PassBall(power, ControlledFootballer->DesiredMovementOrForwardVector());
    }
    
    PassStartTime = 0;
}

void AFootballerController::KickPressed()
{
    KickStartTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
}

void AFootballerController::KickReleased()
{
    float power = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - KickStartTime;
    
    if (ControlledFootballer != nullptr) {
        ControlledFootballer->ShootBall(power, ControlledFootballer->DesiredMovementOrForwardVector());
    }
}

void AFootballerController::SwitchPlayerReleased()
{
    if (ControlledFootballer != nullptr && !ControlledFootballer->GoingForPossession) {
        // Only switch if you're not in possession right now
        Autoswitch();
    }
}

void AFootballerController::FreeMoveReleased()
{
    AFootballer* footballer = GetAutoswitchFootballer();
    if (ControlledFootballer != nullptr) {
        ControlledFootballer->SetGoingForPossession(false);
    }
}

void AFootballerController::Autoswitch_Implementation()
{
    AFootballer* footballer = GetAutoswitchFootballer();
    if (footballer != nullptr) {
        SwitchToFootballer(footballer);
    }
}
bool AFootballerController::Autoswitch_Validate() { return true; }

void AFootballerController::AutoswitchIfNeeded()
{
    // Get the most suitable footballer to switch to, including ourselves and other human-controlled footballers on our team
    AFootballer* footballer = GetAutoswitchFootballer(false);
    
    // And if it's not the one we're already controlling, or the one our human friend is already controlling, we switch to it
    if (footballer != nullptr && footballer != ControlledFootballer && !footballer->IsControlledByPlayer()) {
        SwitchToFootballer(footballer);
    }
}

void AFootballerController::SwitchToFootballer(AFootballer* footballer)
{
    // Don't consider footballers already controlled by a human on your team
    if (footballer == nullptr || footballer == ControlledFootballer || footballer->IsControlledByPlayer()) return;
    
    if (ControlledFootballer != nullptr) {
        ControlledFootballer->Server_LosePlayerControl();
        UnPossess();
    }
    
    ControlledFootballer = footballer;
    Possess(footballer);
    ControlledFootballer->SetGoingForPossession(true);
    ControlledFootballer->Server_GainPlayerControl(this);
	UE_LOG(LogTemp, Warning, TEXT("Controlling %s"), *ControlledFootballer->DisplayName);
}

AFootballer* AFootballerController::GetAutoswitchFootballer(bool rejectIfAlreadyControlled)
{
    float closestDistance = FLT_MAX;
    AFootballer* closestFootballer = nullptr;

    for (AFootballer *footballer : ControlledFootballer->Teammates()) {
        // Don't consider footballers controlled by a human on your team
        if (footballer == ControlledFootballer || footballer->IsControlledByPlayer()) continue;
        
        float distance = footballer->GetDistanceTo(Ball);
        if (distance < closestDistance) {
            closestDistance = distance;
            closestFootballer = footballer;
        }
    }
    
    return closestFootballer;
}
