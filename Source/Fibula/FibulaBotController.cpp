#include "FibulaBotController.h"

#include "FibulaCharacter.h"
#include "FibulaGameMode.h"
#include "FibulaGameState.h"
#include "SpellDatabase.h"
#include "SpellSystem.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

AFibulaBotController::AFibulaBotController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bWantsPlayerState = false;
}

void AFibulaBotController::InitializeBot(int32 Seed)
{
	RandomStream.Initialize(Seed);
}

void AFibulaBotController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority() || !bDirectSteering || (!bHasRoamingDestination && !CurrentTarget.IsValid()))
	{
		return;
	}

	AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot)
	{
		return;
	}

	const FVector Destination = CurrentTarget.IsValid()
		? CurrentTarget->GetActorLocation()
		: RoamingDestination;
	Bot->AddMovementInput((Destination - Bot->GetActorLocation()).GetSafeNormal2D(), 1.0f);
}

void AFibulaBotController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);

	if (!HasAuthority())
	{
		return;
	}

	SetActorTickEnabled(false);
	LastProgressLocation = InPawn ? InPawn->GetActorLocation() : FVector::ZeroVector;
	LastObservedLocation = LastProgressLocation;
	LastProgressTime = GetWorld()->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(
		ThinkTimerHandle,
		this,
		&AFibulaBotController::Think,
		RandomStream.FRandRange(THINK_INTERVAL_MIN, THINK_INTERVAL_MAX),
		true);
}

void AFibulaBotController::OnUnPossess()
{
	SetActorTickEnabled(false);
	GetWorldTimerManager().ClearTimer(ThinkTimerHandle);
	Super::OnUnPossess();
}

void AFibulaBotController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetActorTickEnabled(false);
	GetWorldTimerManager().ClearTimer(ThinkTimerHandle);
	Super::EndPlay(EndPlayReason);
}

AFibulaCharacter *AFibulaBotController::GetBotCharacter() const
{
	return Cast<AFibulaCharacter>(GetPawn());
}

EVocation AFibulaBotController::ChooseRandomVocation(FRandomStream &RandomStream)
{
	static const EVocation VocationChoices[] = {
		EVocation::Knight,
		EVocation::Paladin,
		EVocation::Sorcerer,
		EVocation::Druid};

	return VocationChoices[RandomStream.RandRange(0, UE_ARRAY_COUNT(VocationChoices) - 1)];
}

bool AFibulaBotController::IsValidEnemy(AFibulaCharacter *Candidate) const
{
	AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot || Bot->IsInProtectionZone() || !Candidate || Candidate == Bot || !IsValid(Candidate) ||
		Candidate->GetCurrentHealth() <= 0 || Candidate->IsInProtectionZone())
	{
		return false;
	}

	AFibulaGameMode *GameMode = Cast<AFibulaGameMode>(GetWorld()->GetAuthGameMode());
	return GameMode && GameMode->CanDamage(Bot, Candidate);
}

