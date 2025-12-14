#include "GameSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "FibulaGameState.h"
#include "SpellSystem.h"
#include "ItemDragDropOperation.h"
#include "FibulaHUD.h"
#include "SpellDatabase.h"
#include "ItemDatabase.h"
#include "FibulaCharacter.h"
#include "TooltipWidget.h"
#include "RewardSystem.h"
#include "StatusWidget.h"
UGameSlot::UGameSlot(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    bHasContent = false;
    SlotType = ESlotType::Item;
    SlotPlacement = ESlotPlacement::Inventory;
}

void UGameSlot::NativeConstruct()
{
    Super::NativeConstruct();
    SetIsFocusable(true);

    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }

    if (SlotButton)
    {
        SlotButton->SetVisibility(ESlateVisibility::Visible);
        SlotButton->SetIsEnabled(true);

        
        SlotButton->OnReleased.RemoveDynamic(this, &UGameSlot::OnSlotButtonClicked);

        
        if (SlotPlacement == ESlotPlacement::ActionBar)
        {
            SlotButton->OnReleased.AddDynamic(this, &UGameSlot::OnSlotButtonClicked);
        }

        
        if (SlotPlacement == ESlotPlacement::Inventory ||
            SlotPlacement == ESlotPlacement::Loot ||
            SlotPlacement == ESlotPlacement::Equipment ||
            SlotPlacement == ESlotPlacement::SpellList)
        {
            SlotButton->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
    }

    
    this->SetVisibility(ESlateVisibility::Visible);
}

void UGameSlot::NativeDestruct()
{
    GetWorld()->GetTimerManager().ClearTimer(TooltipTimerHandle);
    HideTooltip();

    if (SlotButton)
    {
        SlotButton->OnReleased.RemoveDynamic(this, &UGameSlot::OnSlotButtonClicked);
    }

    Super::NativeDestruct();
}

