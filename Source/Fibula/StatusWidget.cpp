#include "StatusWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/UniformGridPanel.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "GameSlot.h"
#include "FibulaCharacter.h"
#include "SettingsWidget.h"
#include "EquipmentTypes.h"
#include "ScoreboardWidget.h"
#include "GameModeType.h"
#include "FibulaTeamBattleGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "FloatingTextWidget.h"
#include "FibulaPlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SpellSystem.h"
#include "SpellDatabase.h"
#include "EngineUtils.h"
#include "FibulaHUD.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UStatusWidget::UStatusWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UGameSlot> GameSlotBPClass(TEXT("/Game/HUDs/Widgets/WBP_GameSlot"));
    if (GameSlotBPClass.Class != nullptr)
    {
        GameSlotClass = GameSlotBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<USettingsWidget> SettingsWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_SettingsWidget"));
    if (SettingsWidgetBPClass.Class != nullptr)
    {
        SettingsWidgetClass = SettingsWidgetBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<UScoreboardWidget> ScoreboardWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_ScoreboardWidget"));
    if (ScoreboardWidgetBPClass.Class != nullptr)
    {
        ScoreboardWidgetClass = ScoreboardWidgetBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<USpellListWidget> SpellListWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_SpellListWidget"));
    if (SpellListWidgetBPClass.Class != nullptr)
    {
        SpellListWidgetClass = SpellListWidgetBPClass.Class;
    }
}

void UStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RegisterStatusIcons();

    if (BackpackButton)
    {
        BackpackButton->OnClicked.AddDynamic(this, &UStatusWidget::OnBackpackButtonClicked);
    }

    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UStatusWidget::OnSettingsButtonClicked);
    }

    if (SpellListButton)
    {
        SpellListButton->OnClicked.AddDynamic(this, &UStatusWidget::OnSpellListButtonClicked);
    }

    if (LockActionBarCheckBox)
    {
        LockActionBarCheckBox->SetIsChecked(true);
    }

    if (InventoryPanel)
    {
        InventoryPanel->SetVisibility(ESlateVisibility::Collapsed);
        bIsInventoryVisible = false;

        
        InitializeEquipmentSlots();
        InitializeInventoryGrid();
    }

    if (APlayerController *PC = GetOwningPlayer())
    {
        if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
        {
            UpdateInventoryDisplay(Character->GetInventory());
            UpdateCapacity(Character->GetCurrentCapacity(), Character->GetMaxCapacity());
            UpdateHealth(Character->GetCurrentHealth(), Character->GetMaxHealth());
            UpdateMana(Character->GetCurrentMana(), Character->GetMaxMana());
        }
    }

    if (ScoreboardButton)
    {
        ScoreboardButton->OnClicked.AddDynamic(this, &UStatusWidget::OnScoreboardButtonClicked);
    }

    UpdateMatchDetails();

    if (!ScoreboardWidget && ScoreboardWidgetClass)
    {
        ScoreboardWidget = CreateWidget<UScoreboardWidget>(GetOwningPlayer(), ScoreboardWidgetClass);
    }

    
    if (ChatInputBox)
    {
        ChatInputBox->OnTextCommitted.AddDynamic(this, &UStatusWidget::OnChatInputCommitted);
        ChatInputBox->SetClearKeyboardFocusOnCommit(false);
    }

    
    SetupActionBar();
    UnfocusChat();

    
    MaxRetryAttempts = 5;
    CurrentRetryAttempt = 0;

    GetWorld()->GetTimerManager().SetTimer(
        RetryTimerHandle,
        this,
        &UStatusWidget::RetrySetupActionBar,
        0.5f,
        true); 
}

void UStatusWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }

    if (HealthText)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth))));
    }
}

void UStatusWidget::UpdateMana(float CurrentMana, float MaxMana)
{
    if (ManaBar)
    {
        ManaBar->SetPercent(CurrentMana / MaxMana);
    }

    if (ManaText)
    {
        ManaText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), FMath::RoundToInt(CurrentMana), FMath::RoundToInt(MaxMana))));
    }
}

void UStatusWidget::UpdateExperience(float CurrentExp, float NextLevelExp)
{
    if (ExperienceBar)
    {
        ExperienceBar->SetPercent(CurrentExp / NextLevelExp);
    }
}

void UStatusWidget::UpdateLevel(int32 NewLevel)
{
    if (LevelText)
    {
        LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), NewLevel)));
    }
}