AFibulaCharacter *AFibulaBotController::FindBestTarget() const
{
	const AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot)
	{
		return nullptr;
	}

	AFibulaCharacter *BestTarget = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
	{
		AFibulaCharacter *Candidate = *It;
		if (!IsValidEnemy(Candidate) ||
			(Candidate == RecentlyStalledTarget.Get() && GetWorld()->GetTimeSeconds() < IgnoreStalledTargetUntil))
		{
			continue;
		}

		const float Distance = FVector::Distance(Bot->GetActorLocation(), Candidate->GetActorLocation());
		if (Distance > TARGET_SEARCH_RADIUS)
		{
			continue;
		}

		// Always favor a human opponent inside the search radius.
		const float HumanPreference = Candidate->IsBot() ? TARGET_SEARCH_RADIUS + 1.0f : 0.0f;
		const float Score = Distance + HumanPreference;
		if (Score < BestScore)
		{
			BestScore = Score;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}

AFibulaCharacter *AFibulaBotController::FindBestAllyToHeal() const
{
	AFibulaCharacter *Bot = GetBotCharacter();
	AFibulaGameMode *GameMode = Cast<AFibulaGameMode>(GetWorld()->GetAuthGameMode());
	if (!Bot || !GameMode || GameMode->GetCurrentGameMode() != EGameModeType::TeamBattle)
	{
		return nullptr;
	}

	AFibulaCharacter *BestAlly = nullptr;
	float LowestHealthRatio = 0.75f;
	for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
	{
		AFibulaCharacter *Candidate = *It;
		if (!Candidate || Candidate == Bot || Candidate->GetCurrentHealth() <= 0 ||
			Candidate->IsInProtectionZone() ||
			!GameMode->CanHeal(Bot, Candidate))
		{
			continue;
		}

		const float HealthRatio = Candidate->GetMaxHealth() > 0
			? static_cast<float>(Candidate->GetCurrentHealth()) / Candidate->GetMaxHealth()
			: 1.0f;
		if (HealthRatio < LowestHealthRatio)
		{
			LowestHealthRatio = HealthRatio;
			BestAlly = Candidate;
		}
	}

	return BestAlly;
}

void AFibulaBotController::Think()
{
	AFibulaCharacter *Bot = GetBotCharacter();
	if (!HasAuthority() || !Bot || Bot->GetCurrentHealth() <= 0)
	{
		return;
	}

	ObserveMovementProgress();
	ObserveTargetHealth();

	AFibulaCharacter *BestTarget = FindBestTarget();
	if (CurrentTarget.IsValid() && IsValidEnemy(CurrentTarget.Get()))
	{
		const float CurrentDistance = FVector::Distance(Bot->GetActorLocation(), CurrentTarget->GetActorLocation());
		const float BestDistance = BestTarget
			? FVector::Distance(Bot->GetActorLocation(), BestTarget->GetActorLocation())
			: TNumericLimits<float>::Max();

		// Avoid switching targets for small distance differences; this makes fights readable.
		if (!BestTarget || CurrentTarget == BestTarget || BestDistance + 650.0f >= CurrentDistance)
		{
			BestTarget = CurrentTarget.Get();
		}
	}

	if (BestTarget != CurrentTarget.Get())
	{
		CurrentTarget = BestTarget;
		if (BestTarget)
		{
			++TargetAcquisitionCount;
			UE_LOG(LogTemp, Verbose, TEXT("Bot %s acquired %s"), *Bot->GetCharacterName(), *BestTarget->GetCharacterName());
			Bot->ServerSetTarget(BestTarget);
		}
		else
		{
			Bot->ServerSetTarget(nullptr);
		}
	}

	if (TryHeal())
	{
		return;
	}

	if (AFibulaCharacter *Target = CurrentTarget.Get())
	{
		const FVector ToTarget = Target->GetActorLocation() - Bot->GetActorLocation();
		const float Distance = ToTarget.Size();
		const float DesiredRange = FMath::Max(180.0f, Bot->GetAttackRange() * 0.8f);

		SetFocus(Target);
		if (Distance > DesiredRange)
		{
			MoveToward(Target->GetActorLocation());
		}
		else
		{
			StopMovement();
			bDirectSteering = false;
			SetActorTickEnabled(false);
			bHasRoamingDestination = false;
			if (Bot->GetVocation() == EVocation::Knight || Bot->GetVocation() == EVocation::Paladin)
			{
				// ServerSetTarget starts the existing authoritative auto-attack loop.
				if (!Bot->GetCurrentTarget())
				{
					Bot->ServerSetTarget(Target);
				}
				++CombatOrderCount;
			}
			if (TryCastOffensiveSpell())
			{
				++CombatOrderCount;
			}
		}
		return;
	}

	ClearFocus(EAIFocusPriority::Gameplay);
	if (Bot->IsInProtectionZone() || !bHasRoamingDestination ||
		FVector::DistSquared(Bot->GetActorLocation(), RoamingDestination) <= FMath::Square(ACCEPTANCE_RADIUS))
	{
		ChooseRoamingDestination();
	}

	if (bHasRoamingDestination)
	{
		MoveToward(RoamingDestination);
	}
}

void AFibulaBotController::ObserveMovementProgress()
{
	AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot)
	{
		return;
	}

	const FVector CurrentLocation = Bot->GetActorLocation();
	if (FVector::DistSquared(CurrentLocation, LastObservedLocation) > FMath::Square(STUCK_DISTANCE))
	{
		++MovementProgressCount;
		LastObservedLocation = CurrentLocation;
	}
}

