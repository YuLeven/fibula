#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "SpellProjectile.generated.h"

class AFibulaCharacter;

UCLASS()
class ASpellProjectile : public AActor
{
    GENERATED_BODY()

public:
    ASpellProjectile();

    UPROPERTY(EditAnywhere, Category = "Effects")
    UProjectileMovementComponent *ProjectileMovement;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraComponent *VisualEffect;

    void LaunchProjectile(float Speed);
    void SetSpellName(const FString &InSpellName) { SpellName = InSpellName; }
    void SetCaster(AFibulaCharacter *InCaster) { Caster = InCaster; }
    virtual void Tick(float DeltaTime) override;

protected:
    void PlayImpactEffect(const FVector &ImpactLocation);
    void HandleProjectileEnd();
    FVector GetImpactLocation() const;

    UPROPERTY(ReplicatedUsing = OnRep_SpellName)
    FString SpellName;

    UFUNCTION()
    void OnRep_SpellName();

    void SetupVisualEffects();

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY()
    UAudioComponent *HomingSoundComponent;

    void SetupHomingSound();
    void StopHomingSound();

private:
    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem *ImpactEffect;

    UPROPERTY(VisibleAnywhere)
    USphereComponent *CollisionComponent;

    UPROPERTY()
    AFibulaCharacter *Caster;
};