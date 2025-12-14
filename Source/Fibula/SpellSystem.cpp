#include "SpellSystem.h"
#include "FibulaCharacter.h"
#include "SpellEffectDisplay.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "SpellDatabase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FibulaHUD.h"
#include "GameFormulas.h"
#include "DamageDisplayUtils.h"
#include "MagicWall.h"
#include "NiagaraFunctionLibrary.h"
#include "SpellProjectile.h"
#include "SoundSettingsManager.h"

ASpellSystem::ASpellSystem()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bAlwaysRelevant = true;
    SetReplicateMovement(false);
}

void ASpellSystem::ServerTryExecuteSpell(AFibulaCharacter *Caster, const FString &SpellName, const FVector &TargetLocation)
{
    if (!HasAuthority() || !Caster)
    {
        return;
    }

    const FSpellDefinition *Spell = USpellDatabase::GetSpell(SpellName);
    if (!Spell)
    {
        return;
    }

    if (Caster->IsInProtectionZone() && Spell->SpellType != ESpellType::SupportSpell)
    {
        ShowFloatingMessage(Caster, TEXT("You cannot cast offensive spells while in protection zone."));
        return;
    }

    const FGameItem *RuneItem = UItemDatabase::GetItem(SpellName);
    bool bIsRuneSpell = RuneItem && RuneItem->ItemType == EItemType::Rune;

    if (!ValidateSpell(Caster, Spell, true))
    {
        return;
    }

    
    if (bIsRuneSpell && Spell->bMustBeTargeted)
    {
        AFibulaCharacter *Target = Caster->GetCurrentTarget();
        if (Target)
        {
            float Distance = FVector::Distance(Caster->GetActorLocation(), Target->GetActorLocation());
            if (Distance > Spell->MaxTargetDistance)
            {
                ShowFloatingMessage(Caster, TEXT("Target is too far away."));
                return;
            }
        }
    }

    
    if (bIsRuneSpell && Caster->GetItemCount(RuneItem->Name) <= 0)
    {
        ShowFloatingMessage(Caster, TEXT("You don't have any runes left."));
        return;
    }

    
    if (!Spell->Words.IsEmpty())
    {
        if (AFibulaGameState *GameState = GetWorld()->GetGameState<AFibulaGameState>())
        {
            if (AChatSystem *ChatSystem = GameState->GetChatSystem())
            {
                ChatSystem->MulticastBroadcastMessage(Caster->GetCharacterName(), Spell->Words);
            }
        }
    }

    bool bUseTargetLocation = !TargetLocation.IsZero() &&
                              (Spell->SpellType == ESpellType::AreaRune || Spell->SpellType == ESpellType::MagicWall);

    if (ExecuteSpellEffects(Caster, Spell, Spell->Name, bUseTargetLocation ? TargetLocation : FVector::ZeroVector))
    {
        Caster->ModifyMana(-Spell->ManaCost);
        Caster->SetGeneralExhaust(true);

        if (Spell->SpellType == ESpellType::TargetedSpell ||
            Spell->SpellType == ESpellType::InstantAreaSpell ||
            Spell->SpellType == ESpellType::TargetedRune ||
            Spell->SpellType == ESpellType::AreaRune ||
            Spell->SpellType == ESpellType::ProjectileSpell)
        {
            Caster->SetOffensiveExhaust(true);
        }

        if (bIsRuneSpell)
        {
            Caster->RemoveItem(SpellName, 1);
            ShowFloatingMessage(Caster, FString::Printf(TEXT("Using one of %d %ss."), Caster->GetItemCount(SpellName), *SpellName.ToLower()));
        }
    }
}