void AFibulaBotController::ObserveTargetHealth()
{
	AFibulaCharacter *Target = CurrentTarget.Get();
	if (!Target || !IsValid(Target))
	{
		LastObservedTarget.Reset();
		LastObservedHealth = 0;
		return;
	}

	if (LastObservedTarget == Target && Target->GetCurrentHealth() < LastObservedHealth)
	{
		++DamageObservedCount;
	}
	LastObservedTarget = Target;
	LastObservedHealth = Target->GetCurrentHealth();
}

void AFibulaBotController::MoveToward(const FVector &Destination)
{
	AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot)
	{
		return;
	}

	const FVector CurrentLocation = Bot->GetActorLocation();
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (FVector::DistSquared(CurrentLocation, LastProgressLocation) > FMath::Square(STUCK_DISTANCE))
	{
		LastProgressLocation = CurrentLocation;
		LastProgressTime = CurrentTime;
		bDirectSteering = false;
		SetActorTickEnabled(false);
	}
	else if (CurrentTime - LastProgressTime > STUCK_TIMEOUT)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Bot %s stalled; dropping its route and replanning"), *Bot->GetCharacterName());
		StopMovement();
		if (CurrentTarget.IsValid())
		{
			RecentlyStalledTarget = CurrentTarget;
			IgnoreStalledTargetUntil = CurrentTime + STUCK_TIMEOUT;
			CurrentTarget.Reset();
			Bot->ServerSetTarget(nullptr);
		}
		bHasRoamingDestination = false;
		bDirectSteering = false;
		LastProgressLocation = CurrentLocation;
		LastProgressTime = CurrentTime;
		ChooseRoamingDestination();
		return;
	}

	if (bDirectSteering)
	{
		++MovementOrderCount;
		return;
	}

	UNavigationSystemV1 *NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation ReachableDestination;
	if (NavigationSystem && NavigationSystem->ProjectPointToNavigation(Destination, ReachableDestination, FVector(500.0f, 500.0f, 500.0f)))
	{
		bDirectSteering = false;
		SetActorTickEnabled(false);
		const EPathFollowingRequestResult::Type Result = MoveToLocation(ReachableDestination.Location, ACCEPTANCE_RADIUS);
		if (Result != EPathFollowingRequestResult::Failed)
		{
			++MovementOrderCount;
			return;
		}
	}

	// Keep moving if this map has no usable NavMesh; the progress watchdog retries
	// pathfinding and selects a different target or patrol point after a stall.
	bDirectSteering = true;
	SetActorTickEnabled(true);
	++MovementOrderCount;
}

void AFibulaBotController::ChooseRoamingDestination()
{
	AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot)
	{
		return;
	}

	UNavigationSystemV1 *NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavigationSystem)
	{
		FNavLocation RandomLocation;
		if (NavigationSystem->GetRandomReachablePointInRadius(Bot->GetActorLocation(), ROAM_RADIUS, RandomLocation))
		{
			RoamingDestination = RandomLocation.Location;
			bHasRoamingDestination = true;
			bDirectSteering = false;
			SetActorTickEnabled(false);
			return;
		}
	}

	// A small direct-steering fallback keeps bots mobile on maps without NavMesh.
	const float Angle = RandomStream.FRandRange(0.0f, 2.0f * PI);
	const float Radius = RandomStream.FRandRange(900.0f, ROAM_RADIUS);
	RoamingDestination = Bot->GetActorLocation() + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Radius;
	bHasRoamingDestination = true;
	bDirectSteering = true;
	SetActorTickEnabled(true);
}

