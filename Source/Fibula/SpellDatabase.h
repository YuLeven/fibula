#pragma once

#include "CoreMinimal.h"
#include "SpellSystem.h"
#include "Particles/ParticleSystem.h"
#include "SpellDatabase.generated.h"

UCLASS()
class FIBULA_API USpellDatabase : public UObject
{
    GENERATED_BODY()

public:
    static void Initialize();
    static const FSpellDefinition *GetSpell(const FString &Key);
    static const FSpellDefinition *GetSpellByWords(const FString &Words);
    static const TMap<FString, FSpellDefinition> &GetAllSpells(const EVocation Vocation);

private:
    static void RegisterSpell(const FSpellDefinition &SpellDef);
    static TMap<FString, FSpellDefinition> Spells;
};