#include "AuthTokenManager.h"
#include "FibulaSaveGame.h"
#include "Kismet/GameplayStatics.h"

FString UAuthTokenManager::CurrentToken;
const FString UAuthTokenManager::SaveSlotName = TEXT("AuthData");
const int32 UAuthTokenManager::UserIndex = 0;
FString UAuthTokenManager::LastPlayedCharacter;

void UAuthTokenManager::StoreToken(const FString &Token)
{
    CurrentToken = Token;
    SaveTokenToFile();
}

FString UAuthTokenManager::GetToken()
{
    if (CurrentToken.IsEmpty())
    {
        LoadTokenFromFile();
    }
    return CurrentToken;
}

void UAuthTokenManager::ClearToken()
{
    CurrentToken.Empty();
    UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);
}

bool UAuthTokenManager::HasToken()
{
    if (CurrentToken.IsEmpty())
    {
        LoadTokenFromFile();
    }
    return !CurrentToken.IsEmpty();
}

void UAuthTokenManager::SaveTokenToFile()
{
    if (UFibulaSaveGame *SaveGameInstance = Cast<UFibulaSaveGame>(UGameplayStatics::CreateSaveGameObject(UFibulaSaveGame::StaticClass())))
    {
        SaveGameInstance->AuthToken = CurrentToken;
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
    }
}

void UAuthTokenManager::LoadTokenFromFile()
{
    if (UFibulaSaveGame *SaveGameInstance = Cast<UFibulaSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex)))
    {
        CurrentToken = SaveGameInstance->AuthToken;
    }
}

void UAuthTokenManager::StoreLastPlayedCharacter(const FString &CharacterName)
{
    LastPlayedCharacter = CharacterName;
}

FString UAuthTokenManager::GetLastPlayedCharacter()
{
    return LastPlayedCharacter;
}

void UAuthTokenManager::ClearLastPlayedCharacter()
{
    LastPlayedCharacter.Empty();
}