void UGameSlot::SetEmpty()
{
    CurrentItem = FGameItem();
    CurrentSpell = FSpellDefinition();
    bHasContent = false;

    if (ItemIcon)
    {
        if (DefaultIcon)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(DefaultIcon);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.Tiling = ESlateBrushTileType::NoTile;
            ItemIcon->SetBrush(Brush);
            ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f));
        }
        else
        {
            ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
        }
    }

    if (StackCountText)
    {
        StackCountText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UGameSlot::SetItem(const FGameItem &NewItem)
{
    CurrentItem = NewItem;
    bHasContent = true;
    SlotType = ESlotType::Item;

    if (ItemIcon)
    {
        UTexture2D *IconTexture = Cast<UTexture2D>(StaticLoadObject(
            UTexture2D::StaticClass(),
            nullptr,
            *NewItem.Icon.ToSoftObjectPath().ToString()));

        if (IconTexture)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(IconTexture);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.Tiling = ESlateBrushTileType::NoTile;
            ItemIcon->SetBrush(Brush);
            ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }

    if (StackCountText)
    {
        if (SlotPlacement != ESlotPlacement::ActionBar && NewItem.bIsStackable && NewItem.StackCount > 1)
        {
            StackCountText->SetText(FText::AsNumber(NewItem.StackCount));
            StackCountText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            StackCountText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UGameSlot::SetSpell(const FSpellDefinition &NewSpell)
{
    CurrentSpell = NewSpell;
    bHasContent = true;
    SlotType = ESlotType::Spell;

    if (ItemIcon)
    {
        UTexture2D *IconTexture = Cast<UTexture2D>(StaticLoadObject(
            UTexture2D::StaticClass(),
            nullptr,
            *NewSpell.IconPath));

        if (IconTexture)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(IconTexture);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.Tiling = ESlateBrushTileType::NoTile;
            ItemIcon->SetBrush(Brush);
            ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }

    if (StackCountText)
    {
        StackCountText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UGameSlot::NativeOnDragDetected(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent, UDragDropOperation *&OutOperation)
{
    if (!bHasContent)
        return;

    if (SlotPlacement != ESlotPlacement::Inventory &&
        SlotPlacement != ESlotPlacement::Loot &&
        SlotPlacement != ESlotPlacement::Equipment &&
        SlotPlacement != ESlotPlacement::SpellList &&
        !(SlotPlacement == ESlotPlacement::ActionBar && !IsActionBarLocked()))
        return;

    UItemDragDropOperation *DragDropOperation = NewObject<UItemDragDropOperation>();
    DragDropOperation->SourceSlot = this;
    DragDropOperation->DraggedItem = CurrentItem;
    DragDropOperation->DraggedSpell = CurrentSpell;

    UImage *DragVisual = NewObject<UImage>(this);
    DragVisual->SetBrushFromTexture(Cast<UTexture2D>(ItemIcon->Brush.GetResourceObject()));
    DragVisual->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.7f));

    DragDropOperation->DefaultDragVisual = DragVisual;
    DragDropOperation->Pivot = EDragPivot::CenterCenter;

    OutOperation = DragDropOperation;
    OnItemDragged.Broadcast(this);
}

bool UGameSlot::NativeOnDrop(const FGeometry &InGeometry, const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation)
{
    
    if (SlotButton)
    {
        SlotButton->WidgetStyle.Normal.OutlineSettings.Color = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    UItemDragDropOperation *ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
    if (!ItemDragDrop || !ItemDragDrop->SourceSlot)
        return false;

    
    if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::ActionBar)
    {
        if (SlotPlacement == ESlotPlacement::ActionBar)
            return HandleActionBarDrop(ItemDragDrop);
        else
            return HandleUnsetActionBarSlot(ItemDragDrop);
    }

    
    if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::SpellList)
    {
        if (SlotPlacement == ESlotPlacement::ActionBar)
            return HandleActionBarDrop(ItemDragDrop);
        return false;
    }

    
    UE_LOG(LogTemp, Log, TEXT("NativeOnDrop called for slot placement: %d"), static_cast<int32>(SlotPlacement));
    switch (SlotPlacement)
    {
    case ESlotPlacement::Inventory:
        if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Loot)
            return HandleLooting(ItemDragDrop);
        else if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Inventory)
            return HandleInventoryReorganization(ItemDragDrop);
        else if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Equipment)
            return HandleUnequipToInventory(ItemDragDrop);
        break;

    case ESlotPlacement::Loot:
        if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Inventory)
            return HandleMovingItemToContainer(ItemDragDrop);
        else if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Equipment)
            return HandleUnequipToContainer(ItemDragDrop);
        break;

    case ESlotPlacement::Equipment:
        return HandleEquipmentDrop(ItemDragDrop);

    case ESlotPlacement::ActionBar:
        if (ItemDragDrop->DraggedItem.ItemType == EItemType::Equipment)
            return false;
        return HandleActionBarDrop(ItemDragDrop);

    case ESlotPlacement::Garbage:
        UE_LOG(LogTemp, Log, TEXT("Garbage slot drop"));
        if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Inventory)
            return HandleThrowItemAway(ItemDragDrop);
        else if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::ActionBar)
            return HandleUnsetActionBarSlot(ItemDragDrop);
        break;
    }

    return false;
}

bool UGameSlot::HandleMovingItemToContainer(UItemDragDropOperation *ItemDragDrop)
{
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return false;

    APlayerController *PC = Cast<APlayerController>(Character->GetController());
    if (!PC)
        return false;

    AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD());
    if (!HUD)
        return false;

    ULootWidget *LootWidget = HUD->GetLootWidget();
    if (!LootWidget)
        return false;

    AItemContainer *Container = Cast<AItemContainer>(LootWidget->GetSourceContainer());
    if (!Container)
        return false;

    Character->ServerTransferItemToContainer(Container, ItemDragDrop->DraggedItem);
    return true;
}

