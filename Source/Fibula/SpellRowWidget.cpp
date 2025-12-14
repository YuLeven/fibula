#include "SpellRowWidget.h"

void USpellRowWidget::SetupSpell(const FSpellDefinition &Spell)
{
    if (SpellSlot)
    {
        SpellSlot->SetSpell(Spell);
        SpellSlot->SetSlotPlacement(ESlotPlacement::SpellList);
    }

    if (NameText)
    {
        NameText->SetText(FText::FromString(Spell.Name));
    }

    if (WordsText)
    {
        WordsText->SetVisibility(Spell.Words.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
        WordsText->SetText(FText::FromString(Spell.Words));
    }
}