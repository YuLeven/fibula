

#pragma once

#include "CoreMinimal.h"
#include "FibulaGameMode.h"
#include "GameModeType.h"
#include "FibulaTeamBattleGameMode.generated.h"

UCLASS(minimalapi)
class AFibulaTeamBattleGameMode : public AFibulaGameMode
{
	GENERATED_BODY()

public:
	AFibulaTeamBattleGameMode();
	virtual void BeginPlay() override;
	virtual bool CanDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target) const override;
	virtual bool CanHeal(AFibulaCharacter *Healer, AFibulaCharacter *Target) const override;
	virtual AActor *FindTeamPlayerStart(int32 TeamId) override;
	virtual void OnPlayerDeath(AFibulaCharacter *Character, AFibulaCharacter *Killer) override;

	void StartTeamBattle();
	bool HasTeamWon() const;
	void HandleMatchIsWaitingToStart();
	void HandleMatchHasStarted();
	void HandleMatchHasEnded();

	static constexpr float BATTLE_DURATION = 1500.0f;

protected:
	virtual int32 GetMaxPlayers() const override { return 24; }

private:
	virtual void AssignTeam(AFibulaCharacter *Character) override;
	virtual void PopulateTeamStashes() override;
	void AwardPresentsToWinningTeam(int32 WinningTeamId);
	void CleanupAfterBattle();

	static constexpr float END_BATTLE_DELAY = 20.0f;
	static constexpr float NEW_BATTLE_DELAY = 5.0f;
	static constexpr int32 POINTS_TO_WIN = 30;

	FTimerHandle BattleEndTimerHandle;
	FTimerHandle NewBattleTimerHandle;
};
