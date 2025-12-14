#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "GameItem.h"
#include "SpellSystem.h"
#include "ItemDragDropOperation.generated.h"

UCLASS()
class FIBULA_API UItemDragDropOperation : public UDragDropOperation
{
    GENERATED_BODY()

public:
    UPROPERTY()
    class UGameSlot *SourceSlot;

    UPROPERTY()
    FGameItem DraggedItem;

    UPROPERTY()
    FSpellDefinition DraggedSpell;
};