#include "SpellProjectileAnimationActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "SpellEffectDisplay.h"
#include "SpellSystem.h"
#include "SoundSettingsManager.h"
#include "Kismet/GameplayStatics.h"

ASpellProjectileAnimationActor::ASpellProjectileAnimationActor()
{
    PrimaryActorTick.bCanEverTick = false;

    
    bReplicates = false;
    bAlwaysRelevant = false;
    bNetLoadOnClient = false;

    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bInitialVelocityInLocalSpace = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    ProjectileMovement->OnProjectileStop.AddDynamic(this, &ASpellProjectileAnimationActor::OnProjectileStop);

    
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingAccelerationMagnitude = 10000.0f;

    
    VisualEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VisualEffect"));
    VisualEffect->SetupAttachment(RootComponent);

    HomingTargetActor = nullptr;

    
    HomingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HomingSoundComponent"));
    HomingSoundComponent->bAutoActivate = false;
    HomingSoundComponent->SetupAttachment(RootComponent);
}

void ASpellProjectileAnimationActor::InitializeProjectile(const FVector &StartLocation, const FVector &RequestedTargetLocation,
                                                          float Speed, UNiagaraSystem *HitImpactEffect)
{
    SetActorLocation(StartLocation);
    TargetLocation = RequestedTargetLocation;
    ImpactEffect = HitImpactEffect;

    FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
    ProjectileMovement->Velocity = Direction * Speed;

    
    float Distance = FVector::Distance(StartLocation, RequestedTargetLocation);
    float TimeToReachTarget = (Distance / Speed) + 0.1f; 

    FTimerHandle DestroyTimerHandle;
    GetWorldTimerManager().SetTimer(
        DestroyTimerHandle,
        this,
        &ASpellProjectileAnimationActor::OnReachTarget,
        TimeToReachTarget,
        false);

    if (Spell && Spell->HomingSound)
    {
        HomingSoundComponent->SetSound(Spell->HomingSound);
        HomingSoundComponent->Play();
    }
}

void ASpellProjectileAnimationActor::SetHomingTarget(AActor *Target)
{
    if (Target)
    {
        HomingTargetActor = Target;
        ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
    }
}

void ASpellProjectileAnimationActor::OnProjectileStop(const FHitResult &ImpactResult)
{
    HandleProjectileEnd();
}

void ASpellProjectileAnimationActor::HandleProjectileEnd()
{
    if (HomingSoundComponent)
    {
        HomingSoundComponent->Stop();
    }

    if (VisualEffect)
    {
        VisualEffect->Deactivate();
    }

    
    if (Spell && Spell->HitSound)
    {
        USoundSettingsManager *Settings = USoundSettingsManager::LoadSettings();
        if (Settings)
        {
            float FinalVolume = Settings->MasterVolume * Settings->SoundEffectsVolume;
            UGameplayStatics::PlaySoundAtLocation(
                this,
                Spell->HitSound,
                GetActorLocation(),
                FinalVolume);
        }
    }

    PlayImpactEffect(GetImpactLocation());
    Destroy();
}

void ASpellProjectileAnimationActor::PlayImpactEffect(const FVector &ImpactLocation)
{
    if (Spell && Spell->NiagaraEffect)
    {
        USpellEffectDisplay::SpawnMainEffect(GetWorld(), ImpactLocation, GetActorRotation(), Spell);
    }
    else if (ImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ImpactEffect,
            ImpactLocation,
            GetActorRotation());
    }
}

void ASpellProjectileAnimationActor::OnReachTarget()
{
    if (!IsValid(this))
    {
        return;
    }

    if (HomingTargetActor && IsValid(HomingTargetActor))
    {
        float Distance = FVector::Distance(GetActorLocation(), HomingTargetActor->GetActorLocation());
        if (Distance <= IMPACT_THRESHOLD)
        {
            HandleProjectileEnd();
            return;
        }
    }

    HandleProjectileEnd();
}

FVector ASpellProjectileAnimationActor::GetImpactLocation() const
{
    if (HomingTargetActor != nullptr && IsValid(HomingTargetActor))
    {
        return HomingTargetActor->GetActorLocation();
    }
    return !TargetLocation.IsZero() ? TargetLocation : GetActorLocation();
}
