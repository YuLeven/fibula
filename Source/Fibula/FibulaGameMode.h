

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FibulaGameState.h"
#include "FibulaHUD.h"
#include "ServerStatusReporter.h"
#include "GameModeType.h"
#include "FibulaGameMode.generated.h"

UCLASS(minimalapi)
class AFibulaGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AFibulaGameMode();
	virtual void BeginPlay() override;
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

protected:
	UPROPERTY(EditAnywhere, Category = "Game Mode")
	EGameModeType CurrentGameMode;

	virtual int32 GetMaxPlayers() const { return 44; }

	virtual void PopulateTeamStashes();
	virtual void AssignTeam(AFibulaCharacter *Character);

private:
	EVocation StringToVocation(const FString &VocationStr);
	void LoadCharacterData(APlayerController *NewPlayer, const FString &CharacterName, const FString &Token);
	void OnCharacterDataLoaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess, APlayerController *PlayerController);

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
	static constexpr float ITEM_PERSISTENCE_INTERVAL = 300.0f; 
};