bool ASpellSystem::ValidateSpell(AFibulaCharacter *Caster, const FSpellDefinition *Spell, bool bCheckVocation)
{
    if (Caster->IsGenerallyExhausted() && !Spell->IsOffensiveSpell())
    {
        Caster->ClientShowExhaustedMessage();
        return false;
    }

    if (bCheckVocation && !Spell->AllowedVocations.Contains(Caster->GetVocation()))
    {
        ShowFloatingMessage(Caster, TEXT("You haven't learned this spell yet."));
        return false;
    }

    if (Caster->GetCurrentMana() < Spell->ManaCost)
    {
        ShowFloatingMessage(Caster, TEXT("Not enough mana."));
        return false;
    }

    if (Caster->bCantHeal && Spell->Element == ESpellElement::Healing)
    {
        ShowFloatingMessage(Caster, TEXT("You cannot cast healing spells."));
        return false;
    }

    if (Spell->bMustBeTargeted)
    {
        if (Spell->SpellType == ESpellType::TargetedHeal)
        {
            if (!Caster->GetHealingTarget())
            {
                ShowFloatingMessage(Caster, TEXT("You don't have a healing target."));
                return false;
            }
        }
        else if (!Caster->GetCurrentTarget())
        {
            ShowFloatingMessage(Caster, TEXT("You don't have a target."));
            return false;
        }

        AFibulaCharacter *Target = Caster->GetCurrentTarget();
        if (Target && Spell->MaxTargetDistance > 0.0f)
        {
            float Distance = FVector::Distance(Caster->GetActorLocation(), Target->GetActorLocation());
            if (Distance > Spell->MaxTargetDistance)
            {
                ShowFloatingMessage(Caster, TEXT("Target is too far away."));
                return false;
            }
        }
    }

    if (Spell->IsOffensiveSpell() && Caster->IsOffensivelyExhausted())
    {
        Caster->ClientShowExhaustedMessage();
        return false;
    }

    return true;
}

bool ASpellSystem::ExecuteSpellEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FString &Message, const FVector &OverrideLocation)
{
    if (!Caster || !Spell)
        return false;

    
    FVector SpellCenter = DetermineSpellCenter(Caster, Spell, OverrideLocation);

    
    if (!Spell->bIsSelfTarget && !HasLineOfSightToLocation(Caster, SpellCenter))
    {
        ShowFloatingMessage(Caster, TEXT("You cannot see the target."));
        return false;
    }

    switch (Spell->SpellType)
    {
    case ESpellType::InstantAreaSpell:
    {
        ApplyAreaSpellEffects(Caster, Spell, SpellCenter);
        USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(SpellCenter, Caster, Message);
        return true;
    }

    case ESpellType::AreaRune:
    {
        ApplyAreaSpellEffects(Caster, Spell, SpellCenter);
        USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(SpellCenter, Caster, Message);
        return true;
    }

    case ESpellType::TargetedSpell:
    {
        ApplyTargetedSpellEffects(Caster, Spell, SpellCenter);
        USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(SpellCenter, Caster, Message);
        return true;
    }

    case ESpellType::TargetedRune:
    {
        if (!Caster->GetCurrentTarget())
            return false;

        ApplyTargetedSpellEffects(Caster, Spell, SpellCenter);
        USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(SpellCenter, Caster, Message);
        return true;
    }

    case ESpellType::SupportSpell:
    case ESpellType::SupportRune:
    case ESpellType::Potion:
        ApplySupportEffects(Caster, Spell);
        USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(Caster->GetActorLocation(), Caster, Message);
        return true;

    case ESpellType::MagicWall:
    {
        
        FHitResult GroundHit;
        FCollisionQueryParams GroundQueryParams;
        GroundQueryParams.bTraceComplex = true;

        bool bHitGround = GetWorld()->LineTraceSingleByChannel(
            GroundHit,
            SpellCenter + FVector(0, 0, 100.0f),
            SpellCenter - FVector(0, 0, 100.0f),
            ECC_Visibility,
            GroundQueryParams);

        if (!bHitGround)
        {
            ShowFloatingMessage(Caster, TEXT("You can only place magic walls on solid ground."));
            return false;
        }

        
        FVector SpawnLocation = GroundHit.Location;

        
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

        TArray<AActor *> OverlappingActors;
        bool bLocationClear = true;

        UKismetSystemLibrary::BoxOverlapActors(
            GetWorld(),
            SpawnLocation + FVector(0, 0, 50.0f),
            FVector(40.0f, 40.0f, 90.0f),
            ObjectTypes,
            nullptr,
            TArray<AActor *>(),
            OverlappingActors);

        if (OverlappingActors.Num() > 0)
        {
            bLocationClear = false;
        }

        if (!bLocationClear)
        {
            ShowFloatingMessage(Caster, TEXT("There is no way."));
            return false;
        }

        
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Caster;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AMagicWall *Wall = GetWorld()->SpawnActor<AMagicWall>(
            AMagicWall::StaticClass(),
            SpawnLocation,
            Caster->GetActorRotation(),
            SpawnParams);

        if (Wall)
        {
            USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(SpellCenter, Caster, Message);
            return true;
        }
        return false;
    }

    case ESpellType::TargetedHeal:
    {
        if (AFibulaCharacter *Target = Caster->GetHealingTarget())
        {
            float Distance = FVector::Distance(Caster->GetActorLocation(), Target->GetActorLocation());
            if (Distance > Spell->MaxTargetDistance)
            {
                ShowFloatingMessage(Caster, TEXT("Target is too far away."));
                return false;
            }

            const int32 HealAmount = GameFormulas::CalculateSupportSpellEffect(
                Caster,
                Spell,
                GameMode);

            Target->ServerModifyHealth(HealAmount, Caster, Spell->GetDamageDisplayType());

            USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(Target->GetActorLocation(), Caster, Message);
            return true;
        }
        else
        {
            ShowFloatingMessage(Caster, TEXT("Select a player to heal."));
            return false;
        }
    }

    case ESpellType::InstantAreaHeal:
    {
        ApplyAreaHealingEffects(Caster, Spell, SpellCenter);
        USpellEffectDisplay::SpawnSpellEffectsToNearbyPlayers(SpellCenter, Caster, Message);
        return true;
    }

    case ESpellType::ProjectileSpell:
    {
        if (!Caster)
            return false;

        
        FVector StartLocation = Caster->GetActorLocation() + (Caster->GetActorForwardVector() * 50.0f) + FVector(0, 0, 60.0f);

        
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Caster;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        
        ASpellProjectile *Projectile = GetWorld()->SpawnActor<ASpellProjectile>(
            ASpellProjectile::StaticClass(),
            StartLocation,
            Caster->GetActorRotation(),
            SpawnParams);

        if (Projectile)
        {
            const float ProjectileSpeed = 3000.0f;
            Projectile->SetSpellName(Spell->Name);
            Projectile->SetCaster(Caster);
            Projectile->LaunchProjectile(ProjectileSpeed);
            return true;
        }

        return false;
    }

    default:
        return false;
    }
}