bool UGameSlot::HandleLooting(UItemDragDropOperation *ItemDragDrop)
{
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return false;

    APlayerController *PC = Cast<APlayerController>(Character->GetController());
    if (!PC)
        return false;

    AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD());
    if (!HUD)
        return false;

    ULootWidget *LootWidget = HUD->GetLootWidget();
    if (!LootWidget)
        return false;

    AItemContainer *Container = Cast<AItemContainer>(LootWidget->GetSourceContainer());
    if (!Container)
        return false;

    Character->ServerTransferItemFromContainer(Container, ItemDragDrop->DraggedItem);
    return true;
}

bool UGameSlot::HandleInventoryReorganization(UItemDragDropOperation *ItemDragDrop)
{
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return false;

    Character->ServerSwapInventorySlots(
        ItemDragDrop->SourceSlot->GetItem(),
        bHasContent ? CurrentItem : FGameItem());
    return true;
}

bool UGameSlot::HandleActionBarDrop(UItemDragDropOperation *ItemDragDrop)
{
    
    FGameItem CurrentSlotItem = CurrentItem;
    FSpellDefinition CurrentSlotSpell = CurrentSpell;
    bool bCurrentHasContent = bHasContent;
    ESlotType CurrentSlotType = SlotType;

    UE_LOG(LogTemp, Log, TEXT("HandleActionBarDrop called"));

    
    if (ItemDragDrop->SourceSlot->GetSlotType() == ESlotType::Spell)
    {
        SetSpell(ItemDragDrop->DraggedSpell);
    }
    else
    {
        SetItem(ItemDragDrop->DraggedItem);
    }

    
    if (bCurrentHasContent && ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::ActionBar)
    {
        if (CurrentSlotType == ESlotType::Spell)
        {
            ItemDragDrop->SourceSlot->SetSpell(CurrentSlotSpell);
        }
        else
        {
            ItemDragDrop->SourceSlot->SetItem(CurrentSlotItem);
        }
    }
    
    else if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::ActionBar && !bCurrentHasContent)
    {
        ItemDragDrop->SourceSlot->SetEmpty();
    }

    return true;
}

void UGameSlot::ExecuteActionBarAction()
{
    if (!bHasContent)
        return;

    if (SlotPlacement != ESlotPlacement::ActionBar)
        return;

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return;

    if (SlotType == ESlotType::Item)
    {
        if (CurrentItem.Name == TEXT("Reward Present"))
        {
            Character->ServerOpenRewardPresent();
            return;
        }
        
        const FSpellDefinition *Spell = USpellDatabase::GetSpell(CurrentItem.Name);
        bool bIsTargeted = Spell && (Spell->SpellType == ESpellType::AreaRune || Spell->SpellType == ESpellType::MagicWall);

        if (bIsTargeted)
        {
            
            if (APlayerController *PC = Character->GetController<APlayerController>())
            {
                if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
                {
                    HUD->StartItemTargeting(this, CurrentItem);
                    return;
                }
            }
        }

        
        Character->ServerUseItem(CurrentItem, FVector::ZeroVector);
    }
    else
    {
        Character->ServerCastSpell(CurrentSpell.Name);
    }
}

void UGameSlot::OnSlotButtonClicked()
{
    if (!bHasContent)
        return;

    
    if (SlotPlacement == ESlotPlacement::ActionBar && !IsActionBarLocked())
        return;

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return;

    if (SlotPlacement == ESlotPlacement::Loot)
    {
        APlayerController *PC = Cast<APlayerController>(Character->GetController());
        if (!PC)
            return;

        AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD());
        if (!HUD)
            return;

        ULootWidget *LootWidget = HUD->GetLootWidget();
        if (!LootWidget)
            return;

        AItemContainer *Container = Cast<AItemContainer>(LootWidget->GetSourceContainer());
        if (!Container)
            return;

        int32 MaxStackCount = IsCtrlPressed() ? 100 : 20;
        FGameItem AdjustedItem = CurrentItem;
        if (CurrentItem.bIsStackable)
        {
            AdjustedItem.StackCount = FMath::Min(CurrentItem.StackCount, MaxStackCount);
        }

        Character->ServerTransferItemFromContainer(Container, AdjustedItem);
        return;
    }

    if (SlotPlacement == ESlotPlacement::Inventory && CurrentItem.Name == TEXT("Reward Present"))
    {
        Character->ServerOpenRewardPresent();
        return;
    }

    
    ExecuteActionBarAction();
}

