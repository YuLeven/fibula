#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameItem.h"
#include "ItemContainer.generated.h"

UCLASS(Abstract)
class FIBULA_API AItemContainer : public AActor
{
    GENERATED_BODY()

public:
    AItemContainer();

    void SetInventory(const TArray<FGameItem> &NewInventory);
    const TArray<FGameItem> &GetInventory() const { return Inventory; }
    bool RemoveItem(const FString &ItemName, int32 Count = 1);
    bool AddItem(const FGameItem &Item);
    bool EmptyInventory();

protected:
    UPROPERTY(ReplicatedUsing = OnRep_Inventory)
    TArray<FGameItem> Inventory;

    UFUNCTION(Server, Reliable)
    void ServerRemoveItem(const FString &ItemName, int32 Count);

    UFUNCTION()
    void OnRep_Inventory();

    UPROPERTY(VisibleAnywhere)
    class UBoxComponent *TargetingBox;
};