void UStatusWidget::ShowInventory()
{
    if (!bIsInventoryVisible)
    {
        ToggleInventory();
    }
}

void UStatusWidget::OnBackpackButtonClicked()
{
    ToggleInventory();
}

void UStatusWidget::ToggleInventory()
{
    if (InventoryPanel)
    {
        bIsInventoryVisible = !bIsInventoryVisible;
        InventoryPanel->SetVisibility(bIsInventoryVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UStatusWidget::InitializeInventoryGrid()
{
    if (!InventoryGrid || !GameSlotClass)
    {
        return;
    }

    
    InventoryGrid->ClearChildren();
    GameSlots.Empty();

    
    InventoryGrid->SetMinDesiredSlotWidth(SLOT_SIZE);
    InventoryGrid->SetMinDesiredSlotHeight(SLOT_SIZE);

    
    for (int32 Row = 0; Row < INVENTORY_ROWS; Row++)
    {
        for (int32 Col = 0; Col < INVENTORY_COLS; Col++)
        {
            UGameSlot *NewSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
            if (!NewSlot)
            {
                continue;
            }

            
            USizeBox *SizeBoxWidget = NewObject<USizeBox>(this);
            SizeBoxWidget->SetWidthOverride(SLOT_SIZE);
            SizeBoxWidget->SetHeightOverride(SLOT_SIZE);
            SizeBoxWidget->AddChild(NewSlot);

            
            InventoryGrid->AddChildToUniformGrid(SizeBoxWidget, Row, Col);
            GameSlots.Add(NewSlot);

            NewSlot->SetSlotPlacement(ESlotPlacement::Inventory);
        }
    }
}

void UStatusWidget::UpdateInventoryDisplay(const TArray<FGameItem> &Items)
{
    
    for (UGameSlot *GameSlot : GameSlots)
    {
        if (GameSlot)
        {
            GameSlot->SetEmpty();
        }
    }

    
    for (int32 i = 0; i < Items.Num() && i < GameSlots.Num(); ++i)
    {
        if (GameSlots[i])
        {
            GameSlots[i]->SetItem(Items[i]);
        }
    }
}

void UStatusWidget::UpdateCapacity(float CurrentCapacity, float MaxCapacity)
{
    if (CapacityText)
    {
        CapacityText->SetText(FText::AsNumber(FMath::RoundToInt(MaxCapacity - CurrentCapacity)));
    }
}

void UStatusWidget::RegisterStatusIcons()
{
    
    FStatusIconData InBattleIcon;
    InBattleIcon.StatusType = EStatusIconType::InBattle;
    InBattleIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/InBattle")));
    InBattleIcon.TooltipText = FText::FromString(TEXT("In Combat"));
    StatusIconDefinitions.Add(EStatusIconType::InBattle, InBattleIcon);

    FStatusIconData MagicShieldIcon;
    MagicShieldIcon.StatusType = EStatusIconType::MagicShield;
    MagicShieldIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/MagicShield")));
    MagicShieldIcon.TooltipText = FText::FromString(TEXT("Magic Shield Active"));
    StatusIconDefinitions.Add(EStatusIconType::MagicShield, MagicShieldIcon);

    FStatusIconData HasteIcon;
    HasteIcon.StatusType = EStatusIconType::Haste;
    HasteIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/Haste")));
    HasteIcon.TooltipText = FText::FromString(TEXT("Increased Movement Speed"));
    StatusIconDefinitions.Add(EStatusIconType::Haste, HasteIcon);

    FStatusIconData ParalysedIcon;
    ParalysedIcon.StatusType = EStatusIconType::Paralysed;
    ParalysedIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/Paralysed")));
    ParalysedIcon.TooltipText = FText::FromString(TEXT("Movement Impaired"));
    StatusIconDefinitions.Add(EStatusIconType::Paralysed, ParalysedIcon);

    FStatusIconData WhiteSkullIcon;
    WhiteSkullIcon.StatusType = EStatusIconType::WhiteSkull;
    WhiteSkullIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/WhiteSkull")));
    WhiteSkullIcon.TooltipText = FText::FromString(TEXT("10 frags"));
    StatusIconDefinitions.Add(EStatusIconType::WhiteSkull, WhiteSkullIcon);

    FStatusIconData RedSkullIcon;
    RedSkullIcon.StatusType = EStatusIconType::RedSkull;
    RedSkullIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/RedSkull")));
    RedSkullIcon.TooltipText = FText::FromString(TEXT("50 frags"));
    StatusIconDefinitions.Add(EStatusIconType::RedSkull, RedSkullIcon);

    FStatusIconData BlackSkullIcon;
    BlackSkullIcon.StatusType = EStatusIconType::BlackSkull;
    BlackSkullIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/BlackSkull")));
    BlackSkullIcon.TooltipText = FText::FromString(TEXT("100 frags"));
    StatusIconDefinitions.Add(EStatusIconType::BlackSkull, BlackSkullIcon);

    FStatusIconData ProtectionZoneIcon;
    ProtectionZoneIcon.StatusType = EStatusIconType::InProtectionZone;
    ProtectionZoneIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/InProtectionZone")));
    ProtectionZoneIcon.TooltipText = FText::FromString(TEXT("Protected Area"));
    StatusIconDefinitions.Add(EStatusIconType::InProtectionZone, ProtectionZoneIcon);

    FStatusIconData StrengthenedIcon;
    StrengthenedIcon.StatusType = EStatusIconType::Strengthened;
    StrengthenedIcon.IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/HUDs/Images/StatusIcons/Strengthened")));
    StrengthenedIcon.TooltipText = FText::FromString(TEXT("Strengthened"));
    StatusIconDefinitions.Add(EStatusIconType::Strengthened, StrengthenedIcon);
}

void UStatusWidget::AddStatusIcon(EStatusIconType StatusType)
{
    
    if (ActiveStatusIcons.Contains(StatusType))
    {
        return;
    }

    
    const FStatusIconData *IconData = StatusIconDefinitions.Find(StatusType);
    if (!IconData)
    {
        return;
    }
    if (!StatusIconsContainer)
    {
        return;
    }

    
    USizeBox *SizeBoxWidget = NewObject<USizeBox>(StatusIconsContainer);
    SizeBoxWidget->SetWidthOverride(9.0f);
    SizeBoxWidget->SetHeightOverride(9.0f);

    
    UImage *NewIcon = NewObject<UImage>(SizeBoxWidget);
    if (!NewIcon)
    {
        return;
    }

    
    NewIcon->SetBrushFromTexture(IconData->IconTexture);
    NewIcon->SetToolTipText(IconData->TooltipText);

    
    SizeBoxWidget->AddChild(NewIcon);

    
    StatusIconsContainer->AddChild(SizeBoxWidget);

    
    ActiveStatusIcons.Add(StatusType, NewIcon);
}

void UStatusWidget::RemoveStatusIcon(EStatusIconType StatusType)
{
    UImage **ExistingIcon = ActiveStatusIcons.Find(StatusType);
    if (ExistingIcon && *ExistingIcon && StatusIconsContainer)
    {
        StatusIconsContainer->RemoveChild(*ExistingIcon);
        ActiveStatusIcons.Remove(StatusType);
    }
}

void UStatusWidget::ClearAllStatusIcons()
{
    if (StatusIconsContainer)
    {
        StatusIconsContainer->ClearChildren();
        ActiveStatusIcons.Empty();
    }
}

void UStatusWidget::OnSettingsButtonClicked()
{
    if (!SettingsWidget && SettingsWidgetClass)
    {
        SettingsWidget = CreateWidget<USettingsWidget>(GetOwningPlayer(), SettingsWidgetClass);
    }

    if (SettingsWidget)
    {
        if (SettingsWidget->IsInViewport())
        {
            SettingsWidget->RemoveFromParent();
        }
        else
        {
            SettingsWidget->AddToViewport();
        }
    }
}

void UStatusWidget::InitializeEquipmentSlots()
{
    if (!GameSlotClass)
        return;

    
    if (!HelmSlot)
        HelmSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!ArmorSlot)
        ArmorSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!BagSlot)
        BagSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!WeaponSlot)
        WeaponSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!ShieldSlot)
        ShieldSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!LegsSlot)
        LegsSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!BootsSlot)
        BootsSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!AmuletSlot)
        AmuletSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!RingSlot)
        RingSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!AmmunitionSlot)
        AmmunitionSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
    if (!GarbageSlot)
        GarbageSlot = CreateWidget<UGameSlot>(this, GameSlotClass);

    
    if (HelmSlot)
    {
        HelmSlot->SetAsEquipmentSlot(EEquipmentType::Helm);
    }
    if (ArmorSlot)
    {
        ArmorSlot->SetAsEquipmentSlot(EEquipmentType::Armor);
    }
    if (BagSlot)
    {
        BagSlot->SetAsEquipmentSlot(EEquipmentType::Bag);
    }
    if (WeaponSlot)
    {
        WeaponSlot->SetAsEquipmentSlot(EEquipmentType::Weapon);
    }
    if (ShieldSlot)
    {
        ShieldSlot->SetAsEquipmentSlot(EEquipmentType::Shield);
    }
    if (LegsSlot)
    {
        LegsSlot->SetAsEquipmentSlot(EEquipmentType::Legs);
    }
    if (BootsSlot)
    {
        BootsSlot->SetAsEquipmentSlot(EEquipmentType::Boots);
    }
    if (AmuletSlot)
    {
        AmuletSlot->SetAsEquipmentSlot(EEquipmentType::Amulet);
    }
    if (RingSlot)
    {
        RingSlot->SetAsEquipmentSlot(EEquipmentType::Ring);
    }
    if (AmmunitionSlot)
    {
        AmmunitionSlot->SetAsEquipmentSlot(EEquipmentType::Ammunition);
    }
    if (GarbageSlot)
    {
        GarbageSlot->SetSlotPlacement(ESlotPlacement::Garbage);
        GarbageSlot->SetEmpty();
    }
}

