#include "ItemContainer.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "FibulaHUD.h"

AItemContainer::AItemContainer()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    TargetingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TargetingBox"));
    RootComponent = TargetingBox;
    TargetingBox->SetBoxExtent(FVector(75.0f, 75.0f, 37.5f));
    TargetingBox->SetCollisionObjectType(ECC_GameTraceChannel1);
    TargetingBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TargetingBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TargetingBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void AItemContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AItemContainer, Inventory);
}

void AItemContainer::SetInventory(const TArray<FGameItem> &NewInventory)
{
    if (HasAuthority())
    {
        Inventory = NewInventory;
    }
}

bool AItemContainer::RemoveItem(const FString &ItemName, int32 Count)
{
    if (!HasAuthority())
    {
        ServerRemoveItem(ItemName, Count);
        return true;
    }

    for (int32 i = 0; i < Inventory.Num(); i++)
    {
        if (Inventory[i].Name == ItemName)
        {
            if (Inventory[i].bIsStackable)
            {
                if (Inventory[i].StackCount > Count)
                {
                    Inventory[i].StackCount -= Count;
                    return true;
                }
                else if (Inventory[i].StackCount == Count)
                {
                    Inventory.RemoveAt(i);
                    return true;
                }
            }
            else if (Count == 1)
            {
                Inventory.RemoveAt(i);
                return true;
            }
        }
    }
    return false;
}

bool AItemContainer::AddItem(const FGameItem &Item)
{
    if (!HasAuthority())
    {
        return false;
    }

    
    if (Item.bIsStackable)
    {
        for (FGameItem &ExistingItem : Inventory)
        {
            if (ExistingItem.Name == Item.Name)
            {
                ExistingItem.StackCount += Item.StackCount;
                return true;
            }
        }
    }

    
    Inventory.Add(Item);
    return true;
}

void AItemContainer::ServerRemoveItem_Implementation(const FString &ItemName, int32 Count)
{
    RemoveItem(ItemName, Count);
}

void AItemContainer::OnRep_Inventory()
{
    
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController *PC = It->Get())
        {
            if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
            {
                if (ULootWidget *LootWidget = HUD->GetLootWidget())
                {
                    if (LootWidget->GetSourceContainer() == this)
                    {
                        LootWidget->UpdateLootDisplay(Inventory);
                    }
                }
            }
        }
    }
}

bool AItemContainer::EmptyInventory()
{
    if (!HasAuthority())
        return false;
    Inventory.Empty();
    return true;
}
