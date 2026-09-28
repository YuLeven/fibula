

#include "FibulaGameMode.h"
#include "FibulaCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "FibulaLoginGameMode.h"
#include "FibulaPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameSession.h"
#include "PlayerStartingElements.h"
#include "TeamStash.h"
#include "EngineUtils.h"
#include "Http.h"
#include "Json.h"
#include "Config/ServerConfig.h"
#include "GameFramework/SpectatorPawn.h"
#include "FibulaPlayerController.h"
#include "GameModeType.h"
#include "FibulaBotController.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

FString URLDecode(const FString &EncodedString)
{
	FString DecodedString = EncodedString;
	DecodedString = DecodedString.Replace(TEXT("%20"), TEXT(" "));
	DecodedString = DecodedString.Replace(TEXT("%21"), TEXT("!"));
	DecodedString = DecodedString.Replace(TEXT("%27"), TEXT("'"));
	DecodedString = DecodedString.Replace(TEXT("%28"), TEXT("("));
	DecodedString = DecodedString.Replace(TEXT("%29"), TEXT(")"));
	DecodedString = DecodedString.Replace(TEXT("%2B"), TEXT("+"));
	DecodedString = DecodedString.Replace(TEXT("%2C"), TEXT(","));
	DecodedString = DecodedString.Replace(TEXT("%2F"), TEXT("/"));
	DecodedString = DecodedString.Replace(TEXT("%3A"), TEXT(":"));
	DecodedString = DecodedString.Replace(TEXT("%3D"), TEXT("="));
	DecodedString = DecodedString.Replace(TEXT("%3F"), TEXT("?"));
	return DecodedString;
}

AFibulaGameMode::AFibulaGameMode()
{
	
	DefaultPawnClass = ASpectatorPawn::StaticClass();

	
	GameStateClass = AFibulaGameState::StaticClass();
	PlayerStateClass = AFibulaPlayerState::StaticClass();
	PlayerControllerClass = AFibulaPlayerController::StaticClass();

	
	HUDClass = AFibulaHUD::StaticClass();
	bReplicates = true;
	bUseSeamlessTravel = false;
	CurrentGameMode = EGameModeType::FreeForAll;

	if (HasAuthority())
	{
		ServerStatusReporter = CreateDefaultSubobject<UServerStatusReporter>(TEXT("ServerStatusReporter"));
	}

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class)
	{
		PlayerPawnClass = PlayerPawnBPClass.Class;
	}
}

void AFibulaGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		PopulateTeamStashes();
		bool bIsAutomationTest = false;
#if !UE_BUILD_SHIPPING
		bIsAutomationTest = IsAutomationBotTestMode();
#endif
		if (!bIsAutomationTest && GameSession && ServerStatusReporter)
		{
			int32 Port = GetWorld()->URL.Port;

			ServerStatusReporter->Initialize(Port, GetMaxPlayers());
			ServerStatusReporter->StartReporting();
		}

		InitializeBotRandomStream();
		GetWorldTimerManager().SetTimer(
			BotPopulationTimerHandle,
			this,
			&AFibulaGameMode::ReconcileBotPopulation,
			FMath::Max(0.5f, BotPopulationCheckInterval),
			true);
	}
}

void AFibulaGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BotPopulationTimerHandle);
#if !UE_BUILD_SHIPPING
	GetWorldTimerManager().ClearTimer(AutomationBotTestTimerHandle);
	GetWorldTimerManager().ClearTimer(AutomationBotTestStageTimerHandle);
#endif
	Super::EndPlay(EndPlayReason);
}

void AFibulaGameMode::PopulateTeamStashes()
{
	
}

FString AFibulaGameMode::InitNewPlayer(APlayerController *NewPlayerController, const FUniqueNetIdRepl &UniqueId, const FString &Options, const FString &Portal)
{
	
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	UE_LOG(LogTemp, Log, TEXT("FibulaGameMode InitNewPlayer"));
	UE_LOG(LogTemp, Log, TEXT("InitNewPlayer Options: %s"), *Options);
	if (!ErrorMessage.IsEmpty())
	{
		return ErrorMessage;
	}

#if !UE_BUILD_SHIPPING
	// The local bot integration harness uses disposable characters, never account credentials.
	if (IsAutomationBotTestMode())
	{
		return ErrorMessage;
	}
#endif

	
	FString PlayerNamePrefix = TEXT("PlayerName=");
	FString TokenPrefix = TEXT("Token=");

	FString PlayerName;
	FString Token;

	
	int32 PlayerNameStart = Options.Find(PlayerNamePrefix);
	if (PlayerNameStart != INDEX_NONE)
	{
		PlayerNameStart += PlayerNamePrefix.Len();
		int32 PlayerNameEnd = Options.Find(TEXT("?"), ESearchCase::IgnoreCase, ESearchDir::FromStart, PlayerNameStart);
		FString EncodedName = (PlayerNameEnd == INDEX_NONE) ? Options.Mid(PlayerNameStart) : Options.Mid(PlayerNameStart, PlayerNameEnd - PlayerNameStart);

		PlayerName = URLDecode(EncodedName);
	}

	
	int32 TokenStart = Options.Find(TokenPrefix);
	if (TokenStart != INDEX_NONE)
	{
		TokenStart += TokenPrefix.Len();
		int32 TokenEnd = Options.Find(TEXT("?"), ESearchCase::IgnoreCase, ESearchDir::FromStart, TokenStart);
		Token = (TokenEnd == INDEX_NONE) ? Options.Mid(TokenStart) : Options.Mid(TokenStart, TokenEnd - TokenStart);
	}

	
	LoadCharacterData(NewPlayerController, PlayerName, Token);

	return ErrorMessage;
}

