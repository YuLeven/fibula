#pragma once

#include "CoreMinimal.h"
#include "SpellEffectDisplay.generated.h"

class AFibulaCharacter;
class UNiagaraSystem;
class UParticleSystem;
struct FSpellDefinition;

UCLASS()
class FIBULA_API USpellEffectDisplay : public UObject
{
    GENERATED_BODY()

public:
    static void SpawnSpellEffectsToNearbyPlayers(const FVector &Center, AFibulaCharacter *Caster, const FString &SpellWords);
    static void SpawnSpellEffects(UWorld *World, AFibulaCharacter *Caster, const FVector &Center, const FRotator &Rotation, const FSpellDefinition *Spell);
    static void SpawnMainEffect(UWorld *World, const FVector &Center, const FRotator &Rotation, const FSpellDefinition *Spell);
};