void UStatusWidget::UpdateEquipmentSlot(EEquipmentType SlotType, const FGameItem &Item)
{
    UGameSlot *TargetSlot = nullptr;

    switch (SlotType)
    {
    case EEquipmentType::Helm:
        TargetSlot = HelmSlot;
        break;
    case EEquipmentType::Armor:
        TargetSlot = ArmorSlot;
        break;
    case EEquipmentType::Bag:
        TargetSlot = BagSlot;
        break;
    case EEquipmentType::Weapon:
        TargetSlot = WeaponSlot;
        break;
    case EEquipmentType::Shield:
        TargetSlot = ShieldSlot;
        break;
    case EEquipmentType::Legs:
        TargetSlot = LegsSlot;
        break;
    case EEquipmentType::Boots:
        TargetSlot = BootsSlot;
        break;
    case EEquipmentType::Amulet:
        TargetSlot = AmuletSlot;
        break;
    case EEquipmentType::Ring:
        TargetSlot = RingSlot;
        break;
    case EEquipmentType::Ammunition:
        TargetSlot = AmmunitionSlot;
        break;
    }

    if (TargetSlot)
    {
        if (Item.Name.IsEmpty())
        {
            TargetSlot->SetEmpty();
        }
        else
        {
            TargetSlot->SetItem(Item);
        }
    }
}