void AFibulaGameMode::PreLogin(const FString &Options, const FString &Address, const FUniqueNetIdRepl &UniqueId, FString &ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
#if !UE_BUILD_SHIPPING
	if (IsAutomationBotTestMode() &&
		!Address.StartsWith(TEXT("127.")) &&
		!Address.StartsWith(TEXT("::1")))
	{
		ErrorMessage = TEXT("Bot test sessions only accept loopback clients.");
	}
#endif
}

void AFibulaGameMode::PostLogin(APlayerController *NewPlayer)
{
	Super::PostLogin(NewPlayer);

#if !UE_BUILD_SHIPPING
	if (IsAutomationBotTestMode())
	{
		InitializeAutomationTestPlayer(NewPlayer);
		GetWorldTimerManager().ClearTimer(AutomationBotTestStageTimerHandle);
		GetWorldTimerManager().SetTimer(
			AutomationBotTestStageTimerHandle,
			this,
			&AFibulaGameMode::StageAutomationBotTestPlayers,
			12.0f,
			false);
		GetWorldTimerManager().ClearTimer(AutomationBotTestTimerHandle);
		GetWorldTimerManager().SetTimer(
			AutomationBotTestTimerHandle,
			this,
			&AFibulaGameMode::WriteAutomationBotTestResult,
			80.0f,
			false);
	}
#endif
}

void AFibulaGameMode::InitializePlayerCharacter(APlayerController *NewPlayer, const FString &PlayerName, const FString &VocationStr)
{
	if (!NewPlayer)
		return;

	AFibulaCharacter *Character = Cast<AFibulaCharacter>(NewPlayer->GetPawn());
	if (!Character)
		return;

	
	Character->SetCharacterName(PlayerName);
	
	
	AssignTeam(Character);
	EVocation Vocation = StringToVocation(VocationStr);
	Character->SetVocation(Vocation);
	Character->SetIsTeamBattle(CurrentGameMode == EGameModeType::TeamBattle);

	AActor *PlayerStart = FindTeamPlayerStart(Character->GetTeamId());
	if (PlayerStart)
	{
		FVector SpawnLocation = PlayerStart->GetActorLocation();
		FRotator SpawnRotation = PlayerStart->GetActorRotation();
		Character->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	}

	
	Character->InitializeCharacterStats();
	ReconcileBotPopulation();
}

EVocation AFibulaGameMode::StringToVocation(const FString &VocationStr)
{
	if (VocationStr.Equals(TEXT("Knight"), ESearchCase::IgnoreCase))
		return EVocation::Knight;
	else if (VocationStr.Equals(TEXT("Paladin"), ESearchCase::IgnoreCase))
		return EVocation::Paladin;
	else if (VocationStr.Equals(TEXT("Sorcerer"), ESearchCase::IgnoreCase))
		return EVocation::Sorcerer;
	else if (VocationStr.Equals(TEXT("Druid"), ESearchCase::IgnoreCase))
		return EVocation::Druid;

	return EVocation::Sorcerer;
}

void AFibulaGameMode::AssignTeam(AFibulaCharacter *Character)
{
	if (!Character)
	{
		return;
	}
	Character->SetTeamId(0);
}

bool AFibulaGameMode::CanDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target) const
{
	return true;
}

bool AFibulaGameMode::CanHeal(AFibulaCharacter *Healer, AFibulaCharacter *Target) const
{
	return true;
}

AActor *AFibulaGameMode::FindTeamPlayerStart(int32 TeamId)
{
	return nullptr;
}

void AFibulaGameMode::Logout(AController *Exiting)
{
	Super::Logout(Exiting);
	ReconcileBotPopulation();
}

AFibulaCharacter *AFibulaGameMode::FindExistingCharacter(const FString &CharacterName)
{
	for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
	{
		AFibulaCharacter *Character = *It;
		if (Character && Character->GetCharacterName().Equals(CharacterName, ESearchCase::IgnoreCase))
		{
			return Character;
		}
	}
	return nullptr;
}