void ASpellSystem::ApplySupportEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell)
{
    if (!Caster || !Spell)
        return;

    
    ClearExistingEffect(Caster, Spell->Name);

    
    if (!ActiveEffectTimers.Contains(Caster))
    {
        ActiveEffectTimers.Add(Caster, TMap<FString, FTimerHandle>());
    }

    
    if (Spell->Effects.SpeedMultiplier != 1.0f)
    {
        if (UCharacterMovementComponent *MovementComponent = Caster->GetCharacterMovement())
        {
            
            float OriginalSpeed = Caster->GetBaseSpeed();

            
            Caster->SetSpeed(OriginalSpeed * Spell->Effects.SpeedMultiplier);

            if (Spell->Effects.Duration > 0.0f)
            {
                FTimerHandle &TimerHandle = ActiveEffectTimers[Caster].Add(Spell->Name);
                GetWorld()->GetTimerManager().SetTimer(
                    TimerHandle,
                    [this, Caster, OriginalSpeed, SpellName = Spell->Name]()
                    {
                        Caster->SetSpeed(OriginalSpeed);
                        ActiveEffectTimers[Caster].Remove(SpellName);
                    },
                    Spell->Effects.Duration,
                    false);
            }
        }
    }
    
    else if (Spell->Effects.bIsHealingEffect)
    {
        const int32 HealAmount = GameFormulas::CalculateSupportSpellEffect(
            Caster,
            Spell,
            GameMode);

        Caster->ServerModifyHealth(HealAmount, Caster, Spell->GetDamageDisplayType());
    }
    
    else if (Spell->SpellType == ESpellType::Potion)
    {
        const int32 ManaAmount = FMath::RandRange(
            static_cast<int32>(Spell->SpellFormula.MinMagicLevelRatio),
            static_cast<int32>(Spell->SpellFormula.MaxMagicLevelRatio));

        Caster->ModifyMana(ManaAmount);

        
        Caster->RemoveItem(Spell->Name, 1);
    }
    else if (Spell->Effects.bIsMagicShield)
    {
        
        Caster->EnableMagicShield(true);

        
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [Caster]()
            {
                Caster->EnableMagicShield(false);
            },
            Spell->Effects.Duration,
            false);
    }
    else if (Spell->Effects.bIsLight)
    {
        
        Caster->SetLight(true);

        
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [Caster]()
            {
                Caster->SetLight(false);
            },
            Spell->Effects.Duration,
            false);
    }

    
    if (Spell->Effects.MeleeSkillMultiplier != 1.0f ||
        Spell->Effects.DistanceSkillMultiplier != 1.0f ||
        Spell->Effects.MagicLevelMultiplier != 1.0f ||
        Spell->Effects.bBlocksHealing)
    {

        
        float OriginalMeleeSkillMultiplier = Caster->MeleeSkillMultiplier;
        float OriginalDistanceSkillMultiplier = Caster->DistanceSkillMultiplier;
        float OriginalMagicLevelMultiplier = Caster->MagicLevelMultiplier;

        
        if (Spell->Effects.MeleeSkillMultiplier != 1.0f)
            Caster->MeleeSkillMultiplier = Spell->Effects.MeleeSkillMultiplier;
        if (Spell->Effects.DistanceSkillMultiplier != 1.0f)
            Caster->DistanceSkillMultiplier = Spell->Effects.DistanceSkillMultiplier;
        if (Spell->Effects.MagicLevelMultiplier != 1.0f)
            Caster->MagicLevelMultiplier = Spell->Effects.MagicLevelMultiplier;

        if (Spell->Effects.bBlocksHealing)
            Caster->bCantHeal = true;

        
        Caster->SetStrengthened(true);

        if (Spell->Effects.Duration > 0.0f)
        {
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                TimerHandle,
                [Caster, OriginalMeleeSkillMultiplier, OriginalDistanceSkillMultiplier, OriginalMagicLevelMultiplier]()
                {
                    
                    Caster->MeleeSkillMultiplier = OriginalMeleeSkillMultiplier;
                    Caster->DistanceSkillMultiplier = OriginalDistanceSkillMultiplier;
                    Caster->MagicLevelMultiplier = OriginalMagicLevelMultiplier;
                    Caster->SetStrengthened(false);
                    Caster->bCantHeal = false;
                },
                Spell->Effects.Duration,
                false);
        }
    }
}

