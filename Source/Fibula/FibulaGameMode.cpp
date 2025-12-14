

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
		if (GameSession && ServerStatusReporter)
		{
			int32 Port = GetWorld()->URL.Port;

			ServerStatusReporter->Initialize(Port, GetMaxPlayers());
			ServerStatusReporter->StartReporting();
		}
	}
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

void AFibulaGameMode::PostLogin(APlayerController *NewPlayer)
{
	Super::PostLogin(NewPlayer);
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
		if (Character && Character->GetController())
		{
			PersistCharacterItems(Character);
		}
	}
}

void AFibulaGameMode::PersistCharacterItems(AFibulaCharacter *Character)
{
	if (!Character)
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