void AFibulaGameMode::LoadCharacterData(APlayerController *NewPlayer, const FString &CharacterName, const FString &Token)
{
	
	if (AFibulaCharacter *ExistingCharacter = FindExistingCharacter(CharacterName))
	{
		
		AController *OldController = ExistingCharacter->GetController();

		
		if (OldController)
		{
			OldController->UnPossess();

			
			if (APlayerController *OldPC = Cast<APlayerController>(OldController))
			{
				OldPC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Your character has logged in from another location")));
			}
		}

		
		NewPlayer->Possess(ExistingCharacter);

		
		if (AFibulaPlayerState *PlayerState = NewPlayer->GetPlayerState<AFibulaPlayerState>())
		{
			const FString VocationStr = ExistingCharacter->GetVocationAsString();
			PlayerState->SetPlayerVocation(VocationStr);
		}
		ReconcileBotPopulation();
		return;
	}

	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	FString URL = FString::Printf(TEXT("%s/api/characters/%s"), *ServerConfig::GetLoginServer(), *FGenericPlatformHttp::UrlEncode(CharacterName));

	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));

	HttpRequest->OnProcessRequestComplete().BindUObject(
		this,
		&AFibulaGameMode::OnCharacterDataLoaded,
		NewPlayer);

	HttpRequest->ProcessRequest();
}