FReply UGameSlot::NativeOnMouseButtonDown(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent)
{
    if (!bHasContent)
        return FReply::Handled();

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        
        if (SlotPlacement == ESlotPlacement::Inventory ||
            SlotPlacement == ESlotPlacement::Loot ||
            SlotPlacement == ESlotPlacement::Equipment ||
            SlotPlacement == ESlotPlacement::SpellList)
        {
            return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
        }
        
        else if (SlotPlacement == ESlotPlacement::ActionBar && !IsActionBarLocked())
        {
            return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
        }
        else
        {
            OnSlotButtonClicked();
        }
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton &&
        (SlotPlacement == ESlotPlacement::Inventory || SlotPlacement == ESlotPlacement::Loot))
    {
        OnSlotButtonClicked();
    }

    return FReply::Handled();
}

bool UGameSlot::IsCtrlPressed() const
{
    if (APlayerController *PC = GetOwningPlayer())
    {
        return PC->IsInputKeyDown(EKeys::LeftControl) || PC->IsInputKeyDown(EKeys::RightControl);
    }
    return false;
}

void UGameSlot::NativeOnDragCancelled(const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation)
{
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
    if (SlotButton)
    {
        SlotButton->WidgetStyle.Normal.OutlineSettings.Color = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

bool UGameSlot::NativeOnDragOver(const FGeometry &InGeometry, const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation)
{
    UItemDragDropOperation *ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
    if (!ItemDragDrop || !ItemDragDrop->SourceSlot)
        return false;

    
    if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::ActionBar &&
        SlotPlacement != ESlotPlacement::ActionBar)
        return false;

    
    if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::SpellList &&
        SlotPlacement != ESlotPlacement::ActionBar)
        return false;

    
    if (SlotPlacement == ESlotPlacement::Equipment)
    {
        if (ItemDragDrop->DraggedItem.ItemType != EItemType::Equipment ||
            ItemDragDrop->DraggedItem.EquipmentAttributes.EquipmentType != EquipmentType)
        {
            return false;
        }
    }

    
    if (SlotButton)
    {
        SlotButton->WidgetStyle.Normal.OutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    return true;
}

void UGameSlot::NativeOnDragLeave(const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation)
{
    Super::NativeOnDragLeave(InDragDropEvent, InOperation);
    if (SlotButton)
    {
        SlotButton->WidgetStyle.Normal.OutlineSettings.Color = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void UGameSlot::AssignHotkey(const FKey &Hotkey)
{
    AssignedHotkey = Hotkey;

    if (HotkeyText)
    {
        FString HotkeyString = Hotkey.ToString();
        HotkeyString.ReplaceInline(TEXT("Key_"), TEXT(""));
        HotkeyText->SetText(FText::FromString(HotkeyString));
        HotkeyText->SetVisibility(ESlateVisibility::Visible);
    }
}

bool UGameSlot::HandleThrowItemAway(UItemDragDropOperation *ItemDragDrop)
{
    if (ItemDragDrop->SourceSlot->GetSlotPlacement() != ESlotPlacement::Inventory)
        return false;

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return false;

    auto amountToRemove = FMath::Min(ItemDragDrop->DraggedItem.StackCount, 50);
    Character->ServerRemoveItem(ItemDragDrop->DraggedItem.Name, amountToRemove);
    return true;
}

bool UGameSlot::HandleUnsetActionBarSlot(UItemDragDropOperation *ItemDragDrop)
{
    if (ItemDragDrop->SourceSlot->GetSlotPlacement() != ESlotPlacement::ActionBar)
        return false;

    
    ItemDragDrop->SourceSlot->SetEmpty();
    return true;
}

void UGameSlot::NativeOnMouseEnter(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (bHasContent)
    {
        GetWorld()->GetTimerManager().SetTimer(
            TooltipTimerHandle,
            this,
            &UGameSlot::ShowTooltip,
            0.2f,
            false);
    }
}

void UGameSlot::NativeOnMouseLeave(const FPointerEvent &InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    
    GetWorld()->GetTimerManager().ClearTimer(TooltipTimerHandle);
    HideTooltip();
}

void UGameSlot::ShowTooltip()
{
    if (!ItemTooltipWidget)
    {
        ItemTooltipWidget = CreateWidget<UUserWidget>(GetWorld()->GetGameInstance(), LoadClass<UUserWidget>(nullptr, TEXT("/Game/HUDs/Widgets/WBP_Tooltip.WBP_Tooltip_C")));
        if (!ItemTooltipWidget)
            return;
    }

    if (UTooltipWidget *TooltipWidget = Cast<UTooltipWidget>(ItemTooltipWidget))
    {
        TooltipWidget->SetTitle(GetCurrentTitle());
        TooltipWidget->SetDescription(GetCurrentDescription());
        TooltipWidget->AddToViewport(100);
    }
}

void UGameSlot::HideTooltip()
{
    if (ItemTooltipWidget && ItemTooltipWidget->IsInViewport())
    {
        ItemTooltipWidget->RemoveFromViewport();
    }
}

FString UGameSlot::GetCurrentTitle() const
{
    if (!bHasContent)
        return FString();

    if (SlotType == ESlotType::Item)
        return CurrentItem.Name;
    else if (SlotType == ESlotType::Spell)
        return CurrentSpell.Name;
    return FString();
}

FString UGameSlot::GetCurrentDescription() const
{
    if (!bHasContent)
        return FString();

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());

    if (SlotType == ESlotType::Item)
    {
        
        const FSpellDefinition *SpellDef = USpellDatabase::GetSpell(CurrentItem.Name);
        if (SpellDef)
        {
            return SpellDef->GetDetailedDescription(Character);
        }

        
        switch (CurrentItem.ItemType)
        {
        case EItemType::Equipment:
        case EItemType::Consumable:
        case EItemType::Tool:
            return CurrentItem.GetDetailedDescription(CurrentItem.StackCount);
        default:
            return CurrentItem.Description;
        }
    }
    else if (SlotType == ESlotType::Spell)
    {
        const FSpellDefinition *SpellDef = USpellDatabase::GetSpell(CurrentSpell.Name);
        if (SpellDef)
        {
            return SpellDef->GetDetailedDescription(Character);
        }
    }

    return FString();
}

void UGameSlot::SetAsEquipmentSlot(EEquipmentType Type)
{
    EquipmentType = Type;
    SlotPlacement = ESlotPlacement::Equipment;
    SetEmpty();

    
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (Character)
    {
        FGameItem EquippedItem;
        switch (Type)
        {
        case EEquipmentType::Helm:
            EquippedItem = Character->GetEquippedHelm();
            break;
        case EEquipmentType::Armor:
            EquippedItem = Character->GetEquippedArmor();
            break;
        case EEquipmentType::Bag:
            EquippedItem = Character->GetEquippedBag();
            break;
        case EEquipmentType::Weapon:
            EquippedItem = Character->GetEquippedWeapon();
            break;
        case EEquipmentType::Shield:
            EquippedItem = Character->GetEquippedShield();
            break;
        case EEquipmentType::Legs:
            EquippedItem = Character->GetEquippedLegs();
            break;
        case EEquipmentType::Boots:
            EquippedItem = Character->GetEquippedBoots();
            break;
        case EEquipmentType::Amulet:
            EquippedItem = Character->GetEquippedAmulet();
            break;
        case EEquipmentType::Ring:
            EquippedItem = Character->GetEquippedRing();
            break;
        case EEquipmentType::Ammunition:
            EquippedItem = Character->GetEquippedAmmunition();
            break;
        }

        if (!EquippedItem.Name.IsEmpty())
        {
            SetItem(EquippedItem);
            return;
        }
    }

    
    bHasContent = false;
    if (SlotType == ESlotType::Item)
    {
        CurrentItem = FGameItem();
    }
    else
    {
        CurrentSpell = FSpellDefinition();
    }

    if (ItemIcon)
    {
        if (DefaultIcon)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(DefaultIcon);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.Tiling = ESlateBrushTileType::NoTile;
            ItemIcon->SetBrush(Brush);
            ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f));
        }
        else
        {
            ItemIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
        }
    }

    if (StackCountText)
    {
        StackCountText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

bool UGameSlot::HandleEquipmentDrop(UItemDragDropOperation *ItemDragDrop)
{
    
    if (ItemDragDrop->DraggedItem.ItemType != EItemType::Equipment)
        return false;

    
    if (ItemDragDrop->DraggedItem.EquipmentAttributes.EquipmentType != EquipmentType)
        return false;

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return false;

    
    if (EquipmentType == EEquipmentType::Weapon)
    {
        bool bIsTwoHanded = ItemDragDrop->DraggedItem.EquipmentAttributes.bIsTwoHanded;
        bool bHasShieldEquipped = !Character->GetEquippedShield().Name.IsEmpty();

        if (bIsTwoHanded && bHasShieldEquipped)
        {
            
            return false;
        }
    }
    else if (EquipmentType == EEquipmentType::Shield)
    {
        
        const FGameItem &EquippedWeapon = Character->GetEquippedWeapon();
        if (!EquippedWeapon.Name.IsEmpty() && EquippedWeapon.EquipmentAttributes.bIsTwoHanded)
        {
            
            return false;
        }
    }

    
    if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Loot)
    {
        UE_LOG(LogTemp, Log, TEXT("Handling equipment drop from loot container"));
        APlayerController *PC = Cast<APlayerController>(Character->GetController());
        if (!PC)
            return false;

        AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD());
        if (!HUD)
            return false;

        ULootWidget *LootWidget = HUD->GetLootWidget();
        if (!LootWidget)
            return false;

        AItemContainer *Container = Cast<AItemContainer>(LootWidget->GetSourceContainer());
        if (!Container)
            return false;

        
        FGameItem AdjustedItem = ItemDragDrop->DraggedItem;

        
        if (AdjustedItem.EquipmentAttributes.EquipmentType == EEquipmentType::Ammunition)
        {
            
            int32 ItemsPerTransfer = 100;
            AdjustedItem.StackCount = FMath::Min(AdjustedItem.StackCount, ItemsPerTransfer);
        }
        else
        {
            
            AdjustedItem.StackCount = 1;
        }

        
        Character->ServerTransferItemFromContainer(Container, AdjustedItem);
        Character->ServerEquipItem(AdjustedItem);
        return true;
    }
    
    else if (ItemDragDrop->SourceSlot->GetSlotPlacement() == ESlotPlacement::Inventory)
    {
        Character->ServerEquipItem(ItemDragDrop->DraggedItem);
        return true;
    }

    return false;
}

