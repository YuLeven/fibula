#include "ServerStatusReporter.h"
#include "FibulaGameState.h"
#include "FibulaPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "FibulaCharacter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Config/ServerConfig.h"
#include "Misc/Base64.h"
#include "FibulaGameMode.h"
#include "GameModeType.h"

UServerStatusReporter::UServerStatusReporter()
    : ServerPort(0), MaxPlayers(0), bIsReporting(false)
{
}

void UServerStatusReporter::Initialize(int32 InPort, int32 InMaxPlayers)
{
    ServerPort = InPort;
    MaxPlayers = InMaxPlayers;
}

void UServerStatusReporter::StartReporting()
{
    if (!bIsReporting)
    {
        bIsReporting = true;
        GetWorld()->GetTimerManager().SetTimer(ReportingTimerHandle, this, &UServerStatusReporter::ReportStatus, 30.0f, true, 0.0f);
        GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &UServerStatusReporter::ReportCharacterStats, 60.0f, true, 0.0f);
    }
}

void UServerStatusReporter::StopReporting()
{
    if (bIsReporting)
    {
        bIsReporting = false;
        GetWorld()->GetTimerManager().ClearTimer(ReportingTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(StatsTimerHandle);
    }
}

void UServerStatusReporter::ReportCharacterStats()
{
    if (UWorld *World = GetWorld())
    {
        if (AFibulaGameState *GameState = Cast<AFibulaGameState>(World->GetGameState()))
        {
            TMap<FString, FCharacterStats> Stats = GameState->GetCharactersStats();
            TArray<TSharedPtr<FJsonValue>> CharactersArray;
            for (const auto &Pair : Stats)
            {
                TSharedPtr<FJsonObject> CharacterObject = MakeShared<FJsonObject>();
                CharacterObject->SetStringField("character_name", Pair.Key);
                CharacterObject->SetNumberField("kills", Pair.Value.AccumulatedKills);
                CharacterObject->SetNumberField("match_kills", Pair.Value.MatchKills);
                CharacterObject->SetNumberField("deaths", Pair.Value.AccumulatedDeaths);
                CharacterObject->SetNumberField("match_deaths", Pair.Value.MatchDeaths);
                CharacterObject->SetNumberField("level", Pair.Value.Level);
                CharactersArray.Add(MakeShared<FJsonValueObject>(CharacterObject));
            }

            if (CharactersArray.Num() > 0)
            {
                TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
                JsonObject->SetArrayField("characters", CharactersArray);

                FString JsonString;
                TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
                FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

                TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
                HttpRequest->SetURL(FString::Printf(TEXT("http://%s/backend/characters/stats"), *ServerConfig::GetBackendServer()));
                HttpRequest->SetVerb("POST");
                HttpRequest->SetHeader("Content-Type", "application/json");

                // Add Basic Auth header
                FString AuthHeader = FString::Printf(TEXT("%s:%s"), *ServerConfig::GetServerUsername(), *ServerConfig::GetServerPassword());
                FString EncodedAuth = FBase64::Encode(AuthHeader);
                HttpRequest->SetHeader("Authorization", FString::Printf(TEXT("Basic %s"), *EncodedAuth));

                HttpRequest->SetContentAsString(JsonString);
                HttpRequest->ProcessRequest();
            }

            GameState->ResetCharactersStats();
        }
    }
}

FServerStatus UServerStatusReporter::GatherServerStatus()
{
    FServerStatus Status;
    Status.Port = ServerPort;
    Status.MaxPlayers = MaxPlayers;

    if (UWorld *World = GetWorld())
    {
        TArray<APlayerState *> PlayerStates = World->GetGameState()->PlayerArray;
        Status.OnlinePlayers = PlayerStates.Num();

        for (APlayerState *PS : PlayerStates)
        {
            if (AFibulaPlayerState *FPS = Cast<AFibulaPlayerState>(PS))
            {
                if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(FPS->GetPawn()))
                {
                    FPlayerInfo PlayerInfo;
                    PlayerInfo.Name = Character->GetCharacterName();
                    PlayerInfo.Level = Character->GetCharacterLevel();
                    PlayerInfo.Vocation = StaticEnum<EVocation>()->GetNameStringByValue((int64)Character->GetVocation());
                    Status.Players.Add(PlayerInfo);
                }
            }
        }
    }

    return Status;
}

void UServerStatusReporter::ReportStatus()
{
    FServerStatus Status = GatherServerStatus();

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetNumberField("port", Status.Port);
    JsonObject->SetStringField("address", FString::Printf(TEXT("%s"), *ServerConfig::GetGameServerAddress()));
    JsonObject->SetNumberField("max_players", Status.MaxPlayers);
    JsonObject->SetNumberField("players_online", Status.OnlinePlayers);
    JsonObject->SetStringField("version", ServerConfig::GAME_VERSION);

    if (AFibulaGameMode *GameMode = Cast<AFibulaGameMode>(GetWorld()->GetAuthGameMode()))
    {
        FString GameModeStr = StaticEnum<EGameModeType>()->GetNameStringByValue((int64)GameMode->GetCurrentGameMode());
        JsonObject->SetStringField("game_mode", GameModeStr);
    }

    TArray<TSharedPtr<FJsonValue>> PlayersArray;
    for (const FPlayerInfo &Player : Status.Players)
    {
        TSharedPtr<FJsonObject> PlayerObject = MakeShared<FJsonObject>();
        PlayerObject->SetStringField("name", Player.Name);
        PlayerObject->SetStringField("vocation", Player.Vocation);
        PlayerObject->SetNumberField("level", Player.Level);
        PlayersArray.Add(MakeShared<FJsonValueObject>(PlayerObject));
    }
    JsonObject->SetArrayField("players", PlayersArray);

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(FString::Printf(TEXT("http://%s/backend/server/status"), *ServerConfig::GetBackendServer()));
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader("Content-Type", "application/json");

    // Add Basic Auth header
    FString AuthHeader = FString::Printf(TEXT("%s:%s"), *ServerConfig::GetServerUsername(), *ServerConfig::GetServerPassword());
    FString EncodedAuth = FBase64::Encode(AuthHeader);
    HttpRequest->SetHeader("Authorization", FString::Printf(TEXT("Basic %s"), *EncodedAuth));

    HttpRequest->SetContentAsString(JsonString);
    HttpRequest->ProcessRequest();
}