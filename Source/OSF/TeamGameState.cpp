// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"
#include "TeamGameState.h"
#include "UObjectGlobals.h"
#include "FootballerController.h"
#include "UnrealNetwork.h"

static UClass* footballerClass;
ATeamGameState::ATeamGameState()
{
    static ConstructorHelpers::FObjectFinder<UClass> MyBPClass(TEXT("Blueprint'/Game/Blueprints/BPFootballer.BPFootballer_C'"));
    footballerClass = MyBPClass.Object;
}

void ATeamGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ATeamGameState, HomeTeam);
    DOREPLIFETIME(ATeamGameState, AwayTeam);
    DOREPLIFETIME(ATeamGameState, Ball);
    
}

void ATeamGameState::BeginPlay()
{
    Super::BeginPlay();
    
    if (HasAuthority()) {
        bool poop = true;
    }
    
//    if (HasAuthority()) {
//        HomeTeam = GetWorld()->SpawnActor<AFootballTeam>();
//        AwayTeam = GetWorld()->SpawnActor<AFootballTeam>();
//        
//        LoadSampleState();
//    }
    
//    if (HasAuthority()) {
//        ATeamGameState* state = GetWorld()->GetGameState<ATeamGameState>();
//        
//        check(state != nullptr);
//        check(state->HomeTeam != nullptr);
//        check(state->AwayTeam != nullptr);
//        
//        // Set up the player controllers properly for co-op / vs.
//        bool home = true;
//        for(FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
//        {
//            AFootballerController* controller = Cast<AFootballerController>(*Iterator);
//            AFootballTeam *team;
//            if (home) {
//                team = state->HomeTeam;
//            } else {
//                team = state->AwayTeam;
//            }
//            
//            home = !home; // alternate controllers b/t home and away
//            
//            // UE4 spawns a new footballer for each player controller by default--let's delete it and let our TeamGameState handle that.
//            if (controller->GetPawn() != nullptr) {
//                GetWorld()->DestroyActor(controller->GetPawn());
//            }
//            
//            // Switch to the first open footballer on the controller's team
//            check(team != nullptr);
//            check(team->Footballers.Num() > 0);
//            for (AFootballer* footballer : team->Footballers) {
//                if (!footballer->IsControlledByPlayer()) {
//                    controller->SwitchToFootballer(footballer);
//                    break;
//                }
//            }
//            check(controller->ControlledFootballer != nullptr);
//            
//        }
//    }
}

struct FootballerInfo {
    FString Name;
};

struct Position {
    int IndexInFormation;
    FString Name;
    FVector2D Location;
    // X = 0 is the goal line, X = 1 is midfield (striker)
    // Y = -1 is the left flank, Y = 1 is the right flank
};
	
struct Formation {
    FString Name;
    Position Positions[11];
};

static struct Formation Formation4231 = {
    "4-2-3-1",
    {
        { 0, "GK", { 0.0, 0.5 } },
        
        { 1, "RB", { 0.3, 1 } },
        { 2, "RCB", { 0.2, 0.75 } },
        { 3, "LCB", { 0.2, 0.25 } },
        { 4, "LB", { 0.3, 0 } },
        
        { 5, "RDM", { 0.45, 0.75 } },
        { 6, "LDM", { 0.55, 0.25 } },
        
        { 7, "RAM", { 0.8, 1 } },
        { 8, "CAM", { 0.7, 0.6 } },
        { 9, "LAM", { 0.8, 0 } },
        
        { 10, "CF", { 1, 0.4 } },
    }
};

static struct Formation FormationDebug = {
    "D-E-B-U-G",
    {
        { 0, "GK", { 0.1, 0 } },
        
        { 1, "RB", { 0.3, 0.8 } },
        { 2, "RCB", { 0.3, -0.5 } },
        { 3, "LCB", { 0.3, -0.5 } },
        { 4, "LB", { 0.3, -0.8 } },
        
        { 5, "RDM", { 0.5, 0.5 } },
        { 6, "LDM", { 0.5, -0.5 } },
        
        { 7, "RAM", { 0.8, 0.8 } },
        { 8, "CAM", { 0.8, 0 } },
        { 9, "LAM", { 0.8, -0.8 } },
        
        { 10, "CF", { 1, 0.4 } },
    }
};

void ATeamGameState::LoadSampleState()
{
    static int NUM_PLAYERS = 11;
    
    static struct FootballerInfo homePlayers[11] = {
        { "Szczczczczczcz" },
        
        { "Douche" },
        { "BFG" },
        { "Gabby" },
        { "Nachos" },
        
        { "Cock" },
        { "Santa Gorgonzola" },
        
        { "Sign Da Ting" },
        { "Tattoo Haram" },
        { "Wiz Khalifa - No Sleep" },
        
        { "Goat Cheese" }
    };
    
    static struct FootballerInfo awayPlayers[11] = {
        { "Newer" },
        
        { "Lame" },
        { "Boatingle" },
        { "Artines" },
        { "God" },
        
        { "Shabby" },
        { "Vidal Sassoon" },
        
        { "Robbery I" },
        { "Giraffe" },
        { "Robbery II" },
        
        { "Lrzwrzszcszczewski" }
    };
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        struct FootballerInfo homePlayer = homePlayers[i];
        struct FootballerInfo awayPlayer = awayPlayers[i];
        
        HomeTeam->Footballers.Add(PlayerFromProperties(HomeTeam, i, homePlayer.Name));
        AwayTeam->Footballers.Add(PlayerFromProperties(AwayTeam, i, awayPlayer.Name));
    }
}

static float HalfFieldWidth = 2750;
static float HalfFieldLength = 4755;
AFootballer* ATeamGameState::PlayerFromProperties(AFootballTeam* team, int index, FString name)
{
    struct Position pos = FormationDebug.Positions[index];
    
    FVector location;
    
    // Position.Location.x=0 should be the goalie
    // so for away team, that's highest positive worldX
    // for home team, that's highest negative worldX
    float correctLocation = 1 - pos.Location.X;
    if (team == HomeTeam) correctLocation *= -1;
    
    location.X = correctLocation * HalfFieldLength * 0.9;
    location.Y = pos.Location.Y * HalfFieldWidth * 0.9;
    location.Z = 100;
    
    FTransform transform;
    transform.SetLocation(location);

//    GetWorld()->GetAuthGameMode()->DefaultPawnClass;
    AActor* actor = GetWorld()->SpawnActor(footballerClass, &transform);
    AFootballer* footballer = Cast<AFootballer>(actor);
    if (footballer) {
        footballer->DisplayName = name;
        footballer->Team = team;
		footballer->SetActorLocation(location);
    }
    
    return footballer;
}