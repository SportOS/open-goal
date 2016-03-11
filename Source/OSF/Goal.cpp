// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "Goal.h"


// Sets default values
AGoal::AGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    
    this->LeftPost = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftPost"));
    this->LeftPost->AttachTo(RootComponent);
    
    this->RightPost = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPost"));
    this->RightPost->AttachTo(RootComponent);
    
    this->Crossbar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crossbar"));
    this->Crossbar->AttachTo(RootComponent);
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("StaticMesh'/Game/Shapes/Shape_Cylinder'"));
    if (SphereVisualAsset.Succeeded())
    {
        this->LeftPost->SetStaticMesh(SphereVisualAsset.Object);
        this->LeftPost->SetRelativeLocation(FVector(0.0f, -465.0f, 0.0f));
        this->LeftPost->SetRelativeScale3D(FVector(0.2f, 0.2f, 4.0f));
        
        this->RightPost->SetStaticMesh(SphereVisualAsset.Object);
        this->RightPost->SetRelativeLocation(FVector(0.0f, 465.0f, 0.0f));
        this->RightPost->SetRelativeScale3D(FVector(0.2f, 0.2f, 4.0f));
        
        this->Crossbar->SetStaticMesh(SphereVisualAsset.Object);
        this->Crossbar->SetRelativeLocation(FVector(0.0f, -470.0f, 390.0f));
        this->Crossbar->SetRelativeScale3D(FVector(0.2f, 0.2f, 9.4f));
        this->Crossbar->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
    }
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoal::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AGoal::IsLocationInGoal(FVector BallLocation)
{
    // Ball vector: X=4816.097 Y=-260.000 Z=203.153, Left vector: X=-4755.000 Y=-465.000 Z=50.000, Right vector: X=-4755.000 Y=465.000 Z=50.000
    // Ball vector: X=4816.097 Y=-260.000 Z=203.153, Left vector: X=4755.000 Y=-465.000 Z=50.000, Right vector: X=4755.000 Y=465.000 Z=50.000
    FVector LeftPostVector = this->LeftPost->GetComponentLocation();
    FVector RightPostVector = this->RightPost->GetComponentLocation();
    FVector CrossbarVector = this->Crossbar->GetComponentLocation();
    UE_LOG(LogTemp, Warning, TEXT("Ball Vector: %s, Left Vector: %s, Right Vector: %s, Crossbar Vector: %s"), *BallLocation.ToString(), *LeftPostVector.ToString(), *RightPostVector.ToString(), *CrossbarVector.ToString());
    
    if (BallLocation.Z > 0 && BallLocation.Z < CrossbarVector.Z) {
        if (LeftPostVector.X < 0) {
            if (BallLocation.X < LeftPostVector.X) {
                if (LeftPostVector.Y < 0) {
                    return BallLocation.Y > LeftPostVector.Y && BallLocation.Y < RightPostVector.Y;
                } else {
                    return BallLocation.Y < LeftPostVector.Y && BallLocation.Y > RightPostVector.Y;
                }
            }
        } else {
            // ball is out of bounds AKA behind the goal
            if (BallLocation.X > LeftPostVector.X) {
                if (LeftPostVector.Y < 0) {
                    return BallLocation.Y > LeftPostVector.Y && BallLocation.Y < RightPostVector.Y;
                } else {
                    return BallLocation.Y < LeftPostVector.Y && BallLocation.Y > RightPostVector.Y;
                }
            }
        }
    }
    return false;
}

