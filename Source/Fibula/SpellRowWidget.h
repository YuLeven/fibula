#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSlot.h"
#include "SpellSystem.h"
#include "SpellRowWidget.generated.h"

UCLASS()
class FIBULA_API USpellRowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetupSpell(const FSpellDefinition &Spell);

protected:
    UPROPERTY(meta = (BindWidget))
    UGameSlot *SpellSlot;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *WordsText;
};