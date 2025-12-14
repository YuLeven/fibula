#include "RewardSystem.h"
#include "ItemDatabase.h"
#include "FibulaCharacter.h"

const TMap<EItemRarity, float> URewardSystem::RarityWeights = {
    {EItemRarity::Common, 50.0f},
    {EItemRarity::Uncommon, 25.0f},
    {EItemRarity::Rare, 15.0f},
    {EItemRarity::Epic, 5.0f},
    {EItemRarity::Legendary, 1.0f},
    {EItemRarity::Artifact, 0.001f}};

const TMap<EItemRarity, TPair<int32, int32>> URewardSystem::RarityStackRanges = {
    {EItemRarity::Common, TPair<int32, int32>(5, 100)},
    {EItemRarity::Uncommon, TPair<int32, int32>(1, 8)},
    {EItemRarity::Rare, TPair<int32, int32>(1, 2)},
    {EItemRarity::Epic, TPair<int32, int32>(1, 2)},
    {EItemRarity::Legendary, TPair<int32, int32>(1, 1)},
    {EItemRarity::Artifact, TPair<int32, int32>(1, 1)}};

EItemRarity URewardSystem::GetRandomRarity()
{
    float TotalWeight = 0.0f;
    for (const auto &Pair : RarityWeights)
    {
        TotalWeight += Pair.Value;
    }

    float RandomValue = FMath::FRand() * TotalWeight;
    float CurrentWeight = 0.0f;

    for (const auto &Pair : RarityWeights)
    {
        CurrentWeight += Pair.Value;
        if (RandomValue <= CurrentWeight)
        {
            return Pair.Key;
        }
    }

    return EItemRarity::Common;
}

TArray<FGameItem> URewardSystem::GetItemsByRarity(EItemRarity Rarity)
{
    TArray<FGameItem> ItemsOfRarity;

    const TArray<FGameItem> *RarityItems = UItemDatabase::GetItemsByRarity(Rarity);
    if (RarityItems)
    {
        
        for (const FGameItem &Item : *RarityItems)
        {
            if (Item.Name != TEXT("Reward Present"))
            {
                ItemsOfRarity.Add(Item);
            }
        }
    }

    return ItemsOfRarity;
}

FGameItem URewardSystem::GenerateReward()
{
    
    EItemRarity SelectedRarity = GetRandomRarity();

    
    TArray<FGameItem> PossibleItems = GetItemsByRarity(SelectedRarity);

    
    if (PossibleItems.Num() == 0)
    {
        SelectedRarity = EItemRarity::Common;
        PossibleItems = GetItemsByRarity(SelectedRarity);
    }

    
    if (PossibleItems.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, PossibleItems.Num() - 1);
        FGameItem SelectedItem = PossibleItems[RandomIndex];

        
        if (SelectedItem.bIsStackable)
        {
            const TPair<int32, int32> *StackRange = RarityStackRanges.Find(SelectedRarity);
            if (StackRange)
            {
                SelectedItem.StackCount = FMath::RandRange(StackRange->Key, StackRange->Value);
            }
        }

        return SelectedItem;
    }

    
    return FGameItem();
}

bool URewardSystem::ProcessRewardPresent(AFibulaCharacter *Character)
{
    if (!Character)
    {
        return false;
    }

    
    FGameItem Reward = GenerateReward();
    if (Reward.Name.IsEmpty())
    {
        return false;
    }

    
    Character->RemoveItem(TEXT("Reward Present"));
    Character->AddItem(Reward);

    return true;
}