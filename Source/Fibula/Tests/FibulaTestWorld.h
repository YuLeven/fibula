#pragma once

#if WITH_DEV_AUTOMATION_TESTS

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "FibulaCharacter.h"
#include "FibulaGameMode.h"
#include "FibulaGameState.h"

// Real production actors in an isolated authority world. Simple rule tests skip
// BeginPlay; interaction fixtures explicitly start the real mode/world lifecycle.
namespace FibulaTests
{
class FTestWorld
{
public:
    explicit FTestWorld(UClass* ModeClass = nullptr)
    {
        UWorld::InitializationValues Values;
        Values.AllowAudioPlayback(false).CreatePhysicsScene(true)
            .RequiresHitProxies(false).CreateNavigation(false).CreateAISystem(false)
            .ShouldSimulatePhysics(false).SetTransactional(false);
        World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
            true, ERHIFeatureLevel::SM5, &Values);
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
        World->SetGameInstance(NewObject<UGameInstance>(GEngine));

        // Database initialization uses ConstructorHelpers and must happen in a
        // UObject constructor, never by calling Initialize from a test function.
        GetDefault<AFibulaGameState>();
        if (ModeClass)
        {
            FURL URL;
            URL.AddOption(*FString::Printf(TEXT("game=%s"), *ModeClass->GetPathName()));
            check(World->SetGameMode(URL));
            World->InitializeActorsForPlay(URL);
            World->BeginPlay();
        }
    }

    ~FTestWorld()
    {
        if (World->HasBegunPlay())
        {
            World->EndPlay(EEndPlayReason::Quit);
        }
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    }

    template <typename T> T* Spawn(const FVector& Location = FVector::ZeroVector)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        return World->SpawnActor<T>(Location, FRotator::ZeroRotator, Params);
    }

    AFibulaCharacter* Character(EVocation Vocation = EVocation::Knight, int32 Team = 1)
    {
        auto* Result = Spawn<AFibulaCharacter>(FVector(Characters++ * 500.0f, 0, 100));
        Result->SetVocation(Vocation);
        Result->SetTeamId(Team);
        Result->InitializeCharacterStats();
        Result->SetInProtectionZone(false);
        return Result;
    }

    AFibulaCharacter* JoinPlayer(const FString& PlayerName, const FString& Vocation)
    {
        AFibulaGameMode* Mode = World->GetAuthGameMode<AFibulaGameMode>();
        if (!Mode) return nullptr;

        auto* Result = Spawn<AFibulaCharacter>();
        auto* Controller = Spawn<APlayerController>();
        Controller->Possess(Result);
        Mode->InitializePlayerCharacter(Controller, PlayerName, Vocation);
        Result->SetInProtectionZone(false);
        return Result;
    }

    UWorld* World = nullptr;

private:
    int32 Characters = 0;
};
}
#endif