void ASpellSystem::ApplyHealingEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell)
{
    const int32 HealAmount = GameFormulas::CalculateSupportSpellEffect(
        Caster,
        Spell,
        GameMode);

    Caster->ServerModifyHealth(HealAmount, Caster, Spell->GetDamageDisplayType());
}

void ASpellSystem::ApplyTargetedSpellEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &SpellCenter)
{
    const float EffectRadius = 100.0f * FMath::Max(Spell->Area.GridSize.X, Spell->Area.GridSize.Y);

    TArray<AActor *> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        SpellCenter,
        EffectRadius,
        ObjectTypes,
        AFibulaCharacter::StaticClass(),
        TArray<AActor *>(),
        OverlappingActors);

    for (AActor *Actor : OverlappingActors)
    {
        if (AFibulaCharacter *Target = Cast<AFibulaCharacter>(Actor))
        {
            if (Target != Caster && IsValid(Target) && !Target->IsInProtectionZone())
            {
                
                FVector RelativePos = Target->GetActorLocation() - SpellCenter;
                RelativePos = Caster->GetActorRotation().UnrotateVector(RelativePos);

                
                const float GridCellSize = 100.0f;
                const float GridTolerance = 0.3f;

                float GridXFloat = (RelativePos.X / GridCellSize) + (Spell->Area.GridSize.X / 2);
                float GridYFloat = (RelativePos.Y / GridCellSize) + (Spell->Area.GridSize.Y / 2);

                bool bEffectApplied = false;
                for (float XOffset = -GridTolerance; XOffset <= GridTolerance && !bEffectApplied; XOffset += GridTolerance)
                {
                    for (float YOffset = -GridTolerance; YOffset <= GridTolerance && !bEffectApplied; YOffset += GridTolerance)
                    {
                        int32 GridX = FMath::FloorToInt(GridXFloat + XOffset);
                        int32 GridY = FMath::FloorToInt(GridYFloat + YOffset);

                        if (GridX >= 0 && GridX < Spell->Area.AreaGrid.Num() &&
                            GridY >= 0 && GridY < Spell->Area.AreaGrid[0].Num() &&
                            Spell->Area.AreaGrid[GridX][GridY] > 0)
                        {
                            
                            const int32 Effect = -GameFormulas::CalculateSpellEffect(
                                Caster,
                                Target,
                                Spell);

                            Target->ServerModifyHealth(Effect, Caster, Spell->GetDamageDisplayType());

                            
                            if (Spell->Effects.SpeedMultiplier != 1.0f && Spell->Effects.Duration > 0.0f)
                            {
                                float OriginalSpeed = Target->GetBaseSpeed();
                                Target->SetSpeed(OriginalSpeed * (1.0f + Spell->Effects.SpeedMultiplier));

                                
                                if (!ActiveEffectTimers.Contains(Target))
                                {
                                    ActiveEffectTimers.Add(Target, TMap<FString, FTimerHandle>());
                                }

                                
                                FTimerHandle &TimerHandle = ActiveEffectTimers[Target].Add(Spell->Name);
                                GetWorld()->GetTimerManager().SetTimer(
                                    TimerHandle,
                                    [this, Target, OriginalSpeed, SpellName = Spell->Name]()
                                    {
                                        Target->SetSpeed(OriginalSpeed);
                                        ActiveEffectTimers[Target].Remove(SpellName);
                                    },
                                    Spell->Effects.Duration,
                                    false);
                            }
                            bEffectApplied = true;
                        }
                    }
                }
            }
        }
    }
}

