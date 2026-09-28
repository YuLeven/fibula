#include "FibulaPlayerController.h"
#include "FibulaCharacter.h"
#include "TimerManager.h"
#include "FibulaGameMode.h"
#include "Framework/Application/NavigationConfig.h"
#include "FibulaGameState.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

AFibulaPlayerController::AFibulaPlayerController()
{
    bAutoManageActiveCameraTarget = true;
}

void AFibulaPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (IsLocalPlayerController())
    {
        TSharedRef<FNavigationConfig> Navigation = MakeShared<FNavigationConfig>();
        Navigation->bKeyNavigation = false;
        Navigation->bTabNavigation = false;
        Navigation->bAnalogNavigation = false;
        FSlateApplication::Get().SetNavigationConfig(Navigation);
    }
#if !UE_BUILD_SHIPPING
    if (IsLocalController() && FParse::Param(FCommandLine::Get(), TEXT("FibulaBotTestDriver")))
    {
        BotTestDriverStartTime = GetWorld()->GetTimeSeconds();
        GetWorldTimerManager().SetTimer(BotTestDriverTimerHandle, this, &AFibulaPlayerController::RunBotTestDriverStep, 0.05f, true, 2.0f);
        UE_LOG(LogTemp, Display, TEXT("FIBULA_BOT_TEST_CLIENT_READY"));
    }
#endif
}

void AFibulaPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if !UE_BUILD_SHIPPING
    GetWorldTimerManager().ClearTimer(BotTestDriverTimerHandle);
#endif
    Super::EndPlay(EndPlayReason);
}

#if !UE_BUILD_SHIPPING
void AFibulaPlayerController::RunBotTestDriverStep()
{
	if (GetWorld()->GetTimeSeconds() - BotTestDriverStartTime > 70.0f)
    {
        GetWorldTimerManager().ClearTimer(BotTestDriverTimerHandle);
        const bool bPassed = BotTestMoveInputs > 0 && BotTestTargetCommands > 0;
		const AFibulaCharacter *PlayerCharacter = Cast<AFibulaCharacter>(GetPawn());
		const UCharacterMovementComponent *Movement = PlayerCharacter ? PlayerCharacter->GetCharacterMovement() : nullptr;
		UE_LOG(LogTemp, Display,
			TEXT("FIBULA_BOT_TEST_CLIENT_RESULT %s MoveInputs=%d Movement=%d TargetCommands=%d ReplicatedDamage=%d location=%s velocity=%s role=%d mode=%d ignored=%d speed=%.0f"),
			bPassed ? TEXT("PASS") : TEXT("FAIL"),
			BotTestMoveInputs,
			BotTestObservedMovement,
			BotTestTargetCommands,
			BotTestReplicatedDamage,
			PlayerCharacter ? *PlayerCharacter->GetActorLocation().ToCompactString() : TEXT("none"),
			PlayerCharacter ? *PlayerCharacter->GetVelocity().ToCompactString() : TEXT("none"),
			PlayerCharacter ? static_cast<int32>(PlayerCharacter->GetLocalRole()) : -1,
			Movement ? static_cast<int32>(Movement->MovementMode) : -1,
			PlayerCharacter ? PlayerCharacter->IsMoveInputIgnored() : true,
			Movement ? Movement->MaxWalkSpeed : 0.0f);
        return;
    }

    AFibulaCharacter *PlayerCharacter = Cast<AFibulaCharacter>(GetPawn());
    if (!PlayerCharacter || !IsLocalController())
    {
        return;
    }

    if (BotTestLastHealth < 0)
    {
        BotTestLastHealth = PlayerCharacter->GetCurrentHealth();
        BotTestLastLocation = PlayerCharacter->GetActorLocation();
    }
    if (PlayerCharacter->GetCurrentHealth() < BotTestLastHealth)
    {
        ++BotTestReplicatedDamage;
    }
    BotTestLastHealth = PlayerCharacter->GetCurrentHealth();
    if (FVector::DistSquared(PlayerCharacter->GetActorLocation(), BotTestLastLocation) > FMath::Square(100.0f))
    {
        ++BotTestObservedMovement;
        BotTestLastLocation = PlayerCharacter->GetActorLocation();
    }

    AFibulaCharacter *NearestBot = nullptr;
    float NearestDistance = TNumericLimits<float>::Max();
    const AFibulaGameState *FibulaState = GetWorld()->GetGameState<AFibulaGameState>();
    const bool bTeamBattle = FibulaState && FibulaState->GetCurrentGameMode() == EGameModeType::TeamBattle;
    for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
    {
        AFibulaCharacter *Candidate = *It;
        if (!Candidate || !Candidate->IsBot() || Candidate->GetCurrentHealth() <= 0 || Candidate->IsInProtectionZone())
        {
            continue;
        }
        if (bTeamBattle && !PlayerCharacter->IsOpponentOf(Candidate))
        {
            continue;
        }
        const float Distance = FVector::Distance(PlayerCharacter->GetActorLocation(), Candidate->GetActorLocation());
        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            NearestBot = Candidate;
        }
    }

    if (!NearestBot)
    {
        return;
    }

    const FVector Direction = (NearestBot->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal2D();
    const float CombatDistance = FMath::Max(200.0f, PlayerCharacter->GetAttackRange() * 0.65f);
    const FVector MovementDirection = NearestDistance > CombatDistance
        ? Direction
        : FVector(-Direction.Y, Direction.X, 0.0f);
	PlayerCharacter->ServerAutomationBotTestMove(MovementDirection);
    SetControlRotation(Direction.Rotation());
    ++BotTestMoveInputs;

    if (BotTestTargetCommands == 0 || GetWorld()->GetTimeSeconds() >= BotTestNextTargetTime)
    {
        PlayerCharacter->ServerSetTarget(NearestBot);
        BotTestNextTargetTime = GetWorld()->GetTimeSeconds() + 2.0f;
        ++BotTestTargetCommands;
        if (PlayerCharacter->GetVocation() == EVocation::Knight)
        {
            PlayerCharacter->ServerCastSpell(TEXT("Berserk"));
        }
        UE_LOG(LogTemp, Verbose, TEXT("FIBULA_BOT_TEST_CLIENT_ACTION target=%s distance=%.0f"), *NearestBot->GetCharacterName(), NearestDistance);
    }
}
#endif

void AFibulaPlayerController::PawnLeavingGame()
{
    if (AFibulaCharacter *PlayerCharacter = Cast<AFibulaCharacter>(GetPawn()))
    {
        if (HasAuthority() && PlayerCharacter->IsInCombat())
        {
            
            PlayerCharacter->HandleCombatLogout();

            
            UnPossess();
        }
        else
        {
            Super::PawnLeavingGame();
        }
    }
    else
    {
        Super::PawnLeavingGame();
    }
}