void UStatusWidget::OnScoreboardButtonClicked()
{
    ToggleScoreboard();
}

void UStatusWidget::ShowScoreboard()
{
    if (!ScoreboardWidget && ScoreboardWidgetClass)
    {
        ScoreboardWidget = CreateWidget<UScoreboardWidget>(GetOwningPlayer(), ScoreboardWidgetClass);
    }
    if (ScoreboardWidget)
    {
        if (!ScoreboardWidget->IsInViewport())
        {
            ScoreboardWidget->AddToViewport();
        }
    }
}

void UStatusWidget::HideScoreboard()
{
    if (ScoreboardWidget)
    {
        ScoreboardWidget->RemoveFromViewport();
    }
}

void UStatusWidget::ToggleScoreboard()
{
    if (!ScoreboardWidget)
        return;

    if (ScoreboardWidget->IsInViewport())
        HideScoreboard();
    else
        ShowScoreboard();
}

void UStatusWidget::UpdateMatchDetails()
{
    
    if (TeamGameScoreContainer)
        TeamGameScoreContainer->SetVisibility(ESlateVisibility::Collapsed);

    
    APlayerController *PC = GetOwningPlayer();
    if (!PC)
        return;

    UWorld *World = PC->GetWorld();
    if (!World)
        return;

    AFibulaGameState *GameState = AFibulaGameState::FindProxyGameState(World);
    if (!GameState)
        return;

    
    if (GameState->GetCurrentGameMode() != EGameModeType::TeamBattle)
        return;

    
    AFibulaTeamBattleGameState *TBGameState = Cast<AFibulaTeamBattleGameState>(GameState);
    if (!TBGameState)
        return;

    
    if (!TBGameState->IsBattleActive())
        return;

    
    if (TeamGameScoreContainer)
        TeamGameScoreContainer->SetVisibility(ESlateVisibility::Visible);

    
    if (TeamOnePointsText)
    {
        int32 TeamOnePoints = TBGameState->GetTeamPoints(1);
        TeamOnePointsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), TeamOnePoints)));
    }

    if (TeamTwoPointsText)
    {
        int32 TeamTwoPoints = TBGameState->GetTeamPoints(2);
        TeamTwoPointsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), TeamTwoPoints)));
    }

    
    if (BattleTimeText)
    {
        double RemainingTime = TBGameState->GetRemainingBattleTime();
        int32 Minutes = FMath::FloorToInt(RemainingTime / 60.0f);
        int32 Seconds = FMath::FloorToInt(RemainingTime - Minutes * 60.0f);
        BattleTimeText->SetText(FText::FromString(FString::Printf(TEXT("Battle ends in: %02d:%02d"), Minutes, Seconds)));
    }
}

