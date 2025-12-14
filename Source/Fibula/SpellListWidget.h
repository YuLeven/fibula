#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "GameSlot.h"
#include "SpellSystem.h"
#include "SpellRowWidget.h"
#include "SpellListWidget.generated.h"

UCLASS()
class FIBULA_API USpellListWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    USpellListWidget(const FObjectInitializer &ObjectInitializer);
    void SetupSpellList(EVocation Vocation);
    void Show();
    void Hide();
    bool IsVisible() const;

protected:
    virtual void NativeConstruct() override;

    
    UPROPERTY(meta = (BindWidget))
    UScrollBox *SpellScrollBox;

    
    UPROPERTY(EditDefaultsOnly, Category = "Templates")
    TSubclassOf<USpellRowWidget> SpellRowWidgetClass;

private:
    void AddSpellPair(const FSpellDefinition &FirstSpell, const FSpellDefinition *SecondSpell);
    void SortSpellsByVocation(EVocation Vocation, TArray<FSpellDefinition> &OutSpells);
};