#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/SizeBox.h"
#include "GameSlot.h"
#include "LootWidget.generated.h"

UCLASS()
class FIBULA_API ULootWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    ULootWidget(const FObjectInitializer &ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    void UpdateLootDisplay(const TArray<FGameItem> &Items);
    void SetSourceContainer(class AItemContainer *Container);
    class AItemContainer *GetSourceContainer() const { return SourceContainer; }

protected:
    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel *LootGrid;

    UPROPERTY()
    TArray<class UGameSlot *> LootSlots;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGameSlot> GameSlotClass;

    static constexpr int32 LOOT_ROWS = 7;
    static constexpr int32 LOOT_COLS = 4;
    static constexpr float SLOT_SIZE = 34.0f;
    static constexpr float CHECK_DISTANCE_INTERVAL = 0.5f;
    static constexpr float MAX_LOOT_DISTANCE = 300.0f;

private:
    UPROPERTY()
    class AItemContainer *SourceContainer;

    FTimerHandle DistanceCheckTimer;

    UPROPERTY(meta = (BindWidget))
    class UButton *CloseButton;

    UFUNCTION()
    void OnCloseButtonClicked();

    void InitializeLootGrid();
    void CheckDistance();
};