bool AFibulaBotController::TryHeal()
{
	AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot || Bot->GetMaxHealth() <= 0)
	{
		return false;
	}

	const float HealthRatio = static_cast<float>(Bot->GetCurrentHealth()) / Bot->GetMaxHealth();
	if (HealthRatio < 0.65f)
	{
		if (Bot->GetVocation() == EVocation::Druid)
		{
			if (TryCastSpellFromList({TEXT("Mass Healing")}, false))
			{
				return true;
			}
		}
		else if (TryCastSpellFromList({TEXT("Ultimate Healing")}, false))
		{
			return true;
		}

		if (TryUseHealingItem())
		{
			return true;
		}
	}

	if (Bot->GetVocation() == EVocation::Druid)
	{
		if (AFibulaCharacter *Ally = FindBestAllyToHeal())
		{
			Bot->ServerSetHealingTarget(Ally);
			if (TryCastSpellFromList({TEXT("Heal Friend")}, false))
			{
				return true;
			}
		}
	}

	return false;
}

bool AFibulaBotController::TryUseHealingItem()
{
	AFibulaCharacter *Bot = GetBotCharacter();
	if (!Bot)
	{
		return false;
	}

	for (const FGameItem &Item : Bot->GetInventory())
	{
		if (Item.Name.Contains(TEXT("healing"), ESearchCase::IgnoreCase) ||
			Item.Name.Contains(TEXT("health potion"), ESearchCase::IgnoreCase))
		{
			Bot->ServerUseItem(Item, Bot->GetActorLocation());
			++CombatOrderCount;
			return true;
		}
	}

	return false;
}

bool AFibulaBotController::TryCastOffensiveSpell()
{
	switch (GetBotCharacter()->GetVocation())
	{
	case EVocation::Knight:
		return TryCastSpellFromList({TEXT("Annihilation"), TEXT("Fierce Berserk"), TEXT("Berserk")}, true);
	case EVocation::Paladin:
		return TryCastSpellFromList({TEXT("Ethereal Arrow"), TEXT("Sudden Death Rune"), TEXT("Holy Arrow")}, true);
	case EVocation::Sorcerer:
		return TryCastSpellFromList({TEXT("Flame Strike"), TEXT("Sudden Death Rune"), TEXT("Great Energy Beam"), TEXT("Energy Wave")}, true);
	case EVocation::Druid:
		return TryCastSpellFromList({TEXT("Sudden Death Rune"), TEXT("Eternal Winter")}, true);
	default:
		return false;
	}
}

bool AFibulaBotController::TryCastSpellFromList(const TArray<FString> &SpellNames, bool bOffensive)
{
	AFibulaCharacter *Bot = GetBotCharacter();
	AFibulaCharacter *Target = bOffensive ? CurrentTarget.Get() : nullptr;
	if (!Bot || (bOffensive && !Target))
	{
		return false;
	}

	if ((bOffensive && Bot->IsOffensivelyExhausted()) || (!bOffensive && Bot->IsGenerallyExhausted()))
	{
		return false;
	}

	for (const FString &SpellName : SpellNames)
	{
		const FSpellDefinition *Spell = USpellDatabase::GetSpell(SpellName);
		if (!Spell || !Spell->AllowedVocations.Contains(Bot->GetVocation()) || Bot->GetCurrentMana() < Spell->ManaCost)
		{
			continue;
		}

		if (Spell->GetIsRune() && Bot->GetItemCount(Spell->Name) <= 0)
		{
			continue;
		}

		AFibulaCharacter *SpellTarget = Spell->SpellType == ESpellType::TargetedHeal ? Bot->GetHealingTarget() : Target;
		if (Spell->bMustBeTargeted && !SpellTarget)
		{
			continue;
		}

		if (Spell->MaxTargetDistance > 0.0f && SpellTarget &&
			FVector::Distance(Bot->GetActorLocation(), SpellTarget->GetActorLocation()) > Spell->MaxTargetDistance)
		{
			continue;
		}

		Bot->ServerCastSpell(Spell->Name);
		++CombatOrderCount;
		return true;
	}

	return false;
}
