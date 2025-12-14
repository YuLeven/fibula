#include "SpellListWidget.h"
#include "SpellDatabase.h"
#include "SpellRowWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"

USpellListWidget::USpellListWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<USpellRowWidget> SpellRowWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_SpellRowWidget"));
    if (SpellRowWidgetBPClass.Class != nullptr)
    {
        SpellRowWidgetClass = SpellRowWidgetBPClass.Class;
    }
}

void USpellListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SpellScrollBox)
    {
        
        SpellScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible);
    }

    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    SetIsEnabled(false);
}

void USpellListWidget::SetupSpellList(EVocation Vocation)
{
    if (!SpellScrollBox)
        return;

    SpellScrollBox->ClearChildren();
    SpellScrollBox->SetVisibility(ESlateVisibility::Visible);
    SpellScrollBox->SetScrollBarVisibility(ESlateVisibility::Visible);
    SpellScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible);

    TArray<FSpellDefinition> SortedSpells;
    SortSpellsByVocation(Vocation, SortedSpells);

    
    for (int32 i = 0; i < SortedSpells.Num(); i += 2)
    {
        const FSpellDefinition *SecondSpell = (i + 1 < SortedSpells.Num()) ? &SortedSpells[i + 1] : nullptr;
        AddSpellPair(SortedSpells[i], SecondSpell);
    }
}

void USpellListWidget::Show()
{
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    SetIsEnabled(true);
}

void USpellListWidget::Hide()
{
    SetVisibility(ESlateVisibility::Hidden);
    SetIsEnabled(false);
}

void USpellListWidget::AddSpellPair(const FSpellDefinition &FirstSpell, const FSpellDefinition *SecondSpell)
{
    if (!SpellRowWidgetClass || !SpellScrollBox)
        return;

    
    UHorizontalBox *HorizontalBox = NewObject<UHorizontalBox>(this);

    
    USpellRowWidget *FirstSpellRow = CreateWidget<USpellRowWidget>(this, SpellRowWidgetClass);
    if (FirstSpellRow)
    {
        FirstSpellRow->SetupSpell(FirstSpell);
        UHorizontalBoxSlot *FirstSlot = HorizontalBox->AddChildToHorizontalBox(FirstSpellRow);
        FirstSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        FirstSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
    }

    
    if (SecondSpell)
    {
        USpellRowWidget *SecondSpellRow = CreateWidget<USpellRowWidget>(this, SpellRowWidgetClass);
        if (SecondSpellRow)
        {
            SecondSpellRow->SetupSpell(*SecondSpell);
            UHorizontalBoxSlot *SecondSlot = HorizontalBox->AddChildToHorizontalBox(SecondSpellRow);
            SecondSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            SecondSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        }
    }
    else
    {
        
        USizeBox *EmptyWidget = NewObject<USizeBox>(this);
        EmptyWidget->SetWidthOverride(34.0f); 
        UHorizontalBoxSlot *EmptySlot = HorizontalBox->AddChildToHorizontalBox(EmptyWidget);
        EmptySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }

    SpellScrollBox->AddChild(HorizontalBox);
}

void USpellListWidget::SortSpellsByVocation(EVocation Vocation, TArray<FSpellDefinition> &OutSpells)
{
    const TMap<FString, FSpellDefinition> &AllSpells = USpellDatabase::GetAllSpells(EVocation::None);
    TArray<FSpellDefinition> VocationSpells;
    TArray<FSpellDefinition> OtherSpells;

    for (const auto &SpellPair : AllSpells)
    {
        const FSpellDefinition &Spell = SpellPair.Value;
        if (Spell.AllowedVocations.Contains(Vocation))
        {
            VocationSpells.Add(Spell);
        }
        else if (Spell.AllowedVocations.Contains(EVocation::None))
        {
            OtherSpells.Add(Spell);
        }
    }

    
    VocationSpells.Sort([](const FSpellDefinition &A, const FSpellDefinition &B)
                        { return A.Name < B.Name; });

    OtherSpells.Sort([](const FSpellDefinition &A, const FSpellDefinition &B)
                     { return A.Name < B.Name; });

    OutSpells = VocationSpells;
    OutSpells.Append(OtherSpells);
}