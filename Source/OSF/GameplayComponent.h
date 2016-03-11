// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OSF_API UGameplayComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGameplayComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool CanKickBall(ACharacter *character, AActor *ball);
    
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void MoveToBallForKick(ACharacter *character, AActor *target, FVector desiredEndDirection, float deltaSeconds);
	
};