void AFibulaGameMode::OnCharacterDataLoaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess, APlayerController *PlayerController)
{
	if (!bSuccess || !Response.IsValid() || !PlayerController)
	{
		KickPlayer(PlayerController, TEXT("Failed to load character data"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (GetNumPlayers() >= GetMaxPlayers())
	{
		KickPlayer(PlayerController, TEXT("Server is full"));
		return;
	}

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		KickPlayer(PlayerController, TEXT("Invalid character data"));
		return;
	}

	const TSharedPtr<FJsonObject> *CharacterObject;
	if (!JsonObject->TryGetObjectField(TEXT("character"), CharacterObject))
	{
		KickPlayer(PlayerController, TEXT("Invalid character data format"));
		return;
	}

	
	FString PlayerName = (*CharacterObject)->GetStringField(TEXT("name"));
	FString VocationStr = (*CharacterObject)->GetStringField(TEXT("vocation"));

	
	if (PlayerController->GetPawn())
	{
		PlayerController->GetPawn()->Destroy();
	}

	
	AActor *StartSpot = FindPlayerStart(PlayerController);
	if (!StartSpot)
	{
		KickPlayer(PlayerController, TEXT("Could not find spawn point"));
		return;
	}

	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (!PlayerPawnClass)
	{
		KickPlayer(PlayerController, TEXT("Character blueprint class not found"));
		return;
	}

	AFibulaCharacter *Character = GetWorld()->SpawnActor<AFibulaCharacter>(
		PlayerPawnClass,
		StartSpot->GetActorLocation(),
		StartSpot->GetActorRotation(),
		SpawnParams);

	if (!Character)
	{
		KickPlayer(PlayerController, TEXT("Failed to spawn character"));
		return;
	}

	
	const TArray<TSharedPtr<FJsonValue>> *EquipmentArray = nullptr;
	if ((*CharacterObject)->TryGetArrayField("equipment", EquipmentArray) && EquipmentArray)
	{
		for (const auto &EquipItem : *EquipmentArray)
		{
			UE_LOG(LogTemp, Log, TEXT("Loading item: %s"), *EquipItem->AsObject()->GetStringField("item"));
			TSharedPtr<FJsonObject> ItemObj = EquipItem->AsObject();
			if (!ItemObj)
				continue;

			FString SlotType = ItemObj->GetStringField("slot");
			FString ItemName = ItemObj->GetStringField("item");
			int32 Count = ItemObj->GetIntegerField("count");

			if (const FGameItem *ItemData = UItemDatabase::GetItem(*ItemName))
			{
				FGameItem Item = *ItemData;
				Item.StackCount = Count;

				
				if (ItemObj->HasField("charges"))
				{
					Item.EquipmentAttributes.Charges = ItemObj->GetIntegerField("charges");
				}

				Character->AddItemDirect(Item);
				Character->EquipItem(Item);
			}
		}
	}

	const TArray<TSharedPtr<FJsonValue>> *InventoryArray = nullptr;
	if ((*CharacterObject)->TryGetArrayField("inventory", InventoryArray) && InventoryArray)
	{
		for (const auto &InvItem : *InventoryArray)
		{
			UE_LOG(LogTemp, Log, TEXT("Loading item: %s"), *InvItem->AsObject()->GetStringField("item"));
			TSharedPtr<FJsonObject> ItemObj = InvItem->AsObject();
			if (!ItemObj)
				continue;

			FString ItemName = ItemObj->GetStringField("item");
			int32 Count = ItemObj->GetIntegerField("count");

			if (const FGameItem *ItemData = UItemDatabase::GetItem(*ItemName))
			{
				FGameItem Item = *ItemData;
				Item.StackCount = Count;

				
				if (ItemObj->HasField("charges"))
				{
					Item.EquipmentAttributes.Charges = ItemObj->GetIntegerField("charges");
				}

				Character->AddItemDirect(Item);
			}
		}
	}

	PlayerController->Possess(Character);

	
	if (AFibulaPlayerState *PlayerState = PlayerController->GetPlayerState<AFibulaPlayerState>())
	{
		PlayerState->SetPlayerVocation(VocationStr);
		InitializePlayerCharacter(PlayerController, PlayerName, VocationStr);
	}
}

void AFibulaGameMode::KickPlayer(APlayerController *PlayerController, const FString &Reason)
{
	if (PlayerController)
	{
		PlayerController->ClientReturnToMainMenuWithTextReason(FText::FromString(Reason));
	}
}

void AFibulaGameMode::StartItemPersistenceTimer()
{
	GetWorldTimerManager().SetTimer(
		ItemPersistenceTimerHandle,
		this,
		&AFibulaGameMode::OnPersistItemsTimerTick,
		ITEM_PERSISTENCE_INTERVAL,
		true);
}

void AFibulaGameMode::OnPersistItemsTimerTick()
{
	
	for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
	{
		AFibulaCharacter *Character = *It;
		if (Character && Character->GetController() && !Character->IsPersistenceSuppressed())
		{
			PersistCharacterItems(Character);
		}
	}
}

void AFibulaGameMode::PersistCharacterItems(AFibulaCharacter *Character)
{
	if (!Character || Character->IsPersistenceSuppressed())
		return;

	
	TArray<TSharedPtr<FJsonValue>> Equipment;

	
	auto AddEquipmentItem = [&Equipment](const FString &Slot, const FGameItem &Item)
	{
		if (!Item.Name.IsEmpty())
		{
			TSharedPtr<FJsonObject> EquipItem = MakeShared<FJsonObject>();
			EquipItem->SetStringField("slot", Slot);
			EquipItem->SetStringField("item", Item.Name);
			EquipItem->SetNumberField("count", Item.StackCount);

			
			if (Item.EquipmentAttributes.Charges > 0)
			{
				EquipItem->SetNumberField("charges", Item.EquipmentAttributes.Charges);
			}

			Equipment.Add(MakeShared<FJsonValueObject>(EquipItem));
		}
	};

	
	AddEquipmentItem("helm", Character->GetEquippedHelm());
	AddEquipmentItem("armor", Character->GetEquippedArmor());
	AddEquipmentItem("weapon", Character->GetEquippedWeapon());
	AddEquipmentItem("shield", Character->GetEquippedShield());
	AddEquipmentItem("legs", Character->GetEquippedLegs());
	AddEquipmentItem("boots", Character->GetEquippedBoots());
	AddEquipmentItem("amulet", Character->GetEquippedAmulet());
	AddEquipmentItem("ring", Character->GetEquippedRing());
	AddEquipmentItem("ammunition", Character->GetEquippedAmmunition());
	AddEquipmentItem("bag", Character->GetEquippedBag());

	
	TArray<TSharedPtr<FJsonValue>> Inventory;
	for (const FGameItem &Item : Character->GetInventory())
	{
		TSharedPtr<FJsonObject> InvItem = MakeShared<FJsonObject>();
		InvItem->SetStringField("item", Item.Name);
		InvItem->SetNumberField("count", Item.StackCount);

		
		if (Item.EquipmentAttributes.Charges > 0)
		{
			InvItem->SetNumberField("charges", Item.EquipmentAttributes.Charges);
		}

		Inventory.Add(MakeShared<FJsonValueObject>(InvItem));
	}

	
	TSharedPtr<FJsonObject> RequestJson = MakeShared<FJsonObject>();
	TSharedPtr<FJsonObject> ItemsJson = MakeShared<FJsonObject>();

	ItemsJson->SetArrayField("equipment", Equipment);
	ItemsJson->SetArrayField("inventory", Inventory);
	RequestJson->SetObjectField("items", ItemsJson);

	// Convert to string
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestJson.ToSharedRef(), Writer);

	// Send HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	// Add Basic Auth header
	FString AuthHeader = FString::Printf(TEXT("%s:%s"), *ServerConfig::GetServerUsername(), *ServerConfig::GetServerPassword());
	FString EncodedAuth = FBase64::Encode(AuthHeader);
	HttpRequest->SetHeader("Authorization", FString::Printf(TEXT("Basic %s"), *EncodedAuth));

	FString URL = FString::Printf(TEXT("http://%s/backend/characters/%s/items"),
								  *ServerConfig::GetBackendServer(),
								  *FGenericPlatformHttp::UrlEncode(Character->GetCharacterName()));

	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetContentAsString(RequestBody);

	
	HttpRequest->ProcessRequest();
}

void AFibulaGameMode::OnPlayerDeath(AFibulaCharacter *Character, AFibulaCharacter *Killer)
{
	if (!HasAuthority() || !Character || !Killer)
		return;

	if (AFibulaGameState *FibulaGameState = GetGameState<AFibulaGameState>())
	{
		FibulaGameState->AddCharacterDeath(Character);
		FibulaGameState->AddCharacterKill(Killer);
	}
}

int32 AFibulaGameMode::GetHumanPlayerCount() const
{
	int32 HumanPlayerCount = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController *PlayerController = It->Get();
		const AFibulaCharacter *Character = PlayerController ? Cast<AFibulaCharacter>(PlayerController->GetPawn()) : nullptr;
		if (Character && !Character->IsBot())
		{
			++HumanPlayerCount;
		}
	}
	return HumanPlayerCount;
}

