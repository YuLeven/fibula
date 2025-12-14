#include "SpellEffectDisplay.h"
#include "FibulaCharacter.h"
#include "SpellProjectileAnimationActor.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SpellSystem.h"
#include "GameplayUtils.h"

void USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(const FVector &Center, AFibulaCharacter *Caster, const FString &SpellWords)
{
    TArray<AFibulaCharacter *> NearbyPlayers = UGameplayUtils::GetPlayersWithinRadius(Caster, Center, 2500.0f);
    for (AFibulaCharacter *Player : NearbyPlayers)
    {
        Player->ClientSpawnSpellEffects(Center, Caster, SpellWords);
    }
}

void USpellEffectDisplay::SpawnSpellEffects(UWorld *World, AFibulaCharacter *Caster, const FVector &Center, const FRotator &Rotation, const FSpellDefinition *Spell)
{
    if (!Spell)
    {
        return;
    }

    
    if (Spell->NiagaraHomingEffect)
    {
        FVector StartLocation = Caster->GetActorLocation() + FVector(0, 0, 60.0f);
        FVector EndLocation = Center;

        
        FVector Direction = EndLocation - StartLocation;
        float Distance = Direction.Size();
        Direction.Normalize();

        
        float ProjectileSpeed = 4000.0f;
        float Lifetime = Distance / ProjectileSpeed;

        
        FRotator ProjectileRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

        
        if (Spell->NiagaraHomingEffect)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            ASpellProjectileAnimationActor *Projectile = World->SpawnActor<ASpellProjectileAnimationActor>(
                ASpellProjectileAnimationActor::StaticClass(),
                StartLocation,
                ProjectileRotation,
                SpawnParams);

            if (Projectile)
            {
                UE_LOG(LogTemp, Log, TEXT("Spawned Niagara homing effect"));
                UE_LOG(LogTemp, Log, TEXT("Homing effect end location: %s"), *EndLocation.ToString());

                
                Projectile->SetSpell(Spell);
                Projectile->VisualEffect->SetAsset(Spell->NiagaraHomingEffect);
                Projectile->InitializeProjectile(StartLocation, EndLocation, ProjectileSpeed, Spell->NiagaraEffect);
                return;
            }
        }
    }
    else
    {
        SpawnMainEffect(World, Center, Rotation, Spell);
    }
}

void USpellEffectDisplay::SpawnMainEffect(UWorld *World, const FVector &Center, const FRotator &Rotation, const FSpellDefinition *Spell)
{
    if (!Spell || (!Spell->CascadeEffect && !Spell->NiagaraEffect))
    {
        return;
    }

    
    if (Spell->bSpawnSingleEffect)
    {
        
        if (Spell->NiagaraEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World,
                Spell->NiagaraEffect,
                Center,
                Rotation);
        }
        
        else if (Spell->CascadeEffect)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                World,
                Spell->CascadeEffect,
                Center,
                Rotation,
                FVector(1.0f),
                true,
                EPSCPoolMethod::AutoRelease);
        }
        return;
    }

    const float GridCellSize = 100.0f;

    
    int32 CasterX = -1;
    int32 CasterY = -1;
    for (int32 X = 0; X < Spell->Area.AreaGrid.Num(); ++X)
    {
        for (int32 Y = 0; Y < Spell->Area.AreaGrid[X].Num(); ++Y)
        {
            if (Spell->Area.AreaGrid[X][Y] == 3)
            {
                CasterX = X;
                CasterY = Y;
                break;
            }
        }
        if (CasterX != -1)
            break;
    }

    for (int32 X = 0; X < Spell->Area.AreaGrid.Num(); ++X)
    {
        for (int32 Y = 0; Y < Spell->Area.AreaGrid[X].Num(); ++Y)
        {
            if (Spell->Area.AreaGrid[X][Y] > 0)
            {
                float OffsetX = (X - CasterX) * GridCellSize;
                float OffsetY = (Y - CasterY) * GridCellSize;

                FVector RotatedOffset = Rotation.RotateVector(FVector(OffsetX, OffsetY, 0.0f));
                FVector EffectLocation = Center + RotatedOffset;

                FHitResult HitResult;
                FCollisionQueryParams QueryParams;
                QueryParams.bTraceComplex = true;

                bool bHasLineOfSight = !World->LineTraceSingleByChannel(
                    HitResult,
                    Center,
                    EffectLocation,
                    ECC_Visibility,
                    QueryParams);

                if (bHasLineOfSight)
                {
                    
                    if (Spell->NiagaraEffect)
                    {
                        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                            World,
                            Spell->NiagaraEffect,
                            EffectLocation,
                            Rotation);
                    }
                    
                    else if (Spell->CascadeEffect)
                    {
                        UGameplayStatics::SpawnEmitterAtLocation(
                            World,
                            Spell->CascadeEffect,
                            EffectLocation,
                            Rotation,
                            FVector(1.0f),
                            true,
                            EPSCPoolMethod::AutoRelease);
                    }
                }
            }
        }
    }
}