#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/CheckBox.h"
#include "GameSlot.h"
#include "StatusIconData.h"
#include "ScoreboardWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "FloatingTextWidget.h"
#include "FibulaPlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SpellSystem.h"
#include "GameFramework/SaveGame.h"
#include "SpellListWidget.h"
#include "StatusWidget.generated.h"

USTRUCT()
struct FActionBarSlotData
{
    GENERATED_BODY()

    UPROPERTY()
    FString ContentName;

    UPROPERTY()
    ESlotType SlotType;

    FActionBarSlotData()
        : ContentName(""), SlotType(ESlotType::Item)
    {
    }

    FActionBarSlotData(const FString &InContentName, ESlotType InSlotType)
        : ContentName(InContentName), SlotType(InSlotType)
    {
    }
};

UCLASS()
class FIBULA_API UActionBarSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FActionBarSlotData> Layout;

    UPROPERTY()
    EVocation Vocation;
};

UCLASS()
class FIBULA_API UStatusWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UStatusWidget(const FObjectInitializer &ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    void UpdateHealth(float CurrentHealth, float MaxHealth);
    void UpdateMana(float CurrentMana, float MaxMana);
    void UpdateExperience(float CurrentExp, float NextLevelExp);
    void UpdateLevel(int32 NewLevel);
    void ToggleInventory();
    void ToggleScoreboard();
    void ShowInventory();
    void UpdateInventoryDisplay(const TArray<FGameItem> &Items);
    void UpdateCapacity(float CurrentCapacity, float MaxCapacity);
    void AddStatusIcon(EStatusIconType StatusType);
    void RemoveStatusIcon(EStatusIconType StatusType);
    void ClearAllStatusIcons();
    void UpdateEquipmentSlot(EEquipmentType SlotType, const FGameItem &Item);
    void InitializeEquipmentSlots();
    void ShowVictoryScoreboard();
    void ShowDefeatScoreboard();
    void ShowDrawScoreboard();
    void ShowScoreboard();
    void HideScoreboard();
    void FocusChat();
    void UnfocusChat();
    void SendMessage(const FString &Message);
    void AppendNewMessage(const FString &SenderName, const FString &Message);
    void AddMessageToChat(const FString &FullMessage);
    bool IsChatFocused() const { return bIsChatFocused; }
    bool IsActionBarLocked() const { return LockActionBarCheckBox->IsChecked(); }
    void SetupActionBar();
    UPanelWidget *GetActionBarPanel() const { return ActionBarPanel; }

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar *HealthBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar *ManaBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar *ExperienceBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *LevelText;

    UPROPERTY(meta = (BindWidget))
    UButton *BackpackButton;

    UPROPERTY(meta = (BindWidget))
    UButton *SpellListButton;

    UPROPERTY(meta = (BindWidget))
    class UCanvasPanel *InventoryPanel;

    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel *InventoryGrid;

    UPROPERTY()
    TArray<class UGameSlot *> GameSlots;

    
    static constexpr int32 INVENTORY_ROWS = 7;
    static constexpr int32 INVENTORY_COLS = 4;
    static constexpr float SLOT_SIZE = 34.0f;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGameSlot> GameSlotClass;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *CapacityText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *HealthText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *ManaText;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox *StatusIconsContainer;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Status Icons")
    TMap<EStatusIconType, FStatusIconData> StatusIconDefinitions;

    UPROPERTY()
    TMap<EStatusIconType, class UImage *> ActiveStatusIcons;

    UPROPERTY(meta = (BindWidget))
    class UButton *SettingsButton;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class USettingsWidget> SettingsWidgetClass;

    
    UPROPERTY(meta = (BindWidget))
    class UGameSlot *HelmSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *ArmorSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *BagSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *WeaponSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *ShieldSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *LegsSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *BootsSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *AmuletSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *RingSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *AmmunitionSlot;

    UPROPERTY(meta = (BindWidget))
    class UGameSlot *GarbageSlot;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox *TeamGameScoreContainer;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *TeamOnePointsText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *TeamTwoPointsText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *BattleTimeText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UEditableTextBox *ChatInputBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UScrollBox *ChatScrollBox;

    UPROPERTY(EditDefaultsOnly, Category = "Chat")
    int32 MaxChatMessages = 100;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UPanelWidget *ActionBarPanel;

    UPROPERTY(meta = (BindWidget))
    UCheckBox *LockActionBarCheckBox;

    bool bIsChatFocused;

    void InitializeInventoryGrid();

    virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

private:
    UFUNCTION()
    void OnBackpackButtonClicked();

    UFUNCTION()
    void OnSettingsButtonClicked();

    bool bIsInventoryVisible;
    bool bIsSpellListVisible;
    void RegisterStatusIcons();

    UPROPERTY()
    class USettingsWidget *SettingsWidget;

    UPROPERTY()
    UScoreboardWidget *ScoreboardWidget;

    UPROPERTY(meta = (BindWidget))
    UButton *ScoreboardButton;

    UFUNCTION()
    void OnScoreboardButtonClicked();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UScoreboardWidget> ScoreboardWidgetClass;

    UFUNCTION()
    void UpdateMatchDetails();

    float TimeSinceLastUpdate = 0.0f;
    static constexpr float UPDATE_INTERVAL = 1.0f;

    FTimerHandle RetryTimerHandle;
    int32 MaxRetryAttempts;
    int32 CurrentRetryAttempt;

    void RetrySetupActionBar();
    void IncrementRetryAttempt();
    void ClearRetryTimer();

    UFUNCTION()
    void OnChatInputCommitted(const FText &Text, ETextCommit::Type CommitMethod);

    void SaveActionBarLayout();
    void LoadActionBarLayout();
    FString GetSaveSlotName() const;
    TArray<FActionBarSlotData> GetCurrentActionBarLayout() const;
    void ApplyActionBarLayout(const TArray<FActionBarSlotData> &Layout);

    UPROPERTY()
    USpellListWidget *SpellListWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<USpellListWidget> SpellListWidgetClass;

    UFUNCTION()
    void OnSpellListButtonClicked();
};