#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameItem.h"
#include "FibulaGameState.h"
#include "SpellSystem.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDragDropOperation.h"
#include "EquipmentTypes.h"
#include "GameSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDragged, UGameSlot *, FromSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemDropped, UGameSlot *, FromSlot, UGameSlot *, ToSlot);

UENUM()
enum class ESlotType : uint8
{
    Item,
    Spell
};

UENUM()
enum class ESlotPlacement : uint8
{
    Inventory,
    Loot,
    ActionBar,
    Equipment,
    Garbage,
    SpellList
};

UCLASS()
class FIBULA_API UGameSlot : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeOnDragDetected(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent, UDragDropOperation *&OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry &InGeometry, const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent) override;
    virtual void NativeOnDragCancelled(const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation) override;
    virtual bool NativeOnDragOver(const FGeometry &InGeometry, const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation) override;
    virtual void NativeOnMouseEnter(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent &InMouseEvent) override;
    virtual void NativeOnDragLeave(const FDragDropEvent &InDragDropEvent, UDragDropOperation *InOperation) override;

    UPROPERTY(meta = (BindWidget))
    class UButton *SlotButton;

    UPROPERTY(meta = (BindWidget))
    class UImage *ItemIcon;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *StackCountText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *HotkeyText;

    UPROPERTY()
    FGameItem CurrentItem;

    UPROPERTY()
    FSpellDefinition CurrentSpell;

    UPROPERTY()
    ESlotType SlotType;

    bool bHasContent;

    UPROPERTY()
    ESlotPlacement SlotPlacement;

    UPROPERTY()
    FKey AssignedHotkey;

    bool IsCtrlPressed() const;
    bool IsActionBarLocked() const;

    UPROPERTY()
    class UUserWidget *ItemTooltipWidget;

    FTimerHandle TooltipTimerHandle;

    UPROPERTY()
    EEquipmentType EquipmentType = EEquipmentType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    UTexture2D *DefaultIcon;

public:
    UFUNCTION()
    void ExecuteActionBarAction();
    void SetEmpty();
    void SetItem(const FGameItem &Item);
    void SetSpell(const FSpellDefinition &Spell);
    const FGameItem &GetItem() const { return CurrentItem; }
    const FSpellDefinition &GetSpell() const { return CurrentSpell; }
    bool HasContent() const { return bHasContent; }
    ESlotType GetSlotType() const { return SlotType; }

    UPROPERTY(BlueprintAssignable, Category = "GameSlot")
    FOnItemDragged OnItemDragged;

    UPROPERTY(BlueprintAssignable, Category = "GameSlot")
    FOnItemDropped OnItemDropped;

    UGameSlot(const FObjectInitializer &ObjectInitializer);

    void SetSlotPlacement(ESlotPlacement NewPlacement) { SlotPlacement = NewPlacement; }
    ESlotPlacement GetSlotPlacement() const { return SlotPlacement; }

    void AssignHotkey(const FKey &Hotkey);
    const FKey &GetAssignedHotkey() const { return AssignedHotkey; }

    void SetAsEquipmentSlot(EEquipmentType Type);

private:
    bool HandleLooting(UItemDragDropOperation *ItemDragDrop);
    bool HandleInventoryReorganization(UItemDragDropOperation *ItemDragDrop);
    bool HandleActionBarDrop(UItemDragDropOperation *ItemDragDrop);
    bool HandleThrowItemAway(UItemDragDropOperation *ItemDragDrop);
    bool HandleUnsetActionBarSlot(UItemDragDropOperation *ItemDragDrop);
    bool HandleMovingItemToContainer(UItemDragDropOperation *ItemDragDrop);
    bool HandleEquipmentDrop(UItemDragDropOperation *ItemDragDrop);
    bool HandleUnequipToInventory(UItemDragDropOperation *ItemDragDrop);
    bool HandleUnequipToContainer(UItemDragDropOperation *ItemDragDrop);

    void ShowTooltip();
    void HideTooltip();
    void OnSlotButtonClicked();

    FString GetCurrentDescription() const;
    FString GetCurrentTitle() const;

    FString GetEquipmentSlotIconPath(EEquipmentType Type) const;
};