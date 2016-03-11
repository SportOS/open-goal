// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Goal.generated.h"

UCLASS()
class OSF_API AGoal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoal();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
    
    void VibrateController(AActor* selfActor, AActor* otherActor, FVector normalImpulse, const FHitResult& hit);
    
    UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent *LeftPost;
    
    UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent *RightPost;
    
    UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent *Crossbar;

    UFUNCTION(BlueprintCallable, Category = Custom)
    bool IsLocationInGoal(FVector ballLocation);
	
};
