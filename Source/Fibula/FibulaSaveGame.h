#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FibulaSaveGame.generated.h"

UCLASS()
class FIBULA_API UFibulaSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UFibulaSaveGame();

    UPROPERTY()
    FString AuthToken;
};