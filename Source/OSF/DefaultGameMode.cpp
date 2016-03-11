// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "DefaultGameMode.h"
#include "TeamGameState.h"
#include "FootballerController.h"

static UClass* BPBallClass;

ADefaultGameMode::ADefaultGameMode()
{
    static ConstructorHelpers::FObjectFinder<UClass> BPBallFinder(TEXT("Blueprint'/Game/Blueprints/BPBall.BPBall_C'"));
    BPBallClass = BPBallFinder.Object;
}

void ADefaultGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    NextControllerIsHome = true;
    
    if (HasAuthority()) {
        // Uncomment this to have local 2-player
        // TODO: figure out how to count all the local controllers and call this function (N-1) times
//        UGameplayStatics::CreatePlayer(GetWorld());
    }
}

void ADefaultGameMode::StartPlay()
{
    // Looks like Super::StartPlay() calls StartMatch().
    // Not sure what the semantics are on what should be here and what should be in StartMatch(), so I have everything in StartMatch()
    Super::StartPlay();
}

void ADefaultGameMode::StartMatch()
{
    IsSetup = true;
    
    if (ABallsack::GetWorldBall(GetWorld()) == nullptr) return;
    
    if (HasAuthority()) {
        ATeamGameState* state = GetWorld()->GetGameState<ATeamGameState>();
        
        // Create the teams and the players in them. Also the ball
        state->HomeTeam = GetWorld()->SpawnActor<AFootballTeam>();
        state->AwayTeam = GetWorld()->SpawnActor<AFootballTeam>();
        
        // Spawn another player controller, temporary for now
        //GetWorld()->SpawnActor(PlayerControllerClass);
        
        state->Ball = ABallsack::GetWorldBall(GetWorld()); // (ABallsack *) GetWorld()->SpawnActor(BPBallClass);
        
        state->LoadSampleState();
        
        check(state != nullptr);
        check(state->HomeTeam != nullptr);
        check(state->AwayTeam != nullptr);
        
        check(state != nullptr);
        check(state->HomeTeam != nullptr);
        check(state->AwayTeam != nullptr);
        
        // Set up the player controllers properly for co-op / vs.
        for(FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            AFootballerController* controller = Cast<AFootballerController>(*Iterator);
            
            PlaceController(controller);
        }
    }
    
    Super::StartMatch();
}

void ADefaultGameMode::PlaceController(AFootballerController* controller) {
    if (!IsSetup) {
        // Not yet set up the teams
        // will place the controller using PlayerControllerIterator in StartMatch(), after we create the home and away team
        return;
    }
    
    check(HasAuthority());
    
    ATeamGameState* state = GetWorld()->GetGameState<ATeamGameState>();
    
    AFootballTeam *team;
    if (NextControllerIsHome) {
        UE_LOG(LogTemp, Warning, TEXT("Adding controller to home team"));
        team = state->HomeTeam;
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Adding controller to away team"));
        team = state->AwayTeam;
    }
    
    NextControllerIsHome = !NextControllerIsHome; // alternate controllers b/t home and away
    
    APawn* pawn = controller->GetPawn();
    if (pawn != nullptr) {
        controller->UnPossess();
        GetWorld()->DestroyActor(pawn);
    }
    
    // Switch to the first open footballer on the controller's team
    check(team != nullptr);
    check(team->Footballers.Num() > 0);
    for (int index = team->Footballers.Num() - 1; index >= 0; --index) {
        AFootballer* footballer = team->Footballers[index];
//    for (AFootballer* footballer : team->Footballers) {
        if (!footballer->IsControlledByPlayer()) {
            controller->SwitchToFootballer(footballer);
            break;
        }
    }
    check(controller->ControlledFootballer != nullptr);
}