int32 AFibulaGameMode::GetActiveBotCount() const
{
	int32 BotCount = 0;
	for (TActorIterator<AFibulaBotController> It(GetWorld()); It; ++It)
	{
		const AFibulaCharacter *BotCharacter = Cast<AFibulaCharacter>(It->GetPawn());
		if (BotCharacter && BotCharacter->IsBot())
		{
			++BotCount;
		}
	}
	return BotCount;
}

void AFibulaGameMode::InitializeBotRandomStream()
{
	int32 CommandLineSeed = 0;
	if (FParse::Value(FCommandLine::Get(), TEXT("FibulaBotSeed="), CommandLineSeed))
	{
		BotRandomSeed = CommandLineSeed;
	}
	BotRandomStream.Initialize(BotRandomSeed != 0 ? BotRandomSeed : FMath::Rand());
}

void AFibulaGameMode::ReconcileBotPopulation()
{
	if (!HasAuthority() || !GetWorld())
	{
		return;
	}

	const int32 HumanPlayerCount = GetHumanPlayerCount();
	const int32 CurrentBotCount = GetActiveBotCount();
	int32 RequiredHumanCount = BotMinimumHumanPlayers;
#if !UE_BUILD_SHIPPING
	if (IsAutomationBotTestMode())
	{
		RequiredHumanCount = FMath::Max(2, RequiredHumanCount);
	}
#endif

	int32 DesiredBotCount = 0;
	if (bEnableServerBots && HumanPlayerCount >= RequiredHumanCount)
	{
		const int32 OpenGameSlots = FMath::Max(0, GetMaxPlayers() - HumanPlayerCount);
		DesiredBotCount = FMath::Min3(
			MaximumBotCount,
			FMath::Max(0, BotTargetPopulation - HumanPlayerCount),
			OpenGameSlots);
	}

	int32 RemainingBotCount = CurrentBotCount;
	while (RemainingBotCount > DesiredBotCount)
	{
		int32 TeamOneCount = 0;
		int32 TeamTwoCount = 0;
		if (CurrentGameMode == EGameModeType::TeamBattle)
		{
			for (TActorIterator<AFibulaCharacter> CharacterIt(GetWorld()); CharacterIt; ++CharacterIt)
			{
				if ((*CharacterIt)->GetTeamId() == 1)
				{
					++TeamOneCount;
				}
				else if ((*CharacterIt)->GetTeamId() == 2)
				{
					++TeamTwoCount;
				}
			}
		}

		const int32 TeamToReduce = TeamOneCount > TeamTwoCount ? 1 : TeamTwoCount > TeamOneCount ? 2 : 0;
		AFibulaBotController *BotToRemove = nullptr;
		for (TActorIterator<AFibulaBotController> It(GetWorld()); It; ++It)
		{
			AFibulaBotController *CandidateController = *It;
			const AFibulaCharacter *CandidateCharacter = Cast<AFibulaCharacter>(CandidateController->GetPawn());
			if (!CandidateCharacter || !CandidateCharacter->IsBot())
			{
				continue;
			}
			if (!BotToRemove)
			{
				BotToRemove = CandidateController;
			}
			if (TeamToReduce != 0 && CandidateCharacter->GetTeamId() == TeamToReduce)
			{
				BotToRemove = CandidateController;
				break;
			}
		}

		if (!BotToRemove)
		{
			break;
		}

		AFibulaCharacter *BotCharacter = Cast<AFibulaCharacter>(BotToRemove->GetPawn());
		BotToRemove->UnPossess();
		if (BotCharacter)
		{
			BotCharacter->Destroy();
		}
		BotToRemove->Destroy();
		--RemainingBotCount;
	}

	for (int32 Index = RemainingBotCount; Index < DesiredBotCount; ++Index)
	{
		if (!SpawnBot())
		{
			break;
		}
	}
}

