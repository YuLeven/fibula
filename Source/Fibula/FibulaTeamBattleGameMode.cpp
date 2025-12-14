

#include "FibulaTeamBattleGameMode.h"
#include "FibulaTeamBattleGameState.h"
#include "FibulaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TeamStash.h"
#include "GameFramework/GameModeBase.h"
#include "FibulaGameMode.h"
#include "PlayerStartingElements.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AFibulaTeamBattleGameMode::AFibulaTeamBattleGameMode()
{
	GameStateClass = AFibulaTeamBattleGameState::StaticClass();
	CurrentGameMode = EGameModeType::TeamBattle;
}

void AFibulaTeamBattleGameMode::PopulateTeamStashes()
{
	
	TArray<AActor *> FoundStashes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamStash::StaticClass(), FoundStashes);

	
	TArray<FGameItem> StartingItems = UPlayerStartingElements::GetTeamStashStartingItems();

	
	for (AActor *Actor : FoundStashes)
	{
		if (ATeamStash *Stash = Cast<ATeamStash>(Actor))
		{
			Stash->EmptyInventory();
			Stash->SetInventory(StartingItems);
		}
	}
}

void AFibulaTeamBattleGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartTeamBattle();
}

void AFibulaTeamBattleGameMode::AssignTeam(AFibulaCharacter *Character)
{
	if (!Character)
	{
		return;
	}

	int32 TeamACount = 0;
	int32 TeamBCount = 0;

	for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
	{
		AFibulaCharacter *ExistingCharacter = *It;
		if (ExistingCharacter->GetTeamId() == 1)
		{
			TeamACount++;
		}
		else if (ExistingCharacter->GetTeamId() == 2)
		{
			TeamBCount++;
		}
	}

	int32 NewTeamId;
	if (TeamACount <= TeamBCount)
	{
		NewTeamId = 1;
		Character->SetTeamId(NewTeamId);
	}
	else
	{
		NewTeamId = 2;
		Character->SetTeamId(NewTeamId);
	}

	UE_LOG(LogTemp, Log, TEXT("AssignTeam: New team sizes - Team A: %d, Team B: %d"), TeamACount, TeamBCount);
}

bool AFibulaTeamBattleGameMode::CanDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target) const
{
	if (!Attacker || !Target)
		return false;

	return Attacker->IsOpponentOf(Target);
}

bool AFibulaTeamBattleGameMode::CanHeal(AFibulaCharacter *Healer, AFibulaCharacter *Target) const
{
	if (!Healer || !Target)
		return false;

	return Healer->IsAllyOf(Target);
}

AActor *AFibulaTeamBattleGameMode::FindTeamPlayerStart(int32 TeamId)
{
	TArray<AActor *> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	
	if (PlayerStarts.Num() < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough player starts found. Need at least 2, found %d"), PlayerStarts.Num());
		return nullptr;
	}

	TArray<AActor *> ValidStarts;

	
	int32 MidPoint = PlayerStarts.Num() / 2;

	if (TeamId == 1)
	{
		
		for (int32 i = 0; i < MidPoint; i++)
		{
			ValidStarts.Add(PlayerStarts[i]);
		}
	}
	else
	{
		
		for (int32 i = MidPoint; i < PlayerStarts.Num(); i++)
		{
			ValidStarts.Add(PlayerStarts[i]);
		}
	}

	
	if (ValidStarts.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, ValidStarts.Num() - 1);
		return ValidStarts[RandomIndex];
	}

	UE_LOG(LogTemp, Error, TEXT("No valid player starts found for team %d!"), TeamId);
	return nullptr;
}

void AFibulaTeamBattleGameMode::OnPlayerDeath(AFibulaCharacter *Character, AFibulaCharacter *Killer)
{
	Super::OnPlayerDeath(Character, Killer);
	if (!HasAuthority() || !Character)
		return;

	if (AFibulaTeamBattleGameState *TBGameState = Cast<AFibulaTeamBattleGameState>(GetGameState<AFibulaGameState>()))
	{
		TBGameState->OnPlayerDeath(Character->GetTeamId());
	}

	if (HasTeamWon())
	{
		SetMatchState(TEXT("WaitingPostMatch"));
	}
}