void ASpellSystem::ApplyAreaSpellEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &SpellCenter)
{
    
    TArray<AActor *> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    const float EffectRadius = (100.0f * FMath::Max(Spell->Area.GridSize.X, Spell->Area.GridSize.Y)) * 1.1f;

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        SpellCenter,
        EffectRadius,
        ObjectTypes,
        AFibulaCharacter::StaticClass(),
        TArray<AActor *>(),
        OverlappingActors);

    
    int32 CasterGridX = -1;
    int32 CasterGridY = -1;
    for (int32 X = 0; X < Spell->Area.AreaGrid.Num(); ++X)
    {
        for (int32 Y = 0; Y < Spell->Area.AreaGrid[X].Num(); ++Y)
        {
            if (Spell->Area.AreaGrid[X][Y] == 3)
            {
                CasterGridX = X;
                CasterGridY = Y;
                break;
            }
        }
        if (CasterGridX != -1)
            break;
    }

    for (AActor *Actor : OverlappingActors)
    {
        if (AFibulaCharacter *Target = Cast<AFibulaCharacter>(Actor))
        {
            if (Target != Caster && IsValid(Target) && !Target->IsInProtectionZone())
            {
                
                FVector RelativePos = Target->GetActorLocation() - SpellCenter;

                
                RelativePos = Caster->GetActorRotation().UnrotateVector(RelativePos);

                
                const float GridCellSize = 100.0f;
                const float GridTolerance = 0.3f; 

                
                float GridXFloat = (RelativePos.X / GridCellSize) + CasterGridX;
                float GridYFloat = (RelativePos.Y / GridCellSize) + CasterGridY;

                bool bEffectApplied = false;
                
                for (float XOffset = -GridTolerance; XOffset <= GridTolerance && !bEffectApplied; XOffset += GridTolerance)
                {
                    for (float YOffset = -GridTolerance; YOffset <= GridTolerance && !bEffectApplied; YOffset += GridTolerance)
                    {
                        int32 GridX = FMath::FloorToInt(GridXFloat + XOffset);
                        int32 GridY = FMath::FloorToInt(GridYFloat + YOffset);

                        
                        if (GridX >= 0 && GridX < Spell->Area.AreaGrid.Num() &&
                            GridY >= 0 && GridY < Spell->Area.AreaGrid[0].Num() &&
                            Spell->Area.AreaGrid[GridX][GridY] > 0)
                        {
                            const int32 Effect = -GameFormulas::CalculateSpellEffect(
                                Caster,
                                Target,
                                Spell);

                            Target->ServerModifyHealth(Effect, Caster, Spell->GetDamageDisplayType());
                            
                            if (Spell->Effects.SpeedMultiplier != 1.0f && Spell->Effects.Duration > 0.0f)
                            {
                                float OriginalSpeed = Target->GetBaseSpeed();
                                Target->SetSpeed(OriginalSpeed * (1.0f + Spell->Effects.SpeedMultiplier));

                                
                                if (!ActiveEffectTimers.Contains(Target))
                                {
                                    ActiveEffectTimers.Add(Target, TMap<FString, FTimerHandle>());
                                }

                                
                                FTimerHandle &TimerHandle = ActiveEffectTimers[Target].Add(Spell->Name);
                                GetWorld()->GetTimerManager().SetTimer(
                                    TimerHandle,
                                    [this, Target, OriginalSpeed, SpellName = Spell->Name]()
                                    {
                                        Target->SetSpeed(OriginalSpeed);
                                        ActiveEffectTimers[Target].Remove(SpellName);
                                    },
                                    Spell->Effects.Duration,
                                    false);
                            }
                            bEffectApplied = true;
                        }
                    }
                }
            }
        }
    }
}