void AFibulaGameMode::EquipStartingEquipment(AFibulaCharacter *Character)
{
	if (!Character)
	{
		return;
	}

	const TArray<FGameItem> StartingInventory = Character->GetInventory();
	for (const FGameItem &Item : StartingInventory)
	{
		if (Item.ItemType == EItemType::Equipment)
		{
			Character->EquipItem(Item);
		}
	}
}
bool AFibulaGameMode::SpawnBot()
{
	if (!PlayerPawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn arena bot: character pawn class is unavailable."));
		return false;
	}

	EVocation Vocation = AFibulaBotController::ChooseRandomVocation(BotRandomStream);
#if !UE_BUILD_SHIPPING
	if (IsAutomationBotTestMode())
	{
		static const EVocation TestVocations[] = { EVocation::Knight, EVocation::Paladin, EVocation::Sorcerer, EVocation::Druid };
		if (AutomationTestBotSpawnCount < UE_ARRAY_COUNT(TestVocations))
		{
			Vocation = TestVocations[AutomationTestBotSpawnCount];
		}
		++AutomationTestBotSpawnCount;
	}
#endif
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APawn *SpawnedPawn = GetWorld()->SpawnActor<APawn>(PlayerPawnClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	AFibulaCharacter *BotCharacter = Cast<AFibulaCharacter>(SpawnedPawn);
	if (!BotCharacter)
	{
		if (SpawnedPawn)
		{
			SpawnedPawn->Destroy();
		}
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn arena bot: configured character class is not AFibulaCharacter."));
		return false;
	}

	if (AController *SpawnedController = BotCharacter->GetController())
	{
		SpawnedController->UnPossess();
		SpawnedController->Destroy();
	}

	BotCharacter->SetPersistenceSuppressed(true);
	BotCharacter->SetIsBot(true);
	BotCharacter->SetCharacterName(FString::Printf(TEXT("Arena Bot %03d"), NextBotSerial++));
	AssignTeam(BotCharacter);
	BotCharacter->SetVocation(Vocation);
	BotCharacter->SetIsTeamBattle(CurrentGameMode == EGameModeType::TeamBattle);

	AActor *StartSpot = FindTeamPlayerStart(BotCharacter->GetTeamId());
	if (!StartSpot)
	{
		StartSpot = FindPlayerStart(nullptr);
	}
	if (!StartSpot)
	{
		BotCharacter->Destroy();
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn arena bot: no PlayerStart was found."));
		return false;
	}
	BotCharacter->SetActorLocationAndRotation(
		StartSpot->GetActorLocation(),
		StartSpot->GetActorRotation(),
		false,
		nullptr,
		ETeleportType::TeleportPhysics);
	BotCharacter->InitializeCharacterStats();
	EquipStartingEquipment(BotCharacter);

	// Spawn actors are often grouped tightly around a team start. Spread bots into
	// nearby clear space so they do not block one another (or the joining players)
	// before their movement controllers have a chance to leave the protection zone.
	FVector BotSpawnLocation = StartSpot->GetActorLocation();
	bool bFoundUnoccupiedLocation = false;
	for (float Radius = 400.0f; Radius <= 1600.0f && !bFoundUnoccupiedLocation; Radius += 300.0f)
	{
		const float StartAngle = BotRandomStream.FRandRange(0.0f, 2.0f * PI);
		for (int32 PointIndex = 0; PointIndex < 12; ++PointIndex)
		{
			const float Angle = StartAngle + (2.0f * PI * PointIndex / 12.0f);
			FVector CandidateLocation = StartSpot->GetActorLocation() + FVector(
				FMath::Cos(Angle) * Radius,
				FMath::Sin(Angle) * Radius,
				0.0f);

			bool bHasNearbyCharacter = false;
			for (TActorIterator<AFibulaCharacter> CharacterIt(GetWorld()); CharacterIt; ++CharacterIt)
			{
				if (*CharacterIt != BotCharacter &&
					FVector::DistSquared2D((*CharacterIt)->GetActorLocation(), CandidateLocation) < FMath::Square(450.0f) &&
					FMath::Abs((*CharacterIt)->GetActorLocation().Z - CandidateLocation.Z) < 250.0f)
				{
					bHasNearbyCharacter = true;
					break;
				}
			}
			if (bHasNearbyCharacter || !GetWorld()->FindTeleportSpot(BotCharacter, CandidateLocation, StartSpot->GetActorRotation()))
			{
				continue;
			}

			BotSpawnLocation = CandidateLocation;
			bFoundUnoccupiedLocation = true;
			break;
		}
	}

	if (bFoundUnoccupiedLocation)
	{
		BotCharacter->SetActorLocationAndRotation(
			BotSpawnLocation,
			StartSpot->GetActorRotation(),
			false,
			nullptr,
			ETeleportType::TeleportPhysics);
	}

	AFibulaBotController *BotController = GetWorld()->SpawnActor<AFibulaBotController>(
		AFibulaBotController::StaticClass(),
		BotCharacter->GetActorLocation(),
		BotCharacter->GetActorRotation());
	if (!BotController)
	{
		BotCharacter->Destroy();
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn AI controller for arena bot."));
		return false;
	}

	BotController->InitializeBot(BotRandomStream.RandRange(1, 1000000000));
	BotController->Possess(BotCharacter);
	UE_LOG(LogTemp, Log, TEXT("Spawned %s (%s) on team %d."),
		*BotCharacter->GetCharacterName(),
		*BotCharacter->GetVocationAsString(),
		BotCharacter->GetTeamId());
	return true;
}

#if !UE_BUILD_SHIPPING
void AFibulaGameMode::RecordAutomationBotDamage(int32 DamageAmount)
{
	if (IsAutomationBotTestMode() && DamageAmount > 0)
	{
		AutomationBotDamage += DamageAmount;
	}
}

bool AFibulaGameMode::IsAutomationBotTestMode() const
{
	return FParse::Param(FCommandLine::Get(), TEXT("FibulaBotTest"));
}

void AFibulaGameMode::InitializeAutomationTestPlayer(APlayerController *NewPlayer)
{
	if (!NewPlayer || !PlayerPawnClass)
	{
		return;
	}

	if (NewPlayer->GetPawn())
	{
		NewPlayer->GetPawn()->Destroy();
	}
	AActor *StartSpot = FindPlayerStart(NewPlayer);
	if (!StartSpot)
	{
		KickPlayer(NewPlayer, TEXT("Bot test could not find a PlayerStart."));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APawn *SpawnedPawn = GetWorld()->SpawnActor<APawn>(
		PlayerPawnClass,
		StartSpot->GetActorLocation(),
		StartSpot->GetActorRotation(),
		SpawnParams);
	AFibulaCharacter *Character = Cast<AFibulaCharacter>(SpawnedPawn);
	if (!Character)
	{
		KickPlayer(NewPlayer, TEXT("Bot test could not create a test character."));
		return;
	}

	Character->SetPersistenceSuppressed(true);
	NewPlayer->Possess(Character);
	const FString TestPlayerName = FString::Printf(TEXT("BotTestPlayer%02d"), ++AutomationTestPlayerCount);
	InitializePlayerCharacter(NewPlayer, TestPlayerName, TEXT("Knight"));
	EquipStartingEquipment(Character);
	if (AFibulaPlayerState *PlayerState = NewPlayer->GetPlayerState<AFibulaPlayerState>())
	{
		PlayerState->SetPlayerName(TestPlayerName);
		PlayerState->SetPlayerVocation(TEXT("Knight"));
	}
	UE_LOG(LogTemp, Display, TEXT("FIBULA_BOT_TEST_JOINED name=%s team=%d"),
		*TestPlayerName,
		Character->GetTeamId());
}

void AFibulaGameMode::StageAutomationBotTestPlayers()
{
	if (!IsAutomationBotTestMode() || GetHumanPlayerCount() < 2)
	{
		return;
	}

	// Restrict this metric to bot damage after both test players are staged.
	AutomationBotDamage = 0;

	for (TActorIterator<AFibulaCharacter> HumanIt(GetWorld()); HumanIt; ++HumanIt)
	{
		AFibulaCharacter *Human = *HumanIt;
		if (!Human || Human->IsBot())
		{
			continue;
		}

		AFibulaCharacter *EnemyBot = nullptr;
		float BestScore = TNumericLimits<float>::Max();
		for (TActorIterator<AFibulaCharacter> BotIt(GetWorld()); BotIt; ++BotIt)
		{
			AFibulaCharacter *Candidate = *BotIt;
			if (!Candidate || !Candidate->IsBot() || Candidate->GetCurrentHealth() <= 0 || !CanDamage(Human, Candidate))
			{
				continue;
			}

			const float Distance = FVector::Distance(Human->GetActorLocation(), Candidate->GetActorLocation());
			const float ProtectionPenalty = Candidate->IsInProtectionZone() ? 50000.0f : 0.0f;
			const float Score = Distance + ProtectionPenalty;
			if (Score < BestScore)
			{
				BestScore = Score;
				EnemyBot = Candidate;
			}
		}

		if (!EnemyBot)
		{
			continue;
		}

		const FVector AwayFromBot = (Human->GetActorLocation() - EnemyBot->GetActorLocation()).GetSafeNormal2D();
		const float BaseAngle = AwayFromBot.IsNearlyZero() ? 0.0f : AwayFromBot.Rotation().Yaw;
		FVector StagingLocation = Human->GetActorLocation();
		bool bFoundStagingLocation = false;
		for (float Radius = 800.0f; Radius <= 2000.0f && !bFoundStagingLocation; Radius += 300.0f)
		{
			for (int32 PointIndex = 0; PointIndex < 16; ++PointIndex)
			{
				const float Angle = FMath::DegreesToRadians(BaseAngle) + (2.0f * PI * PointIndex / 16.0f);
				FVector CandidateLocation = EnemyBot->GetActorLocation() + FVector(
					FMath::Cos(Angle) * Radius,
					FMath::Sin(Angle) * Radius,
					0.0f);

				bool bHasNearbyCharacter = false;
				for (TActorIterator<AFibulaCharacter> CharacterIt(GetWorld()); CharacterIt; ++CharacterIt)
				{
					if (*CharacterIt != Human &&
						FVector::DistSquared2D((*CharacterIt)->GetActorLocation(), CandidateLocation) < FMath::Square(500.0f) &&
						FMath::Abs((*CharacterIt)->GetActorLocation().Z - CandidateLocation.Z) < 250.0f)
					{
						bHasNearbyCharacter = true;
						break;
					}
				}
				if (bHasNearbyCharacter || !GetWorld()->FindTeleportSpot(Human, CandidateLocation, Human->GetActorRotation()))
				{
					continue;
				}

				StagingLocation = CandidateLocation;
				bFoundStagingLocation = true;
				break;
			}
		}

		if (bFoundStagingLocation)
		{
			Human->SetActorLocationAndRotation(
				StagingLocation,
				(EnemyBot->GetActorLocation() - StagingLocation).Rotation(),
				false,
				nullptr,
				ETeleportType::TeleportPhysics);
			Human->SetInProtectionZone(false);
			UE_LOG(LogTemp, Display,
				TEXT("FIBULA_BOT_TEST_STAGED human=%s target=%s distance=%.0f location=%s"),
				*Human->GetCharacterName(),
				*EnemyBot->GetCharacterName(),
				FVector::Distance(Human->GetActorLocation(), EnemyBot->GetActorLocation()),
				*Human->GetActorLocation().ToCompactString());
		}
	}
}

void AFibulaGameMode::WriteAutomationBotTestResult()
{
	int32 TargetAcquisitions = 0;
	int32 MovementOrders = 0;
	int32 MovementProgress = 0;
	int32 CombatOrders = 0;
	int32 DamageObserved = 0;
	int32 ProtectedBots = 0;
	int32 ProtectedHumans = 0;
	int32 ActiveTargets = 0;
	int32 VocationCounts[4] = {};

	for (TActorIterator<AFibulaBotController> It(GetWorld()); It; ++It)
	{
		TargetAcquisitions += It->GetTargetAcquisitionCount();
		MovementOrders += It->GetMovementOrderCount();
		MovementProgress += It->GetMovementProgressCount();
		CombatOrders += It->GetCombatOrderCount();
		DamageObserved += It->GetDamageObservedCount();

		const AFibulaCharacter *Bot = Cast<AFibulaCharacter>(It->GetPawn());
		if (Bot)
		{
			ProtectedBots += Bot->IsInProtectionZone() ? 1 : 0;
			const int32 VocationIndex = static_cast<int32>(Bot->GetVocation()) - 1;
			if (VocationIndex >= 0 && VocationIndex < UE_ARRAY_COUNT(VocationCounts))
			{
				++VocationCounts[VocationIndex];
			}

			const AFibulaCharacter *Target = It->GetTargetCharacter();
			const float TargetDistance = Target
				? FVector::Distance(Bot->GetActorLocation(), Target->GetActorLocation())
				: -1.0f;
			ActiveTargets += Target ? 1 : 0;
			UE_LOG(LogTemp, Display,
				TEXT("FIBULA_BOT_TEST_BOT name=%s team=%d protected=%d target=%s targetProtected=%d distance=%.0f location=%s"),
				*Bot->GetCharacterName(),
				Bot->GetTeamId(),
				Bot->IsInProtectionZone(),
				Target ? *Target->GetCharacterName() : TEXT("none"),
				Target ? Target->IsInProtectionZone() : false,
				TargetDistance,
				*Bot->GetActorLocation().ToCompactString());
		}
	}

	for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
	{
		if (!It->IsBot())
		{
			ProtectedHumans += It->IsInProtectionZone() ? 1 : 0;
			UE_LOG(LogTemp, Display,
				TEXT("FIBULA_BOT_TEST_HUMAN name=%s team=%d protected=%d location=%s velocity=%s"),
				*It->GetCharacterName(),
				It->GetTeamId(),
				It->IsInProtectionZone(),
				*It->GetActorLocation().ToCompactString(),
				*It->GetVelocity().ToCompactString());
		}
	}

	const bool bAllVocationsPresent =
		VocationCounts[0] > 0 && VocationCounts[1] > 0 &&
		VocationCounts[2] > 0 && VocationCounts[3] > 0;
	const bool bPassed = GetHumanPlayerCount() >= 2 &&
		bAllVocationsPresent &&
		GetActiveBotCount() > 0 &&
		TargetAcquisitions > 0 &&
		MovementOrders > 0 &&
		MovementProgress > 0 &&
		CombatOrders > 0 &&
		DamageObserved > 0 &&
		AutomationBotDamage > 0;

	UE_LOG(LogTemp, Display,
		TEXT("FIBULA_BOT_TEST_RESULT %s Bots=%d Vocations=K%d,P%d,S%d,D%d Targets=%d ActiveTargets=%d ProtectedBots=%d ProtectedHumans=%d Movement=%d MovementProgress=%d Combat=%d DamageObserved=%d BotDamage=%d Humans=%d Seed=%d"),
		bPassed ? TEXT("PASS") : TEXT("FAIL"),
		GetActiveBotCount(),
		VocationCounts[0], VocationCounts[1], VocationCounts[2], VocationCounts[3],
		TargetAcquisitions,
		ActiveTargets,
		ProtectedBots,
		ProtectedHumans,
		MovementOrders,
		MovementProgress,
		CombatOrders,
		DamageObserved,
		AutomationBotDamage,
		GetHumanPlayerCount(),
		BotRandomSeed);
}
#endif
