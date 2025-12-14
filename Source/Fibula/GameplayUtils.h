#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FibulaCharacter.h"
#include "GameplayUtils.generated.h"

UCLASS()
class FIBULA_API UGameplayUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    static TArray<AFibulaCharacter *> GetPlayersWithinRadius(const UObject *WorldContextObject, const FVector &Center, float Radius = 3000.0f);
};