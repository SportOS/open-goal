// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Footballer.h"
#include "GameFramework/PlayerController.h"
#include "FootballerController.generated.h"

/**
 * 
 */
UCLASS()
class OSF_API AFootballerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    virtual void Tick(float DeltaSeconds) override;
    
    virtual void SetupInputComponent() override;
    
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category="Assistance/Switching")
    void SwitchToFootballer(AFootballer* footballer);

    UFUNCTION(BlueprintCallable, Category="Assistance/Switching")
    AFootballer* GetAutoswitchFootballer(bool rejectIfAlreadyControlled = true);
    
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category="Assistance/Switching")
    void Autoswitch();
    
    UFUNCTION(BlueprintCallable, Category="Assistance/Switching")
    void AutoswitchIfNeeded(); // TODO make this a server function if we use it in the future
    
    
    void MoveForward(float axisValue);
    void MoveRight(float axisValue);
    void SprintAxisChanged(float axisValue);
    void PassPressed();
    void PassReleased();
    void KickPressed();
    void KickReleased();
    void SwitchPlayerReleased();
    void FreeMoveReleased();
    
    UPROPERTY(Replicated)
    AFootballer* ControlledFootballer;
    AFootballer* TargetedFootballer;
    ABallsack* Ball;
    
private:
    float PassStartTime, KickStartTime;
    bool SpawnedTeammates;
    bool DoneInitialSetup;
};