void ASpellSystem::ApplyAreaHealingEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &SpellCenter)
{
    
    TArray<AActor *> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    const float EffectRadius = 100.0f * FMath::Max(Spell->Area.GridSize.X, Spell->Area.GridSize.Y);

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        SpellCenter,
        EffectRadius,
        ObjectTypes,
        AFibulaCharacter::StaticClass(),
        TArray<AActor *>(),
        OverlappingActors);

    
    int32 CasterGridX = -1;
    int32 CasterGridY = -1;
    for (int32 X = 0; X < Spell->Area.AreaGrid.Num(); ++X)
    {
        for (int32 Y = 0; Y < Spell->Area.AreaGrid[X].Num(); ++Y)
        {
            if (Spell->Area.AreaGrid[X][Y] == 3)
            {
                CasterGridX = X;
                CasterGridY = Y;
                break;
            }
        }
        if (CasterGridX != -1)
            break;
    }

    for (AActor *Actor : OverlappingActors)
    {
        if (AFibulaCharacter *Target = Cast<AFibulaCharacter>(Actor))
        {
            if (IsValid(Target) && !Target->IsInProtectionZone())
            {
                
                FVector RelativePos = Target->GetActorLocation() - SpellCenter;

                
                RelativePos = Caster->GetActorRotation().UnrotateVector(RelativePos);

                
                const float GridCellSize = 100.0f;
                const float GridTolerance = 0.3f;

                float GridXFloat = (RelativePos.X / GridCellSize) + CasterGridX;
                float GridYFloat = (RelativePos.Y / GridCellSize) + CasterGridY;

                bool bEffectApplied = false;
                for (float XOffset = -GridTolerance; XOffset <= GridTolerance && !bEffectApplied; XOffset += GridTolerance)
                {
                    for (float YOffset = -GridTolerance; YOffset <= GridTolerance && !bEffectApplied; YOffset += GridTolerance)
                    {
                        int32 GridX = FMath::FloorToInt(GridXFloat + XOffset);
                        int32 GridY = FMath::FloorToInt(GridYFloat + YOffset);

                        if (GridX >= 0 && GridX < Spell->Area.AreaGrid.Num() &&
                            GridY >= 0 && GridY < Spell->Area.AreaGrid[0].Num() &&
                            Spell->Area.AreaGrid[GridX][GridY] > 0)
                        {
                            const int32 HealAmount = GameFormulas::CalculateSupportSpellEffect(
                                Caster,
                                Spell,
                                GameMode);

                            Target->ServerModifyHealth(HealAmount, Caster, Spell->GetDamageDisplayType());
                            bEffectApplied = true;
                        }
                    }
                }
            }
        }
    }
}

