#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CharacterEnums.h"
#include "FibulaBotController.generated.h"

class AFibulaCharacter;

UCLASS()
class FIBULA_API AFibulaBotController : public AAIController
{
	GENERATED_BODY()

public:
	AFibulaBotController();

	virtual void OnPossess(APawn *InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	void InitializeBot(int32 Seed);
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	int32 GetTargetAcquisitionCount() const { return TargetAcquisitionCount; }
	int32 GetMovementOrderCount() const { return MovementOrderCount; }
	int32 GetMovementProgressCount() const { return MovementProgressCount; }
	int32 GetCombatOrderCount() const { return CombatOrderCount; }
	int32 GetDamageObservedCount() const { return DamageObservedCount; }
	AFibulaCharacter *GetTargetCharacter() const { return CurrentTarget.Get(); }

	static EVocation ChooseRandomVocation(FRandomStream &RandomStream);

private:
	void Think();
	AFibulaCharacter *GetBotCharacter() const;
	AFibulaCharacter *FindBestTarget() const;
	AFibulaCharacter *FindBestAllyToHeal() const;
	bool IsValidEnemy(AFibulaCharacter *Candidate) const;
	void MoveToward(const FVector &Destination);
	void ChooseRoamingDestination();
	bool TryHeal();
	bool TryCastOffensiveSpell();
	bool TryCastSpellFromList(const TArray<FString> &SpellNames, bool bOffensive);
	bool TryUseHealingItem();
	void ObserveTargetHealth();
	void ObserveMovementProgress();

	FRandomStream RandomStream;
	FTimerHandle ThinkTimerHandle;
	TWeakObjectPtr<AFibulaCharacter> CurrentTarget;
	TWeakObjectPtr<AFibulaCharacter> LastObservedTarget;
	TWeakObjectPtr<AFibulaCharacter> RecentlyStalledTarget;
	FVector LastProgressLocation = FVector::ZeroVector;
	FVector LastObservedLocation = FVector::ZeroVector;
	FVector RoamingDestination = FVector::ZeroVector;
	float LastProgressTime = 0.0f;
	float IgnoreStalledTargetUntil = 0.0f;
	int32 LastObservedHealth = 0;
	int32 TargetAcquisitionCount = 0;
	int32 MovementOrderCount = 0;
	int32 MovementProgressCount = 0;
	int32 CombatOrderCount = 0;
	int32 DamageObservedCount = 0;
	bool bHasRoamingDestination = false;
	bool bDirectSteering = false;

	static constexpr float THINK_INTERVAL_MIN = 0.65f;
	static constexpr float THINK_INTERVAL_MAX = 0.95f;
	static constexpr float TARGET_SEARCH_RADIUS = 10000.0f;
	static constexpr float ROAM_RADIUS = 3500.0f;
	static constexpr float STUCK_TIMEOUT = 5.0f;
	static constexpr float STUCK_DISTANCE = 100.0f;
	static constexpr float ACCEPTANCE_RADIUS = 175.0f;
};
