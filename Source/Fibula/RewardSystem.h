#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameItem.h"
#include "RewardSystem.generated.h"

UCLASS()
class FIBULA_API URewardSystem : public UObject
{
    GENERATED_BODY()

public:
    
    static FGameItem GenerateReward();

    
    static bool ProcessRewardPresent(class AFibulaCharacter *Character);

private:
    
    static EItemRarity GetRandomRarity();

    
    static TArray<FGameItem> GetItemsByRarity(EItemRarity Rarity);

    
    static const TMap<EItemRarity, float> RarityWeights;

    
    static const TMap<EItemRarity, TPair<int32, int32>> RarityStackRanges;
};