void ASpellSystem::ShowFloatingMessage(AFibulaCharacter *Caster, const FString &Message)
{
    if (Caster)
    {
        Caster->ShowCenteredMessage(Message, 3.0f, EFloatingTextType::MinorInfo);
    }
}

bool ASpellSystem::HasLineOfSightToLocation(const AFibulaCharacter *Caster, const FVector &TargetLocation) const
{
    if (!Caster)
        return false;

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    QueryParams.AddIgnoredActor(Caster);

    const float CharacterRadius = 40.0f;
    const TArray<FVector> StartOffsets = {
        FVector(0, 0, 50.0f),                // Eye level
        FVector(CharacterRadius, 0, 50.0f),  // Right
        FVector(-CharacterRadius, 0, 50.0f), // Left
        FVector(0, CharacterRadius, 50.0f),  // Front
        FVector(0, -CharacterRadius, 50.0f), // Back
        FVector(0, 0, 100.0f)                // Above head
    };

    const TArray<float> TargetHeights = {
        5.0f,  // Ground level
        50.0f, // Mid height
        100.0f // Full height
    };

    for (const FVector &StartOffset : StartOffsets)
    {
        FVector StartTrace = Caster->GetActorLocation() + StartOffset;

        for (float TargetHeight : TargetHeights)
        {
            FVector EndTrace = TargetLocation + FVector(0, 0, TargetHeight);

            FHitResult HitResult;
            bool bHit = GetWorld()->LineTraceSingleByChannel(
                HitResult,
                StartTrace,
                EndTrace,
                ECollisionChannel::ECC_GameTraceChannel3, // SpellTrace channel
                QueryParams);

            if (!bHit || HitResult.Location.Z - TargetLocation.Z < 20.0f)
            {
                return true;
            }
        }
    }

    return false;
}

FVector ASpellSystem::DetermineSpellCenter(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &OverrideLocation)
{
    if (!Caster || !Spell)
        return FVector::ZeroVector;

    if (!OverrideLocation.IsZero() &&
        (Spell->SpellType == ESpellType::AreaRune || Spell->SpellType == ESpellType::MagicWall))
    {
        return OverrideLocation;
    }

    
    if (Spell->bIsSelfTarget || Spell->SpellType == ESpellType::InstantAreaSpell)
    {
        return Caster->GetActorLocation();
    }

    
    if (Spell->SpellType == ESpellType::TargetedSpell || Spell->SpellType == ESpellType::TargetedRune)
    {
        if (AFibulaCharacter *Target = Caster->GetCurrentTarget())
        {
            float Distance = FVector::Distance(Caster->GetActorLocation(), Target->GetActorLocation());
            if (Distance <= Spell->MaxTargetDistance)
            {
                return Target->GetActorLocation();
            }
        }
    }

    
    return Caster->GetActorLocation() + Caster->GetActorForwardVector() * 100.0f;
}

void ASpellSystem::OnProjectileSpellHit(const FString &SpellName, AFibulaCharacter *Caster, const FVector &TargetLocation)
{
    const FSpellDefinition *Spell = USpellDatabase::GetSpell(SpellName);
    if (!Spell || !Caster)
        return;

    TArray<AActor *> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        TargetLocation,
        300.0f,
        ObjectTypes,
        AFibulaCharacter::StaticClass(),
        TArray<AActor *>(),
        OverlappingActors);

    for (AActor *Actor : OverlappingActors)
    {
        if (AFibulaCharacter *Target = Cast<AFibulaCharacter>(Actor))
        {
            if (Target != Caster && IsValid(Target) && !Target->IsInProtectionZone())
            {
                const int32 Effect = -GameFormulas::CalculateSpellEffect(
                    Caster,
                    Target,
                    Spell);

                Target->ServerModifyHealth(Effect, Caster, Spell->GetDamageDisplayType());
            }
        }
    }
}