void AFibulaTeamBattleGameMode::StartTeamBattle()
{
	if (!HasAuthority())
		return;

	PopulateTeamStashes();
	SetMatchState(TEXT("WaitingToStart"));
	FTimerHandle StartMatchTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		StartMatchTimerHandle,
		[this]()
		{
			SetMatchState(TEXT("InProgress"));
		},
		2.0f,
		false);
}

void AFibulaTeamBattleGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	if (!HasAuthority())
		return;

	if (AFibulaTeamBattleGameState *TBGameState = Cast<AFibulaTeamBattleGameState>(GameState))
	{
		TBGameState->SetTeamPoints(1, 0);
		TBGameState->SetTeamPoints(2, 0);
		TBGameState->SetBattleStartTime(GetWorld()->GetTimeSeconds());
	}
}

void AFibulaTeamBattleGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	if (!HasAuthority())
		return;

	GetWorld()->GetTimerManager().SetTimer(
		BattleEndTimerHandle,
		[this]()
		{
			SetMatchState(TEXT("WaitingPostMatch"));
		},
		BATTLE_DURATION,
		false);
}

void AFibulaTeamBattleGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	if (!HasAuthority())
		return;

	AFibulaTeamBattleGameState *TBGameState = Cast<AFibulaTeamBattleGameState>(GameState);
	if (!TBGameState)
		return;

	GetWorld()->GetTimerManager().ClearTimer(BattleEndTimerHandle);

	int32 TeamOnePoints = TBGameState->GetTeamPoints(1);
	int32 TeamTwoPoints = TBGameState->GetTeamPoints(2);
	int32 WinningTeamId = (TeamOnePoints > TeamTwoPoints) ? 1 : (TeamTwoPoints > TeamOnePoints) ? 2
																								: 0;

	
	AwardPresentsToWinningTeam(WinningTeamId);

	
	TBGameState->MulticastOnBattleEnd(WinningTeamId);
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController *PC = It->Get())
		{
			if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
			{
				Character->ShowCenteredMessage(TEXT("The battle has ended. You will be kicked out shortly."), 15.0f, EFloatingTextType::MajorInfo);
			}
		}
	}

	
	GetWorld()->GetTimerManager().SetTimer(
		BattleEndTimerHandle,
		[this]()
		{
			CleanupAfterBattle();
		},
		END_BATTLE_DELAY,
		false);
}

void AFibulaTeamBattleGameMode::CleanupAfterBattle()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController *PC = It->Get())
		{
			if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
			{
				Character->SetInProtectionZone(true);
				KickPlayer(PC, TEXT("Battle has ended"));
			}
		}
	}

	
	GetWorld()->GetTimerManager().SetTimer(
		NewBattleTimerHandle,
		[this]()
		{
			StartTeamBattle();
		},
		NEW_BATTLE_DELAY,
		false);
}

void AFibulaTeamBattleGameMode::AwardPresentsToWinningTeam(int32 WinningTeamId)
{
	if (WinningTeamId == 0)
		return;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController *PC = It->Get())
		{
			if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
			{
				if (Character->GetTeamId() == WinningTeamId)
				{
					const FGameItem *RewardItem = UItemDatabase::GetItem(TEXT("Reward Present"));
					if (RewardItem)
					{
						FGameItem RewardPresent = *RewardItem;
						Character->AddItemDirect(RewardPresent);
						Character->AddItemDirect(RewardPresent);
					}
				}
			}
		}
	}
}

bool AFibulaTeamBattleGameMode::HasTeamWon() const
{
	AFibulaTeamBattleGameState *TBGameState = Cast<AFibulaTeamBattleGameState>(GameState);
	if (!TBGameState)
		return false;

	int32 TeamOnePoints = TBGameState->GetTeamPoints(1);
	int32 TeamTwoPoints = TBGameState->GetTeamPoints(2);

	return TeamOnePoints >= POINTS_TO_WIN ||
		   TeamTwoPoints >= POINTS_TO_WIN ||
		   !TBGameState->IsBattleActive();
}
