// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Ballsack.h"
#include "OSF.h"

class AFootballerController; // forward-declaration hell
class AFootballerAIController; // :>
class AFootballTeam;

#include "Footballer.generated.h"

UCLASS()
class OSF_API AFootballer : public ACharacter
{
	GENERATED_BODY()

public:
    // Useful typedefs
    typedef struct _FootballAttributeInfo {
        int SprintSpeed;
    } FootballAttributeInfo;
    
    typedef enum class _PendingActionType {
        FootballerActionNone,
        FootballerActionShot,
        FootballerActionPass,
    } PendingActionType;
    
    typedef struct _PendingActionInfo {
        PendingActionType Type;
        float Power;
        FVector Direction;
    } PendingActionInfo;
    
	AFootballer();

	// MARK: - Override Functions
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
    
    // MARK: - Authority-set (replicated) properties
    // All organized in one place to make writing GetLifetimeReplicatedProps easier.
    // "Only the changes made to replicated properties in the server will be replicated to clients. If a client changes the value of a replicated variable locally then it will stay that way until the next time the server changes it (after which it will be replicated and overwritten on the client). In this sense you should consider a UPROPERTY that is tagged with Replicated to be owned/controlled by the server."
    
    UPROPERTY(BlueprintReadWrite, Category=Custom, Replicated)
    FVector DesiredMovement;
    
    UPROPERTY(Replicated)
    float DesiredSprintStrength;
    
    UPROPERTY(BlueprintReadWrite, Category=Custom, Replicated)
    ABallsack* Ball;
    
    /**
     Tracks whether the player is trying to move toward the ball, to shoot/pass/get control if he thinks the ball is meant for him or he should challenge for it.
     */
    UPROPERTY(Replicated)
    bool GoingForPossession;
    
    /**
     Tracks if the player thinks they are currently dribbling the ball. (Stays true between touches on the ball, but is false as soon as you pass or shoot.)
     TODO: evaluate if this is needed
     */
    UPROPERTY(Replicated)
    bool ThinksHasPossession;
    
    /**
     GoingForPossession doesn't quite do it when we're awaiting a pass. Cause we're not really going for it. We're waiting for it.
     */
    UPROPERTY(Replicated)
    bool WaitingForPass;
    
    /**
     Prevents spamming shots, passes and dribbles.
     When we e.g. knock the ball on, we set JustKickedBall = true,
     then when we are far enough away from the ball that we wouldn't trigger another touch
     (i.e. !CanKickBall()), we set it back to false so we can touch it again.
     */
    UPROPERTY(Replicated)
    bool JustKickedBall;
    
    /**
     IDEA:
     stores the last kick time. we limit you to one kick per N ms.
     N goes down as your dribbling stat goes up.
     This allows us to differentiate players in close control dribbling later.
     */
    UPROPERTY(Replicated)
    float LastKickTime;
    
    /**
     very nullable.
     */
    UPROPERTY(Replicated)
    AFootballerController* FootballerController;
    
    //UPROPERTY(Replicated)
    FootballAttributeInfo FootballAttributes;
    
    UPROPERTY(Replicated)
    FString DisplayName;
    
    // nullable, but very shortly should not be.
    UPROPERTY(BlueprintReadWrite, Category=Custom, Replicated)
    AFootballTeam* Team;
    
    /**
     created on each footballer. An AI controller we can hot-swap to, when needed.
     */
    UPROPERTY(BlueprintReadWrite, Category=Custom, Replicated)
    AFootballerAIController* AIController;
    
    //UPROPERTY(Replicated)
    PendingActionInfo PendingAction;
    
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_ControlledByPlayer)
    bool ControlledByPlayer;
    
    // Nullable
    UPROPERTY(Replicated)
    TArray<AFootballer*> CachedTeammates;
    
    UPROPERTY(Replicated)
    bool DoneInitialSetup;
    
    
    // MARK: - Uncategorized
    
    UFUNCTION()
    void SetDesiredMovement(FVector movement);
    
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetDesiredMovement(FVector movement);
    virtual void Server_SetDesiredMovement_Implementation(FVector movement);
    virtual bool Server_SetDesiredMovement_Validate(FVector movement);
    
    UFUNCTION(Server, Reliable, WithValidation)
    void SetDesiredSprintStrength(float strength);
    virtual void SetDesiredSprintStrength_Implementation(float strength);
    virtual bool SetDesiredSprintStrength_Validate(float strength);
    
    UFUNCTION(BlueprintCallable, Category=Custom, Server, Reliable, WithValidation)
    void Server_GainPlayerControl(AFootballerController* newController);
    
//    UFUNCTION(BlueprintCallable, Category=Custom, Client, Reliable)
//    void Client_GainPlayerControl(AFootballerController* newController);
    
    UFUNCTION(BlueprintCallable, Category=Custom, Server, Reliable, WithValidation)
    void Server_LosePlayerControl();
    
//    UFUNCTION(BlueprintCallable, Category=Custom, Client, Reliable)
//    void Client_LosePlayerControl();

    UFUNCTION(BlueprintCallable, Category=Custom)
    bool IsControlledByPlayer();
    
    UPROPERTY(BlueprintReadWrite, Category=Custom)
    UStaticMeshComponent* PlayerControlIndicator;
    
    void ShowTargetingIndicator();
    void HideTargetingIndicator();
    
    UPROPERTY(BlueprintReadWrite, Category=Custom)
    UStaticMeshComponent* TargetingIndicator;
    
    UFUNCTION(BlueprintCallable, Category="Queue Actions")
    void ClearPendingAction();
    
    UFUNCTION(BlueprintCallable, Category="Queue Actions")
    void SetPendingShot(float power, FVector desiredDirection);
    
    UFUNCTION(BlueprintCallable, Category="Queue Actions")
    void SetPendingPass(float power, FVector desiredDirection);
    
    UFUNCTION(BlueprintCallable, Category="Control")
    FVector DesiredMovementOrForwardVector();
    
    UFUNCTION(BlueprintCallable, Category="Control")
    bool CanKickBall();
    
    UFUNCTION(BlueprintCallable, Category=Custom, Server, Reliable, WithValidation)
    void KnockBallOn(float deltaSeconds, float strength);
    
    UFUNCTION(BlueprintCallable, Category=Custom, Server, Reliable, WithValidation)
    void ShootBall(float power, FVector desiredDirection);
    
    AFootballer* FindPassTarget(float power, FVector desiredDirection);
    
    UFUNCTION(Server, BlueprintCallable, Category=Custom, Reliable, WithValidation)
    void PassBall(float power, FVector desiredDirection);
    
    UFUNCTION(BlueprintCallable, Category="Movement")
    void MoveToBallForKick(FVector desiredEndDirection, float deltaSeconds);
    
    void MoveToBallForPass(FVector desiredMovement, float deltaSeconds);
    
    // Moves based on desired movement without trying to move toward the ball.
    UFUNCTION(BlueprintCallable, Category="Movement")
    void FreeMoveDesired();
    
    UFUNCTION(Server, Reliable, WithValidation)
    void SetGoingForPossession(bool going);
    
    UFUNCTION(BlueprintCallable, Category=Custom)
    TArray<AFootballer*> Teammates();
    
private:
    UFUNCTION()
    void OnRep_ControlledByPlayer();
};
