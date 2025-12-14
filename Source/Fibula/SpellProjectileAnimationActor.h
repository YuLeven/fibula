#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/AudioComponent.h"
#include "SpellProjectileAnimationActor.generated.h"

struct FSpellDefinition;

UCLASS()
class ASpellProjectileAnimationActor : public AActor
{
    GENERATED_BODY()

public:
    ASpellProjectileAnimationActor();

    void InitializeProjectile(const FVector &StartLocation, const FVector &RequestedTargetLocation,
                              float Speed, UNiagaraSystem *HitImpactEffect = nullptr);
    void SetHomingTarget(AActor *Target);
    void SetSpell(const FSpellDefinition *InSpell) { Spell = InSpell; }

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent *ProjectileMovement;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent *VisualEffect;

protected:
    UPROPERTY()
    UAudioComponent *HomingSoundComponent;

    void SetupHomingSound();
    void StopHomingSound();

private:
    UFUNCTION()
    virtual void OnProjectileStop(const FHitResult &ImpactResult);

    void HandleProjectileEnd();
    void PlayImpactEffect(const FVector &ImpactLocation);
    FVector GetImpactLocation() const;
    void OnReachTarget();

    const float IMPACT_THRESHOLD = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem *ImpactEffect;

    UPROPERTY()
    FVector TargetLocation;

    UPROPERTY()
    AActor *HomingTargetActor;

    const FSpellDefinition *Spell;
};