void UStatusWidget::ShowVictoryScoreboard()
{
    if (ScoreboardWidget)
    {
        ScoreboardWidget->ShowVictory();
        ShowScoreboard();
    }
}

void UStatusWidget::ShowDefeatScoreboard()
{
    if (ScoreboardWidget)
    {
        ScoreboardWidget->ShowDefeat();
        ShowScoreboard();
    }
}

void UStatusWidget::ShowDrawScoreboard()
{
    if (ScoreboardWidget)
    {
        ScoreboardWidget->ShowDraw();
        ShowScoreboard();
    }
}

void UStatusWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!GetWorld() || !GetWorld()->HasBegunPlay())
        return;

    TimeSinceLastUpdate += InDeltaTime;
    if (TimeSinceLastUpdate >= UPDATE_INTERVAL)
    {
        UpdateMatchDetails();
        TimeSinceLastUpdate = 0.0f;
    }
}

void UStatusWidget::RetrySetupActionBar()
{
    if (!ActionBarPanel)
    {
        ClearRetryTimer();
        return;
    }

    APlayerController *PC = GetOwningPlayer();
    if (!PC)
    {
        ClearRetryTimer();
        return;
    }

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn());
    if (!Character)
    {
        IncrementRetryAttempt();
        return;
    }

    if (Character->GetVocation() == EVocation::None)
    {
        IncrementRetryAttempt();
        return;
    }

    
    SetupActionBar();
    ClearRetryTimer();
}

void UStatusWidget::IncrementRetryAttempt()
{
    CurrentRetryAttempt++;
    if (CurrentRetryAttempt >= MaxRetryAttempts)
    {
        ClearRetryTimer();
        UE_LOG(LogTemp, Warning, TEXT("Failed to setup action bar after %d attempts"), MaxRetryAttempts);
    }
}

void UStatusWidget::ClearRetryTimer()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RetryTimerHandle);
    }
}

void UStatusWidget::SetupActionBar()
{
    if (!ActionBarPanel)
        return;

    APlayerController *PC = GetOwningPlayer();
    if (!PC)
        return;

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn());
    if (!Character || Character->GetVocation() == EVocation::None)
        return;

    
    LoadActionBarLayout();

    
    if (ActionBarPanel->GetChildrenCount() == 0)
    {
        
        TArray<FSpellDefinition> AllSpells;
        for (const auto &SpellPair : USpellDatabase::GetAllSpells(Character->GetVocation()))
        {
            if (!SpellPair.Value.IconPath.IsEmpty())
            {
                AllSpells.Add(SpellPair.Value);
            }
        }

        const int32 SlotsPerRow = 10;
        const int32 NumRows = 2;
        const int32 TotalSlots = SlotsPerRow * NumRows;
        const float SlotSize = 34.0f;

        
        ActionBarPanel->ClearChildren();

        
        UUniformGridPanel *Grid = Cast<UUniformGridPanel>(ActionBarPanel);
        if (Grid)
        {
            Grid->SetMinDesiredSlotWidth(SlotSize);
            Grid->SetMinDesiredSlotHeight(SlotSize);
        }

        for (int32 i = 0; i < TotalSlots; i++)
        {
            int32 Row = i / SlotsPerRow;
            int32 Col = i % SlotsPerRow;

            UGameSlot *GameSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
            if (!GameSlot)
            {
                continue;
            }

            
            if (i < AllSpells.Num())
            {
                const FSpellDefinition &Spell = AllSpells[i];
                if (Spell.GetIsRune())
                {
                    const FGameItem *RuneItem = UItemDatabase::GetItem(Spell.Name);
                    if (RuneItem)
                    {
                        GameSlot->SetItem(*RuneItem);
                    }
                    else
                    {
                        GameSlot->SetEmpty();
                    }
                }
                else
                {
                    GameSlot->SetSpell(Spell);
                }
            }
            else
            {
                GameSlot->SetEmpty();
            }

            
            Cast<UUniformGridPanel>(ActionBarPanel)->AddChildToUniformGrid(GameSlot, Row, Col);

            GameSlot->SetSlotPlacement(ESlotPlacement::ActionBar);

            
            if (Row == 0 && Col < 10) 
            {
                FString HotkeyStr;
                if (Col == 9) 
                {
                    HotkeyStr = TEXT("0");
                }
                else
                {
                    HotkeyStr = FString::Printf(TEXT("%d"), Col + 1);
                }
                FKey Hotkey = FKey(*HotkeyStr);
                GameSlot->AssignHotkey(Hotkey);
            }
            else if (Row == 1 && Col < 10) 
            {
                FString HotkeyStr;
                switch (Col)
                {
                case 0:
                    HotkeyStr = TEXT("Q");
                    break;
                case 1:
                    HotkeyStr = TEXT("Z");
                    break;
                case 2:
                    HotkeyStr = TEXT("E");
                    break;
                case 3:
                    HotkeyStr = TEXT("C");
                    break;
                case 4:
                    HotkeyStr = TEXT("R");
                    break;
                case 5:
                    HotkeyStr = TEXT("F");
                    break;
                case 6:
                    HotkeyStr = TEXT("V");
                    break;
                case 7:
                    HotkeyStr = TEXT("T");
                    break;
                case 8:
                    HotkeyStr = TEXT("G");
                    break;
                case 9:
                    HotkeyStr = TEXT("B");
                    break;
                }
                FKey Hotkey = FKey(*HotkeyStr);
                GameSlot->AssignHotkey(Hotkey);
            }
        }
    }
}