bool UGameSlot::HandleUnequipToInventory(UItemDragDropOperation *ItemDragDrop)
{
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return false;

    Character->ServerUnequipItem(ItemDragDrop->DraggedItem);
    return true;
}

bool UGameSlot::HandleUnequipToContainer(UItemDragDropOperation *ItemDragDrop)
{
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return false;

    APlayerController *PC = Cast<APlayerController>(Character->GetController());
    if (!PC)
        return false;

    AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD());
    if (!HUD)
        return false;

    ULootWidget *LootWidget = HUD->GetLootWidget();
    if (!LootWidget)
        return false;

    AItemContainer *Container = Cast<AItemContainer>(LootWidget->GetSourceContainer());
    if (!Container)
        return false;

    
    Character->ServerUnequipItem(ItemDragDrop->DraggedItem);

    
    Character->ServerTransferItemToContainer(Container, ItemDragDrop->DraggedItem);
    return true;
}

bool UGameSlot::IsActionBarLocked() const
{
    AFibulaHUD *HUD = Cast<AFibulaHUD>(GetOwningPlayer()->GetHUD());
    if (!HUD)
        return false;

    UStatusWidget *StatusWidget = HUD->GetStatusWidget();
    if (!StatusWidget)
        return false;

    return StatusWidget->IsActionBarLocked();
}
