#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FibulaGameState.h"
#include "FibulaHUD.h"
#include "ServerStatusReporter.h"
#include "GameModeType.h"
#include "FibulaGameMode.generated.h"

class AFibulaBotController;
class AFibulaCharacter;

UCLASS(Config = Game, minimalapi)
class AFibulaGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AFibulaGameMode();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PreLogin(const FString &Options, const FString &Address, const FUniqueNetIdRepl &UniqueId, FString &ErrorMessage) override;
	virtual void PostLogin(APlayerController *NewPlayer) override;
	void InitializePlayerCharacter(APlayerController *NewPlayer, const FString &PlayerName, const FString &VocationStr);
	virtual FString InitNewPlayer(APlayerController *NewPlayerController, const FUniqueNetIdRepl &UniqueId, const FString &Options, const FString &Portal = TEXT("")) override;
	virtual void Logout(AController *Exiting) override;

	virtual bool CanDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target) const;
	virtual bool CanHeal(AFibulaCharacter *Healer, AFibulaCharacter *Target) const;
	void PersistCharacterItems(AFibulaCharacter *Character);
	virtual AActor *FindTeamPlayerStart(int32 TeamId);
	void KickPlayer(APlayerController *PlayerController, const FString &Reason);
	virtual void OnPlayerDeath(AFibulaCharacter *Character, AFibulaCharacter *Killer);
	virtual EGameModeType GetCurrentGameMode() const { return CurrentGameMode; }

	int32 GetActiveBotCount() const;
#if !UE_BUILD_SHIPPING
	void RecordAutomationBotDamage(int32 DamageAmount);
#endif

protected:
	UPROPERTY(EditAnywhere, Config, Category = "Bots")
	bool bEnableServerBots = true;

	// Desired total population, counting human players and bots.
	UPROPERTY(EditAnywhere, Config, Category = "Bots", meta = (ClampMin = "0"))
	int32 BotTargetPopulation = 8;

	UPROPERTY(EditAnywhere, Config, Category = "Bots", meta = (ClampMin = "0"))
	int32 MaximumBotCount = 12;

	UPROPERTY(EditAnywhere, Config, Category = "Bots", meta = (ClampMin = "0"))
	int32 BotMinimumHumanPlayers = 1;

	UPROPERTY(EditAnywhere, Config, Category = "Bots", meta = (ClampMin = "0.5"))
	float BotPopulationCheckInterval = 3.0f;

	// A nonzero seed makes bot vocations and decisions reproducible.
	UPROPERTY(EditAnywhere, Config, Category = "Bots")
	int32 BotRandomSeed = 0;

	UPROPERTY(EditAnywhere, Category = "Game Mode")
	EGameModeType CurrentGameMode;

	virtual int32 GetMaxPlayers() const { return 44; }

	virtual void PopulateTeamStashes();
	virtual void AssignTeam(AFibulaCharacter *Character);

private:
	EVocation StringToVocation(const FString &VocationStr);
	void LoadCharacterData(APlayerController *NewPlayer, const FString &CharacterName, const FString &Token);
	void OnCharacterDataLoaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess, APlayerController *PlayerController);
	void ReconcileBotPopulation();
	bool SpawnBot();
	void EquipStartingEquipment(AFibulaCharacter *Character);
	int32 GetHumanPlayerCount() const;
	void InitializeBotRandomStream();

#if !UE_BUILD_SHIPPING
	bool IsAutomationBotTestMode() const;
	void InitializeAutomationTestPlayer(APlayerController *NewPlayer);
	void StageAutomationBotTestPlayers();
	void WriteAutomationBotTestResult();
	FTimerHandle AutomationBotTestTimerHandle;
	FTimerHandle AutomationBotTestStageTimerHandle;
	int32 AutomationTestPlayerCount = 0;
	int32 AutomationBotDamage = 0;
	int32 AutomationTestBotSpawnCount = 0;
#endif

	const TArray<FString> FamousPlayerNames = {
		TEXT("Bubble"),
		TEXT("Kotku"),
		TEXT("Masakrowiec"),
		TEXT("Eternal Oblivion"),
		TEXT("Xanadu"),
		TEXT("Cachero"),
		TEXT("Mithrandir of the Istari"),
		TEXT("Thee Panda"),
		TEXT("Arieswar"),
		TEXT("Panxor")};

	UPROPERTY()
	UServerStatusReporter *ServerStatusReporter;

	TSubclassOf<APawn> PlayerPawnClass;

	AFibulaCharacter *FindExistingCharacter(const FString &CharacterName);

	void StartItemPersistenceTimer();
	void OnPersistItemsTimerTick();

	FTimerHandle ItemPersistenceTimerHandle;
	FTimerHandle BotPopulationTimerHandle;
	FRandomStream BotRandomStream;
	int32 NextBotSerial = 1;
	static constexpr float ITEM_PERSISTENCE_INTERVAL = 300.0f;
};