void UStatusWidget::NativeDestruct()
{
    Super::NativeDestruct();
    SaveActionBarLayout();
}

void UStatusWidget::SaveActionBarLayout()
{
    UE_LOG(LogTemp, Log, TEXT("Saving action bar layout"));
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return;

    UActionBarSaveGame *SaveGameInstance = Cast<UActionBarSaveGame>(UGameplayStatics::CreateSaveGameObject(UActionBarSaveGame::StaticClass()));
    if (!SaveGameInstance)
        return;

    SaveGameInstance->Layout = GetCurrentActionBarLayout();
    SaveGameInstance->Vocation = Character->GetVocation();

    UGameplayStatics::SaveGameToSlot(SaveGameInstance, GetSaveSlotName(), 0);
}

void UStatusWidget::LoadActionBarLayout()
{
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return;

    UActionBarSaveGame *LoadedGame = Cast<UActionBarSaveGame>(UGameplayStatics::LoadGameFromSlot(GetSaveSlotName(), 0));
    if (!LoadedGame || LoadedGame->Vocation != Character->GetVocation())
        return;

    TArray<FActionBarSlotData> Layout = LoadedGame->Layout;

    
    LoadedGame->Layout = Layout;
    UGameplayStatics::SaveGameToSlot(LoadedGame, GetSaveSlotName(), 0);

    
    ApplyActionBarLayout(Layout);
}

FString UStatusWidget::GetSaveSlotName() const
{
    AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
    if (!Character)
        return FString();

    
    FString VocationName = UEnum::GetValueAsString(Character->GetVocation());
    VocationName.Split(TEXT("::"), nullptr, &VocationName);

    return FString::Printf(TEXT("ActionBar_%s"), *VocationName);
}

TArray<FActionBarSlotData> UStatusWidget::GetCurrentActionBarLayout() const
{
    TArray<FActionBarSlotData> Layout;
    if (!ActionBarPanel)
        return Layout;

    UUniformGridPanel *Grid = Cast<UUniformGridPanel>(ActionBarPanel);
    if (!Grid)
        return Layout;

    for (UWidget *Child : Grid->GetAllChildren())
    {
        if (UGameSlot *GameSlot = Cast<UGameSlot>(Child))
        {
            if (GameSlot->HasContent())
            {
                FString ContentName;
                if (GameSlot->GetSlotType() == ESlotType::Spell)
                    ContentName = GameSlot->GetSpell().Name;
                else
                    ContentName = GameSlot->GetItem().Name;

                Layout.Add(FActionBarSlotData(ContentName, GameSlot->GetSlotType()));
            }
            else
            {
                Layout.Add(FActionBarSlotData());
            }
        }
    }

    return Layout;
}