FString FSpellDefinition::GetDetailedDescription(const AFibulaCharacter *Character) const
{
    FString DetailedDesc;

    
    if (ManaCost > 0)
    {
        DetailedDesc += FString::Printf(TEXT("%d mana\n"), ManaCost);
    }

    
    if (MaxTargetDistance > 0)
    {
        DetailedDesc += FString::Printf(TEXT("Range: %d meters\n"), FMath::RoundToInt(MaxTargetDistance / 100.0f));
    }

    
    if (Character && (SpellType == ESpellType::TargetedSpell ||
                      SpellType == ESpellType::InstantAreaSpell ||
                      SpellType == ESpellType::TargetedRune ||
                      SpellType == ESpellType::AreaRune ||
                      SpellType == ESpellType::ProjectileSpell))
    {
        const int32 MinDamage = GameFormulas::CalculateSpellMinDamage(Character, this);
        const int32 MaxDamage = GameFormulas::CalculateSpellMaxDamage(Character, this);
        DetailedDesc += FString::Printf(TEXT("%d - %d damage\n"), MinDamage, MaxDamage);
    }
    
    else if (Character && Element == ESpellElement::Healing)
    {
        const float HealingMultiplier = GameFormulas::GameModeHealingMultiplier(EGameModeType::TeamBattle, Character->GetVocation());
        const int32 MinHeal = FMath::RoundToInt(GameFormulas::CalculateSpellMinDamage(Character, this) * HealingMultiplier);
        const int32 MaxHeal = FMath::RoundToInt(GameFormulas::CalculateSpellMaxDamage(Character, this) * HealingMultiplier);
        DetailedDesc += FString::Printf(TEXT("%d - %d healing\n"), MinHeal, MaxHeal);
    }

    
    if (!Description.IsEmpty())
    {
        DetailedDesc += TEXT("\n") + Description;
    }

    return DetailedDesc;
}

void ASpellSystem::ClearExistingEffect(AFibulaCharacter *Caster, const FString &SpellName)
{
    if (!Caster || !ActiveEffectTimers.Contains(Caster))
        return;

    if (ActiveEffectTimers[Caster].Contains(SpellName))
    {
        
        GetWorld()->GetTimerManager().ClearTimer(ActiveEffectTimers[Caster][SpellName]);
        ActiveEffectTimers[Caster].Remove(SpellName);

        
        const FSpellDefinition *Spell = USpellDatabase::GetSpell(SpellName);
        if (!Spell)
            return;

        if (Spell->Effects.SpeedMultiplier != 1.0f)
        {
            float BaseSpeed = GameFormulas::CalculateSpeed(Caster->GetCharacterLevel(), Caster->GetVocation());
            Caster->SetSpeed(BaseSpeed);
        }

        if (Spell->Effects.MeleeSkillMultiplier != 1.0f)
            Caster->MeleeSkillMultiplier = 1.0f;

        if (Spell->Effects.DistanceSkillMultiplier != 1.0f)
            Caster->DistanceSkillMultiplier = 1.0f;

        if (Spell->Effects.MagicLevelMultiplier != 1.0f)
            Caster->MagicLevelMultiplier = 1.0f;

        if (Spell->Effects.bIsMagicShield)
            Caster->EnableMagicShield(false);

        if (Spell->Effects.bIsLight)
            Caster->SetLight(false);

        Caster->SetStrengthened(false);
    }
}

void ASpellSystem::BeginDestroy()
{
    
    for (auto &CharacterEffects : ActiveEffectTimers)
    {
        for (auto &Effect : CharacterEffects.Value)
        {
            GetWorld()->GetTimerManager().ClearTimer(Effect.Value);
        }
    }
    ActiveEffectTimers.Empty();

    Super::BeginDestroy();
}

void ASpellSystem::BeginPlay()
{
    Super::BeginPlay();

    if (AFibulaGameState *GameState = GetWorld()->GetGameState<AFibulaGameState>())
    {
        GameMode = GameState->GetCurrentGameMode();
    }
    else
    {
        GameMode = EGameModeType::TeamBattle;
    }
}