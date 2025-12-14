#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuthTokenManager.generated.h"

UCLASS()
class FIBULA_API UAuthTokenManager : public UObject
{
    GENERATED_BODY()

public:
    static void StoreToken(const FString &Token);
    static FString GetToken();
    static void ClearToken();
    static bool HasToken();
    static void StoreLastPlayedCharacter(const FString &CharacterName);
    static FString GetLastPlayedCharacter();
    static void ClearLastPlayedCharacter();

private:
    static const FString SaveSlotName;
    static const int32 UserIndex;
    static FString CurrentToken;
    static FString LastPlayedCharacter;

    static void SaveTokenToFile();
    static void LoadTokenFromFile();
};