void UStatusWidget::ApplyActionBarLayout(const TArray<FActionBarSlotData> &Layout)
{
    if (!ActionBarPanel)
        return;

    UUniformGridPanel *Grid = Cast<UUniformGridPanel>(ActionBarPanel);
    if (!Grid)
        return;

    const int32 SlotsPerRow = 10;
    const int32 NumRows = 2;
    const float SlotSize = 34.0f;

    Grid->ClearChildren();

    UE_LOG(LogTemp, Log, TEXT("Applying action bar layout"));
    for (int32 i = 0; i < Layout.Num() && i < SlotsPerRow * NumRows; i++)
    {
        int32 Row = i / SlotsPerRow;
        int32 Col = i % SlotsPerRow;

        UGameSlot *GameSlot = CreateWidget<UGameSlot>(this, GameSlotClass);
        if (!GameSlot)
            continue;

        const FActionBarSlotData &SlotData = Layout[i];
        if (!SlotData.ContentName.IsEmpty())
        {
            if (SlotData.SlotType == ESlotType::Spell)
            {
                const FSpellDefinition *SpellDef = USpellDatabase::GetSpell(SlotData.ContentName);
                if (SpellDef)
                {
                    if (SpellDef->GetIsRune())
                    {
                        const FGameItem *RuneItem = UItemDatabase::GetItem(SlotData.ContentName);
                        if (RuneItem)
                            GameSlot->SetItem(*RuneItem);
                        else
                            GameSlot->SetEmpty();
                    }
                    else
                    {
                        GameSlot->SetSpell(*SpellDef);
                    }
                }
            }
            else
            {
                const FGameItem *Item = UItemDatabase::GetItem(SlotData.ContentName);
                if (Item)
                    GameSlot->SetItem(*Item);
                else
                    GameSlot->SetEmpty();
            }
        }
        else
        {
            GameSlot->SetEmpty();
        }

        Grid->AddChildToUniformGrid(GameSlot, Row, Col);
        GameSlot->SetSlotPlacement(ESlotPlacement::ActionBar);

        
        if (Row == 0 && Col < 10)
        {
            FString HotkeyStr = (Col == 9) ? TEXT("0") : FString::Printf(TEXT("%d"), Col + 1);
            GameSlot->AssignHotkey(FKey(*HotkeyStr));
        }
        else if (Row == 1 && Col < 10)
        {
            FString HotkeyStr;
            switch (Col)
            {
            case 0:
                HotkeyStr = TEXT("Q");
                break;
            case 1:
                HotkeyStr = TEXT("Z");
                break;
            case 2:
                HotkeyStr = TEXT("E");
                break;
            case 3:
                HotkeyStr = TEXT("C");
                break;
            case 4:
                HotkeyStr = TEXT("R");
                break;
            case 5:
                HotkeyStr = TEXT("F");
                break;
            case 6:
                HotkeyStr = TEXT("V");
                break;
            case 7:
                HotkeyStr = TEXT("T");
                break;
            case 8:
                HotkeyStr = TEXT("G");
                break;
            case 9:
                HotkeyStr = TEXT("B");
                break;
            }
            GameSlot->AssignHotkey(FKey(*HotkeyStr));
        }
    }
}

void UStatusWidget::FocusChat()
{
    if (ChatInputBox)
    {
        bIsChatFocused = true;
        ChatInputBox->SetKeyboardFocus();
    }
}

void UStatusWidget::UnfocusChat()
{
    if (!bIsChatFocused)
        return;

    FSlateApplication::Get().ClearAllUserFocus(EFocusCause::SetDirectly);
    FSlateApplication::Get().SetAllUserFocusToGameViewport(EFocusCause::SetDirectly);

    bIsChatFocused = false;
}

void UStatusWidget::SendMessage(const FString &Message)
{
    APlayerController *OwningPC = GetOwningPlayer();
    if (!OwningPC || !OwningPC->PlayerState)
        return;

    AFibulaCharacter *Character = Cast<AFibulaCharacter>(OwningPC->GetPawn());

    if (Character)
    {
        AFibulaPlayerState *PlayerState = Cast<AFibulaPlayerState>(OwningPC->PlayerState);
        if (PlayerState)
        {
            FString SenderName = Character->GetCharacterName();
            Character->ServerBroadcastMessage(SenderName, Message);
        }
    }
}

