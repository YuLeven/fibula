#include "LootWidget.h"
#include "ItemContainer.h"
#include "GameSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/SizeBox.h"
#include "FibulaHUD.h"

ULootWidget::ULootWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UGameSlot> GameSlotBPClass(TEXT("/Game/HUDs/Widgets/WBP_GameSlot"));
    if (GameSlotBPClass.Class != nullptr)
    {
        GameSlotClass = GameSlotBPClass.Class;
    }
}

void ULootWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseButton)
    {
        CloseButton->OnClicked.RemoveDynamic(this, &ULootWidget::OnCloseButtonClicked);
        CloseButton->OnClicked.AddDynamic(this, &ULootWidget::OnCloseButtonClicked);
    }

    if (!GameSlotClass)
    {
        UE_LOG(LogTemp, Error, TEXT("LootWidget: GameSlotClass is null in NativeConstruct"));
        return;
    }

    InitializeLootGrid();

    
    if (SourceContainer)
    {
        UpdateLootDisplay(SourceContainer->GetInventory());
    }

    
    if (UWorld *World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            DistanceCheckTimer,
            this,
            &ULootWidget::CheckDistance,
            CHECK_DISTANCE_INTERVAL,
            true);
    }
}

void ULootWidget::NativeDestruct()
{
    if (CloseButton)
    {
        CloseButton->OnClicked.RemoveDynamic(this, &ULootWidget::OnCloseButtonClicked);
    }

    if (UWorld *World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(DistanceCheckTimer);
    }

    Super::NativeDestruct();
}

void ULootWidget::CheckDistance()
{
    if (!SourceContainer || !IsValid(SourceContainer))
    {
        RemoveFromParent();
        return;
    }

    APlayerController *PC = GetOwningPlayer();
    APawn *PlayerPawn = PC ? PC->GetPawn() : nullptr;

    if (!PlayerPawn)
    {
        return;
    }

    float Distance = FVector::Distance(PlayerPawn->GetActorLocation(), SourceContainer->GetActorLocation());

    if (Distance > MAX_LOOT_DISTANCE)
    {
        if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
        {
            HUD->HideLootWindow();
        }
    }
}

void ULootWidget::InitializeLootGrid()
{
    if (!LootGrid || !GameSlotClass)
    {
        UE_LOG(LogTemp, Error, TEXT("LootWidget: LootGrid or GameSlotClass is null"));
        return;
    }

    
    LootGrid->ClearChildren();
    LootSlots.Empty();

    
    LootGrid->SetMinDesiredSlotWidth(SLOT_SIZE);
    LootGrid->SetMinDesiredSlotHeight(SLOT_SIZE);

    
    for (int32 Row = 0; Row < LOOT_ROWS; Row++)
    {
        for (int32 Col = 0; Col < LOOT_COLS; Col++)
        {
            UGameSlot *NewSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
            if (!NewSlot)
            {
                UE_LOG(LogTemp, Error, TEXT("LootWidget: Failed to create slot at [%d,%d]"), Row, Col);
                continue;
            }

            
            USizeBox *SizeBoxWidget = NewObject<USizeBox>(this);
            SizeBoxWidget->SetWidthOverride(SLOT_SIZE);
            SizeBoxWidget->SetHeightOverride(SLOT_SIZE);
            SizeBoxWidget->AddChild(NewSlot);

            
            LootGrid->AddChildToUniformGrid(SizeBoxWidget, Row, Col);
            LootSlots.Add(NewSlot);

            NewSlot->SetSlotPlacement(ESlotPlacement::Loot);
        }
    }
}

void ULootWidget::UpdateLootDisplay(const TArray<FGameItem> &Items)
{
    
    for (UGameSlot *LootSlot : LootSlots)
    {
        if (LootSlot)
        {
            LootSlot->SetEmpty();
        }
    }

    
    for (int32 i = 0; i < Items.Num() && i < LootSlots.Num(); i++)
    {
        if (LootSlots[i])
        {
            LootSlots[i]->SetItem(Items[i]);
        }
    }
}

void ULootWidget::SetSourceContainer(AItemContainer *Container)
{
    SourceContainer = Container;

    
    if (SourceContainer && LootSlots.Num() > 0)
    {
        UpdateLootDisplay(SourceContainer->GetInventory());
    }
}

void ULootWidget::OnCloseButtonClicked()
{
    if (APlayerController *PC = GetOwningPlayer())
    {
        if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
        {
            HUD->HideLootWindow();
        }
    }
}