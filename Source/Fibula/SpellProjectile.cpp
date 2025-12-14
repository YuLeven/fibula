#include "SpellProjectile.h"
#include "FibulaCharacter.h"
#include "SpellSystem.h"
#include "GameFramework/GameModeBase.h"
#include "NiagaraFunctionLibrary.h"
#include "SpellDatabase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FibulaGameState.h"
#include "SoundSettingsManager.h"
#include "Kismet/GameplayStatics.h"

ASpellProjectile::ASpellProjectile()
{
    
    bReplicates = true;
    bAlwaysRelevant = true;
    bNetLoadOnClient = true;

    
    PrimaryActorTick.bCanEverTick = true;

    
    SetReplicateMovement(true);

    
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(15.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    CollisionComponent->SetSimulatePhysics(false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RootComponent = CollisionComponent;

    
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = false;
    ProjectileMovement->bInitialVelocityInLocalSpace = true;
    ProjectileMovement->ProjectileGravityScale = 0.05f;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.3f;
    ProjectileMovement->Friction = 0.4f;
    ProjectileMovement->BounceVelocityStopSimulatingThreshold = 0.0f;
    ProjectileMovement->MaxSimulationTimeStep = 0.016667f;
    ProjectileMovement->MaxSimulationIterations = 8;

    
    VisualEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VisualEffect"));
    VisualEffect->SetupAttachment(RootComponent);
    VisualEffect->bAutoActivate = true;

    
    InitialLifeSpan = 4.0f;
}

void ASpellProjectile::LaunchProjectile(float Speed)
{
    if (ProjectileMovement)
    {
        
        ProjectileMovement->Velocity = GetActorForwardVector() * Speed;
    }
}

void ASpellProjectile::HandleProjectileEnd()
{
    
    if (GetLocalRole() == ROLE_Authority)
    {
        if (AFibulaGameState *GameState = GetWorld()->GetGameState<AFibulaGameState>())
        {
            if (ASpellSystem *SpellSystem = GameState->GetSpellSystem())
            {
                SpellSystem->OnProjectileSpellHit(SpellName, Caster, GetActorLocation());
            }
        }
    }
}

void ASpellProjectile::PlayImpactEffect(const FVector &ImpactLocation)
{
    if (ImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ImpactEffect,
            ImpactLocation,
            GetActorRotation());
    }
}

FVector ASpellProjectile::GetImpactLocation() const
{
    return GetActorLocation();
}

void ASpellProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
    if (ProjectileMovement && !ProjectileMovement->Velocity.IsNearlyZero())
    {
        FVector Velocity = ProjectileMovement->Velocity;

        
        Velocity.Z = 0;

        
        if (!Velocity.IsNearlyZero())
        {
            FRotator NewRotation = Velocity.GetSafeNormal().Rotation();
            
            NewRotation.Pitch = 0.0f;
            SetActorRotation(NewRotation);
        }
    }

    
    if (HasAuthority())
    {
        const float ProximityThreshold = 100.0f; 
        TArray<AActor *> OverlappingActors;
        FVector Location = GetActorLocation();

        
        UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(),
            Location,
            ProximityThreshold,
            TArray<TEnumAsByte<EObjectTypeQuery>>(),
            AFibulaCharacter::StaticClass(),
            TArray<AActor *>(),
            OverlappingActors);

        
        for (AActor *Actor : OverlappingActors)
        {
            if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(Actor))
            {
                if (Character != Caster) 
                {
                    Destroy();
                    break;
                }
            }
        }
    }
}

void ASpellProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASpellProjectile, SpellName);
}

void ASpellProjectile::OnRep_SpellName()
{
    SetupVisualEffects();
}

void ASpellProjectile::SetupVisualEffects()
{
    const FSpellDefinition *Spell = USpellDatabase::GetSpell(SpellName);
    if (Spell)
    {
        if (Spell->NiagaraHomingEffect)
        {
            VisualEffect->SetAsset(Spell->NiagaraHomingEffect);
            ImpactEffect = Spell->NiagaraEffect;
        }

        
        if (Spell->HomingSound)
        {
            HomingSoundComponent->SetSound(Spell->HomingSound);
            HomingSoundComponent->Play();
        }
    }
}

void ASpellProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (HomingSoundComponent)
    {
        HomingSoundComponent->Stop();
    }

    
    PlayImpactEffect(GetActorLocation());

    
    const FSpellDefinition *Spell = USpellDatabase::GetSpell(SpellName);
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

    if (EndPlayReason == EEndPlayReason::Destroyed && GetLocalRole() == ROLE_Authority)
    {
        HandleProjectileEnd();
    }

    Super::EndPlay(EndPlayReason);
}