void UStatusWidget::AppendNewMessage(const FString &SenderName, const FString &Message)
{
    APlayerController *OtherPC = GetOwningPlayer();
    if (!OtherPC)
        return;

    
    AFibulaCharacter *SenderChar = nullptr;
    for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
    {
        AFibulaCharacter *Character = *It;
        if (Character && Character->GetCharacterName() == SenderName)
        {
            SenderChar = Character;
            break;
        }
    }

    if (!SenderChar)
        return;

    
    AFibulaCharacter *ReceiverChar = Cast<AFibulaCharacter>(OtherPC->GetPawn());
    if (ReceiverChar)
    {
        float Distance = FVector::Distance(SenderChar->GetActorLocation(), ReceiverChar->GetActorLocation());
        if (Distance > 2000.0f)
            return;
    }

    if (AFibulaHUD *HUD = Cast<AFibulaHUD>(OtherPC->GetHUD()))
    {
        if (TSubclassOf<UFloatingTextWidget> FloatingTextClass = HUD->GetFloatingTextWidgetClass())
        {
            
            UFloatingTextWidget *FloatingText = UFloatingTextWidget::GetOrCreateWidgetForChannel(
                OtherPC, FloatingTextClass, EFloatingTextChannel::Chat, SenderName);

            if (FloatingText)
            {
                
                FVector2D ScreenPosition;
                FVector WorldPosition = SenderChar->GetActorLocation();
                
                WorldPosition.Z += 150.0f;

                if (OtherPC->ProjectWorldLocationToScreen(WorldPosition, ScreenPosition))
                {
                    
                    TArray<FString> Lines;
                    const int32 MaxCharsPerLine = 40; 

                    
                    for (int32 i = 0; i < Message.Len(); i += MaxCharsPerLine)
                    {
                        int32 CharsToTake = FMath::Min(MaxCharsPerLine, Message.Len() - i);
                        Lines.Add(Message.Mid(i, CharsToTake));
                    }

                    
                    FString DisplayText = Lines[0];
                    for (int32 i = 1; i < Lines.Num(); i++)
                    {
                        DisplayText.Append(FString::Printf(TEXT("\n  %s"), *Lines[i]));
                    }

                    FloatingText->SetText(SenderName, DisplayText);

                    
                    FloatingText->SetPositionInViewport(ScreenPosition);
                }

                
                FString FullMessage = FString::Printf(TEXT("%s [%d]: %s"), *SenderName, SenderChar->GetCharacterLevel(), *Message);
                AddMessageToChat(FullMessage);
            }
        }
    }
}

void UStatusWidget::AddMessageToChat(const FString &FullMessage)
{
    if (!ChatScrollBox)
    {
        return;
    }

    UTextBlock *NewMessage = NewObject<UTextBlock>(ChatScrollBox);
    if (!NewMessage)
    {
        return;
    }

    NewMessage->SetText(FText::FromString(FullMessage));
    FSlateFontInfo FontInfo = NewMessage->Font;
    FontInfo.Size = 10;
    NewMessage->SetFont(FontInfo);
    ChatScrollBox->AddChild(NewMessage);

    while (ChatScrollBox->GetChildrenCount() > MaxChatMessages)
    {
        ChatScrollBox->RemoveChildAt(0);
    }

    ChatScrollBox->ScrollToEnd();
}

void UStatusWidget::OnChatInputCommitted(const FText &Text, ETextCommit::Type CommitMethod)
{
    FString Message = Text.ToString();

    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (!Message.IsEmpty())
        {
            SendMessage(Message);
            ChatInputBox->SetText(FText::GetEmpty());
        }
        UnfocusChat();
    }
    else if (CommitMethod == ETextCommit::OnCleared || CommitMethod == ETextCommit::OnUserMovedFocus)
    {
        UnfocusChat();
    }
}

void UStatusWidget::OnSpellListButtonClicked()
{
    if (!SpellListWidget && SpellListWidgetClass)
    {
        SpellListWidget = CreateWidget<USpellListWidget>(GetOwningPlayer(), SpellListWidgetClass);
        if (SpellListWidget)
        {
            SpellListWidget->AddToViewport();
        }
    }

    if (!SpellListWidget)
        return;

    if (bIsSpellListVisible)
    {
        bIsSpellListVisible = false;
        SpellListWidget->Hide();
    }
    else
    {
        AFibulaCharacter *Character = Cast<AFibulaCharacter>(GetOwningPlayerPawn());
        if (Character)
        {
            SpellListWidget->SetupSpellList(Character->GetVocation());
            SpellListWidget->Show();
            bIsSpellListVisible = true;
        }
    }
}
