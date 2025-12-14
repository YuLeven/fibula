#pragma once

#include "CoreMinimal.h"
#include "GameItem.h"
#include "ItemDatabase.generated.h"

UCLASS()
class FIBULA_API UItemDatabase : public UObject
{
    GENERATED_BODY()

private:
    static TMap<FString, FGameItem> Items;
    static TMap<EItemRarity, TArray<FGameItem>> ItemsByRarity;
    static void RegisterRunes();
    static void RegisterConsumables();
    static void RegisterEquipment();

public:
    static void RegisterItem(const FGameItem &Item);
    static const FGameItem *GetItem(const FString &ItemName);
    static void Initialize();
    static const TArray<FGameItem> *GetItemsByRarity(EItemRarity Rarity);
};