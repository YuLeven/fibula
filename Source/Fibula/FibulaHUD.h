#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FloatingTextWidget.h"
#include "FloatingNumberWidget.h"
#include "StatusWidget.h"
#include "LootWidget.h"
#include "FibulaHUD.generated.h"

UCLASS()
class FIBULA_API AFibulaHUD : public AHUD
{
    GENERATED_BODY()

public:
    AFibulaHUD();
    virtual void BeginPlay() override;

    TSubclassOf<UFloatingTextWidget> GetFloatingTextWidgetClass() const { return FloatingTextWidgetClass; }

    TSubclassOf<class UFloatingNumberWidget> GetFloatingNumberWidgetClass() const { return FloatingNumberWidgetClass; }

    UFUNCTION(BlueprintCallable, Category = "UI")
    UStatusWidget *GetStatusWidget() const { return StatusWidget; }

    void StartItemTargeting(UGameSlot *SourceSlot, const FGameItem &Item);
    void EndItemTargeting();

    void ShowLootWindow(AItemContainer *Container);
    void HideLootWindow();

    UFUNCTION()
    ULootWidget *GetLootWidget() const { return LootWidget; }

    void HandleHotkeyPressed(const FKey &PressedKey);
    UGameSlot *FindSlotByHotkey(const FKey &Hotkey) const;

protected:
    virtual void DrawHUD() override;

    UPROPERTY()
    class ULootWidget *LootWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class ULootWidget> LootWidgetClass;

private:
    TSubclassOf<UFloatingTextWidget> FloatingTextWidgetClass;
    TSubclassOf<class UFloatingNumberWidget> FloatingNumberWidgetClass;

    UPROPERTY()
    UStatusWidget *StatusWidget;

    TSubclassOf<UStatusWidget> StatusWidgetClass;

    bool bIsTargetingItem;
    UGameSlot *TargetingSourceSlot;
    FGameItem TargetingItem;

    void HandleItemTargetingClick();

    UPROPERTY()
    UNiagaraSystem *TargetingCircleTemplate;

    UPROPERTY()
    UNiagaraComponent *ActiveTargetingCircle;
};