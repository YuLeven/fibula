#include "SpellDatabase.h"

TMap<FString, FSpellDefinition> USpellDatabase::Spells;

void USpellDatabase::Initialize()
{
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExplosionAsset(TEXT("/Game/Effects/NS_Explosion"));
    if (!ExplosionAsset.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> IceStrikeAsset(TEXT("/Game/Effects/NS_ky_waterBallHit.NS_ky_waterBallHit"));
    if (!IceStrikeAsset.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UParticleSystem> WarriorSwipeAsset(TEXT("/Game/Effects/P_Warrior_Shout"));
    if (!WarriorSwipeAsset.Succeeded())
    {
        return;
    }
    static ConstructorHelpers::FObjectFinder<UParticleSystem> EnergyStrikeAsset(TEXT("/Game/Effects/P_ky_ThunderBallHit"));
    if (!EnergyStrikeAsset.Succeeded())
    {
        return;
    }
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> MageFireballAsset(TEXT("/Game/Effects/NS_Mage_Fireball.NS_Mage_Fireball"));
    if (!MageFireballAsset.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FireSlashEffect(TEXT("/Game/Effects/N_FireSlash.N_FireSlash"));
    if (!FireSlashEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> MagicSparkles(TEXT("/Game/Effects/N_MagicSparkles.N_MagicSparkles"));
    if (!MagicSparkles.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SuddenDeathProjectile(TEXT("/Game/Effects/NS_SuddenDeathProjectile.NS_SuddenDeathProjectile"));
    if (!SuddenDeathProjectile.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BloodHitEffect(TEXT("/Game/Effects/NS_BloodHit.NS_BloodHit"));
    if (!BloodHitEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> GrandFireSlash(TEXT("/Game/Effects/NS_GrandFireSlash.NS_GrandFireSlash"));
    if (!GrandFireSlash.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> WaterSlashEffect(TEXT("/Game/Effects/NS_WaterSlash.NS_WaterSlash"));
    if (!WaterSlashEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> RapidSlicesEffect(TEXT("/Game/Effects/NS_RapidSlices.NS_RapidSlices"));
    if (!RapidSlicesEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> HolyArrowEffect(TEXT("/Game/Effects/NS_HolyArrow.NS_HolyArrow"));
    if (!HolyArrowEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> HolyArrowHitEffect(TEXT("/Game/Effects/NS_HolyArrowHit.NS_HolyArrowHit"));
    if (!HolyArrowHitEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> GalaxyArrowEffect(TEXT("/Game/Effects/NS_GalaxyArrow.NS_GalaxyArrow"));
    if (!GalaxyArrowEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> GalaxyArrowHitEffect(TEXT("/Game/Effects/NS_GalaxyArrowHit.NS_GalaxyArrowHit"));
    if (!GalaxyArrowHitEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ThrowingAxeEffect(TEXT("/Game/Effects/NS_Warrior_ThrowingAxe.NS_Warrior_ThrowingAxe"));
    if (!ThrowingAxeEffect.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> BerserkShoutSound(TEXT("/Game/SoundFX/berserk_shout_cue"));
    if (!BerserkShoutSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> ExplosionSound(TEXT("/Game/SoundFX/explosion_cue"));
    if (!ExplosionSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> FireMagicSound(TEXT("/Game/SoundFX/fire_magic_cue"));
    if (!FireMagicSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> SwordHitSound(TEXT("/Game/SoundFX/sword_hit_cue"));
    if (!SwordHitSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> WindGustSound(TEXT("/Game/SoundFX/wind_gust_cue"));
    if (!WindGustSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> KnightLightStrainSound(TEXT("/Game/SoundFX/knight_light_straining_cue"));
    if (!KnightLightStrainSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> KnightHeavyStrainSound(TEXT("/Game/SoundFX/knight_heavy_straining_cue"));
    if (!KnightHeavyStrainSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> KnightModerateStrainSound(TEXT("/Game/SoundFX/knight_moderate_straining_cue"));
    if (!KnightModerateStrainSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> KnightDashSound(TEXT("/Game/SoundFX/knight_dash_cue"));
    if (!KnightDashSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> LoudExplosionSound(TEXT("/Game/SoundFX/loud_explosion_cue"));
    if (!LoudExplosionSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> WooshSound(TEXT("/Game/SoundFX/woosh_cue"));
    if (!WooshSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> BowAndArrowSound(TEXT("/Game/SoundFX/bow_and_arrow_Cue"));
    if (!BowAndArrowSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> ThumpSound(TEXT("/Game/SoundFX/thump_Cue"));
    if (!ThumpSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> RotatingWooshSound(TEXT("/Game/SoundFX/rotating_woosh_Cue"));
    if (!RotatingWooshSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> RockRollSound(TEXT("/Game/SoundFX/rock_roll_Cue"));
    if (!RockRollSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> MagicWooshThumpSound(TEXT("/Game/SoundFX/magic_woosh_thump_Cue"));
    if (!MagicWooshThumpSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> SwordSwingSound(TEXT("/Game/SoundFX/sword_swing_Cue"));
    if (!SwordSwingSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> SdLaughSound(TEXT("/Game/SoundFX/sd_laugh_Cue"));
    if (!SdLaughSound.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> MagicLowWooshSound(TEXT("/Game/SoundFX/magic_low_woosh_Cue"));
    if (!MagicLowWooshSound.Succeeded())
    {
        return;
    }

    // Define vocations sets
    TSet<EVocation> SorcererOnly = {EVocation::Sorcerer};
    TSet<EVocation> DruidOnly = {EVocation::Druid};
    TSet<EVocation> MagesOnly = {EVocation::Sorcerer, EVocation::Druid};
    TSet<EVocation> KnightOnly = {EVocation::Knight};
    TSet<EVocation> PaladinOnly = {EVocation::Paladin};
    TSet<EVocation> MagesAndPaladins = {EVocation::Sorcerer, EVocation::Druid, EVocation::Paladin};
    TSet<EVocation> KnightsAndPaladins = {EVocation::Knight, EVocation::Paladin};
    TSet<EVocation> AllVocations = {EVocation::Sorcerer, EVocation::Druid, EVocation::Paladin, EVocation::Knight};

    // Define common area patterns
    const TArray<TArray<int32>> ExplosionArea = {
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}};

    const TArray<TArray<int32>> SmallExplosionArea = {
        {0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 3, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0}};

    const TArray<TArray<int32>> ConeArea = {
        {0, 0, 3, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}};

    const TArray<TArray<int32>> SingleTargetArea = {
        {0, 0, 0},
        {0, 3, 0},
        {0, 0, 0}};

    const TArray<TArray<int32>> MeleeArea = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 3, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}};

    const TArray<TArray<int32>> CrossArea = {
        {0, 1, 0},
        {1, 3, 1},
        {0, 1, 0}};

    // Define beam area pattern
    const TArray<TArray<int32>> BeamArea = {
        {0, 0, 3, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0}};

    // Ultimate Explosion
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Ultimate Explosion"),
        .Description = TEXT("The mage saturates the surrounding air with mana and ignites it producing a devastating explosion."),
        .Words = TEXT("exevo gran mas vis"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = ExplosionArea},
        .NiagaraEffect = ExplosionAsset.Object,
        .SpellSound = LoudExplosionSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 1.8f,
            .MaxMagicLevelRatio = 2.8f},
        .SpellType = ESpellType::InstantAreaSpell,
        .ManaCost = 1500,
        .IconPath = TEXT("/Game/HUDs/Images/ultimate_explosion_icon.ultimate_explosion_icon"),
        .AllowedVocations = SorcererOnly});

    // Flame Wave
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Flame Wave"),
        .Description = TEXT("Conjures a wave of fire that burns everything in its path."),
        .Words = TEXT("exevo flam hur"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = ConeArea},
        .NiagaraEffect = ExplosionAsset.Object,
        .SpellSound = FireMagicSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.6f,
            .MaxMagicLevelRatio = 2.2f},
        .SpellType = ESpellType::InstantAreaSpell,
        .Element = ESpellElement::Fire,
        .ManaCost = 120,
        .IconPath = TEXT("/Game/HUDs/Images/fire_wave_icon.fire_wave_icon"),
        .AllowedVocations = SorcererOnly});

    // Energy Wave
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Energy Wave"),
        .Description = TEXT("Releases a cone of pure energy that damages all creatures in its path."),
        .Words = TEXT("exevo mort hur"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = ConeArea},
        .CascadeEffect = EnergyStrikeAsset.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 1.0f,
            .MaxMagicLevelRatio = 2.0f},
        .SpellType = ESpellType::InstantAreaSpell,
        .Element = ESpellElement::Energy,
        .ManaCost = 170,
        .IconPath = TEXT("/Game/HUDs/Images/energy_wave_icon.energy_wave_icon"),
        .AllowedVocations = SorcererOnly});

    // Flame Strike
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Flame Strike"),
        .Description = TEXT("Unleashes a jolt of fire at a single target."),
        .Words = TEXT("exori flam"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 1200.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = ExplosionAsset.Object,
        .NiagaraHomingEffect = MageFireballAsset.Object,
        .SpellSound = FireMagicSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.70f,
            .MaxMagicLevelRatio = 1.00f},
        .SpellType = ESpellType::TargetedSpell,
        .Element = ESpellElement::Fire,
        .ManaCost = 40,
        .IconPath = TEXT("/Game/HUDs/Images/flame_strike_icon.flame_strike_icon"),
        .AllowedVocations = MagesOnly});

    // Ultimate Healing
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Ultimate Healing"),
        .Description = TEXT("Infuses the caster with a powerful healing force, instantly restoring a large amount of health."),
        .Words = TEXT("exura vita"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .bIsHealingEffect = true},
        .SpellFormula = {.MinMagicLevelRatio = 3.3f, .MaxMagicLevelRatio = 5.3f},
        .SpellType = ESpellType::SupportSpell,
        .Element = ESpellElement::Healing,
        .ManaCost = 100,
        .IconPath = TEXT("/Game/HUDs/Images/ultimate_healing_icon.ultimate_healing_icon"),
        .AllowedVocations = MagesAndPaladins});

    // Strong Haste
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Strong Haste"),
        .Description = TEXT("Channels mana through the body, greatly increasing the caster's movement speed."),
        .Words = TEXT("utani gran hur"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .SpellSound = MagicLowWooshSound.Object,
        .Effects = {
            .SpeedMultiplier = 1.7f,
            .Duration = 21.0f},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::SupportSpell,
        .ManaCost = 60,
        .IconPath = TEXT("/Game/HUDs/Images/strong_haste_icon.strong_haste_icon"),
        .AllowedVocations = MagesOnly});

    // Sudden Death Rune
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Sudden Death Rune"),
        .Description = TEXT("This rune is etched with a sigil of death, striking directly at the target's soul."),
        .Words = TEXT(""), // Runes don't have magic words
        .bIsSelfTarget = false,
        .bMustBeTargeted = true,
        .MaxTargetDistance = 1200.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = BloodHitEffect.Object,
        .NiagaraHomingEffect = SuddenDeathProjectile.Object,
        .HomingSound = SdLaughSound.Object,
        .HitSound = MagicWooshThumpSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 1.3f,
            .MaxMagicLevelRatio = 1.7f},
        .SpellType = ESpellType::TargetedRune,
        .ManaCost = 0,
        .IconPath = TEXT("/Game/HUDs/Images/sudden_death_rune_icon.sudden_death_rune_icon"),
        .AllowedVocations = MagesAndPaladins}); // Anyone can use runes

    // Mana Fluid (Potion)
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Mana Fluid"),
        .Description = TEXT("A potion that restores a significant amount of mana. Ingredients unknown."),
        .Words = TEXT(""),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 115.0f,
            .MaxMagicLevelRatio = 205.0f},
        .SpellType = ESpellType::Potion,
        .ManaCost = 0,
        .IconPath = TEXT("/Game/HUDs/Images/UI/Items/manafluid_icon.manafluid_icon"),
        .AllowedVocations = AllVocations});

    // Ultimate Healing Rune (SupportRune)
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Ultimate Healing Rune"),
        .Description = TEXT("This rune is etched with a sigil of healing, instantly restoring a large amount of health."),
        .Words = TEXT(""), // Runes don't have magic words
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .bIsHealingEffect = true},
        .SpellFormula = {.MinMagicLevelRatio = 3.3f, .MaxMagicLevelRatio = 5.3f},
        .SpellType = ESpellType::SupportRune,
        .Element = ESpellElement::Healing,
        .ManaCost = 0,
        .IconPath = TEXT("/Game/HUDs/Images/ultimate_healing_rune_icon.ultimate_healing_rune_icon"),
        .AllowedVocations = AllVocations});

    // Great Fireball Rune (AreaRune)
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Great Fireball Rune"),
        .Description = TEXT("This rune is etched with a sigil of fire, creating a large explosion that deals fire damage to all within its area."),
        .Words = TEXT(""),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 1200.0f,
        .Area = {.AreaGrid = SmallExplosionArea},
        .NiagaraEffect = ExplosionAsset.Object,
        .NiagaraHomingEffect = MageFireballAsset.Object,
        .HomingSound = WooshSound.Object,
        .HitSound = FireMagicSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.35f,
            .MaxMagicLevelRatio = 0.65f},
        .SpellType = ESpellType::AreaRune,
        .Element = ESpellElement::Fire,
        .ManaCost = 0,
        .IconPath = TEXT("/Game/HUDs/Images/great_fireball_rune_icon.great_fireball_rune_icon"),
        .AllowedVocations = AllVocations});

    // Explosion Rune (AreaRune)
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Explosion Rune"),
        .Description = TEXT("This rune is etched with a sigil of explosion, creating a cross-shaped explosion that deals damage to all within its area."),
        .Words = TEXT(""),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 1200.0f,
        .Area = {.AreaGrid = CrossArea},
        .NiagaraEffect = ExplosionAsset.Object,
        .NiagaraHomingEffect = MageFireballAsset.Object,
        .HomingSound = WooshSound.Object,
        .HitSound = ExplosionSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.2f,
            .MaxMagicLevelRatio = 1.0f},
        .SpellType = ESpellType::AreaRune,
        .ManaCost = 0,
        .IconPath = TEXT("/Game/HUDs/Images/explosion_rune_icon.explosion_rune_icon"),
        .AllowedVocations = AllVocations});

    // Magic Wall Rune
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Magic Wall Rune"),
        .Description = TEXT("This rune is etched with a sigil that creates a magic wall, blocking movement."),
        .Words = TEXT(""),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 1200.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = ExplosionAsset.Object,
        .NiagaraHomingEffect = MageFireballAsset.Object,
        .HomingSound = WooshSound.Object,
        .HitSound = RockRollSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::MagicWall,
        .ManaCost = 0,
        .IconPath = TEXT("/Game/HUDs/Images/magic_wall_rune_icon.magic_wall_rune_icon"),
        .AllowedVocations = AllVocations});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Berserk"),
        .Description = TEXT("The knight channels a surge of energy and swings his weapon with force at all surrounding enemies."),
        .Words = TEXT("exori"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = MeleeArea},
        .NiagaraEffect = WaterSlashEffect.Object,
        .bSpawnSingleEffect = true,
        .SpellSound = KnightLightStrainSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.7f,
            .MaxMagicLevelRatio = 1.0f},
        .bIsSkillBasedSpell = true,
        .SpellType = ESpellType::InstantAreaSpell,
        .ManaCost = 95,
        .IconPath = TEXT("/Game/HUDs/Images/berserk_icon.berserk_icon"),
        .AllowedVocations = KnightOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Fierce Berserk"),
        .Description = TEXT("The knight channels an overwhelming surge of energy and unleashes a devastating swing at all surrounding enemies."),
        .Words = TEXT("exori gran"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = MeleeArea},
        .NiagaraEffect = GrandFireSlash.Object,
        .bSpawnSingleEffect = true,
        .SpellSound = KnightModerateStrainSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.9f,
            .MaxMagicLevelRatio = 1.5f},
        .bIsSkillBasedSpell = true,
        .SpellType = ESpellType::InstantAreaSpell,
        .ManaCost = 190,
        .IconPath = TEXT("/Game/HUDs/Images/fierce_berserk_icon.fierce_berserk_icon"),
        .AllowedVocations = KnightOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Annihilation"),
        .Description = TEXT("The knight's very life essence is poured into a single strike, dealing massive damage to a single target."),
        .Words = TEXT("exori gran ico"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = true,
        .MaxTargetDistance = 250.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = RapidSlicesEffect.Object,
        .SpellSound = KnightHeavyStrainSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 1.4f,
            .MaxMagicLevelRatio = 1.9f},
        .bIsSkillBasedSpell = true,
        .SpellType = ESpellType::TargetedSpell,
        .ManaCost = 400,
        .IconPath = TEXT("/Game/HUDs/Images/annihilation_icon.annihilation_icon"),
        .AllowedVocations = KnightOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Blood Rage"),
        .Description = TEXT("The knight channels a surge of bloodlust, increasing his speed and damage output but reducing his defense."),
        .Words = TEXT("utito tempo"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = BloodHitEffect.Object,
        .SpellSound = BerserkShoutSound.Object,
        .Effects = {
            .SpeedMultiplier = 1.2f,
            .Duration = 12.0f,
            .MeleeSkillMultiplier = 1.25f,
            .DamageReceivedMultiplier = 1.25f},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::SupportSpell,
        .ManaCost = 210,
        .IconPath = TEXT("/Game/HUDs/Images/blood_rage_icon.blood_rage_icon"),
        .AllowedVocations = KnightOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Charge"),
        .Description = TEXT("The knight's inner strength propels the caster forward with unstoppable force. Greatly increases the caster's movement speed."),
        .Words = TEXT("utani tempo hur"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .SpellSound = KnightDashSound.Object,
        .Effects = {
            .SpeedMultiplier = 1.8f,
            .Duration = 5.0f},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::SupportSpell,
        .ManaCost = 300,
        .IconPath = TEXT("/Game/HUDs/Images/charge_icon.charge_icon"),
        .AllowedVocations = KnightOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Eternal Winter"),
        .Description = TEXT("The druid calls on the spirits of Winter, causing a massive blizzard that deals ice damage to all within its area."),
        .Words = TEXT("exevo gran mas frigo"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = ExplosionArea},
        .NiagaraEffect = IceStrikeAsset.Object,
        .SpellSound = WindGustSound.Object,
        .Effects = {
            .SpeedMultiplier = -0.15f,
            .Duration = 4.0f},
        .SpellFormula = {.MinMagicLevelRatio = 1.4f, .MaxMagicLevelRatio = 2.4f},
        .SpellType = ESpellType::InstantAreaSpell,
        .Element = ESpellElement::Ice,
        .ManaCost = 1500,
        .IconPath = TEXT("/Game/HUDs/Images/eternal_winter_icon.eternal_winter_icon"),
        .AllowedVocations = DruidOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Magic Shield"),
        .Description = TEXT("The mage channels a protective barrier around the caster, absorbing damage and reducing incoming damage."),
        .Words = TEXT("utamo vita"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .SpeedMultiplier = 1.0f,
            .Duration = 180.0f,
            .bIsHealingEffect = false,
            .bIsMagicShield = true,
            .DamageReceivedMultiplier = 1.2f},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::SupportSpell,
        .ManaCost = 50,
        .IconPath = TEXT("/Game/HUDs/Images/mana_shield_icon"),
        .AllowedVocations = MagesAndPaladins});

    // Haste
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Haste"),
        .Description = TEXT("The warrior channels a surge of energy, increasing his movement speed."),
        .Words = TEXT("utani hur"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .SpellSound = MagicLowWooshSound.Object,
        .Effects = {
            .SpeedMultiplier = 1.3f,
            .Duration = 30.0f},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::SupportSpell,
        .ManaCost = 60,
        .IconPath = TEXT("/Game/HUDs/Images/strong_haste_icon.strong_haste_icon"),
        .AllowedVocations = KnightsAndPaladins});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Heal Friend"),
        .Description = TEXT("The druid calls upon the healing spirits, instantly restoring health to a single target."),
        .Words = TEXT("exura sio"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = true,
        .MaxTargetDistance = 1600.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .bIsHealingEffect = true},
        .SpellFormula = {.MinMagicLevelRatio = 4.0f, .MaxMagicLevelRatio = 6.0f},
        .SpellType = ESpellType::TargetedHeal,
        .Element = ESpellElement::Healing,
        .ManaCost = 140,
        .IconPath = TEXT("/Game/HUDs/Images/heal_friend_icon.heal_friend_icon"),
        .AllowedVocations = DruidOnly});

    // Mass Healing
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Mass Healing"),
        .Description = TEXT("The druid calls upon the healing spirits, instantly restoring health to all allies within its area."),
        .Words = TEXT("exura gran mas res"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SmallExplosionArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .bIsHealingEffect = true},
        .SpellFormula = {.MinMagicLevelRatio = 3.0f, .MaxMagicLevelRatio = 4.0f},
        .SpellType = ESpellType::InstantAreaHeal,
        .Element = ESpellElement::Healing,
        .ManaCost = 150,
        .IconPath = TEXT("/Game/HUDs/Images/mass_healing_icon.mass_healing_icon"),
        .AllowedVocations = DruidOnly});

    // Sharpshooter
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Sharpshooter"),
        .Description = TEXT("The paladin focuses his mind and aims meticulously, greatly increasing his distance skill, but reducing his movement speed and preventing him from healing."),
        .Words = TEXT("utito tempo san"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .SpeedMultiplier = 0.25f,
            .Duration = 10.0f,
            .DistanceSkillMultiplier = 1.55f,
            .bBlocksHealing = true},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::SupportSpell,
        .ManaCost = 450,
        .IconPath = TEXT("/Game/HUDs/Images/sharpshooter_icon.sharpshooter_icon"),
        .AllowedVocations = PaladinOnly});

    // Holy Arrow
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Holy Arrow"),
        .Description = TEXT("The paladin calls upon the heavens to unleash a holy arrow, striking a single target with a powerful force."),
        .Words = TEXT("exori con san"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = true,
        .MaxTargetDistance = 1800.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = HolyArrowHitEffect.Object,
        .NiagaraHomingEffect = HolyArrowEffect.Object,
        .SpellSound = BowAndArrowSound.Object,
        .HitSound = ThumpSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.8f,
            .MaxMagicLevelRatio = 1.5f},
        .bIsSkillBasedSpell = true,
        .SpellType = ESpellType::TargetedSpell,
        .Element = ESpellElement::Holy,
        .ManaCost = 1000,
        .IconPath = TEXT("/Game/HUDs/Images/holy_arrow_icon.holy_arrow_icon"),
        .AllowedVocations = PaladinOnly});

    // Ethereal Arrow
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Ethereal Arrow"),
        .Description = TEXT("The paladin calls upon the heavens to unleash an ethereal arrow that will bounce until it hits a target, detonating on impact and dealing damage to all within its area."),
        .Words = TEXT("exori con"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 1600.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = GalaxyArrowHitEffect.Object,
        .NiagaraHomingEffect = GalaxyArrowEffect.Object,
        .SpellSound = BowAndArrowSound.Object,
        .HitSound = ThumpSound.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {.MinMagicLevelRatio = 0.75f, .MaxMagicLevelRatio = 1.4f},
        .bIsSkillBasedSpell = true,
        .SpellType = ESpellType::ProjectileSpell,
        .Element = ESpellElement::Energy,
        .ManaCost = 500,
        .IconPath = TEXT("/Game/HUDs/Images/ethereal_arrow_icon.ethereal_arrow_icon"),
        .AllowedVocations = PaladinOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Light"),
        .Description = TEXT("Let there be light."),
        .Words = TEXT("utevo lux"),
        .bIsSelfTarget = true,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .SpeedMultiplier = 1.0f,
            .Duration = 360.0f,
            .bIsLight = true},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::SupportSpell,
        .ManaCost = 20,
        .IconPath = TEXT("/Game/HUDs/Images/light_icon.light_icon"),
        .AllowedVocations = AllVocations});

    // Great Energy Beam
    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Great Energy Beam"),
        .Description = TEXT("The sorcerer channels a powerful beam of energy, damaging all enemies ahead."),
        .Words = TEXT("exevo gran vis lux"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = false,
        .MaxTargetDistance = 0.0f,
        .Area = {.AreaGrid = BeamArea},
        .CascadeEffect = EnergyStrikeAsset.Object,
        .Effects = FSpellEffect(),
        .SpellFormula = {
            .MinMagicLevelRatio = 0.8f,
            .MaxMagicLevelRatio = 2.3f},
        .SpellType = ESpellType::InstantAreaSpell,
        .Element = ESpellElement::Energy,
        .ManaCost = 250,
        .IconPath = TEXT("/Game/HUDs/Images/great_energy_beam_icon.great_energy_beam_icon"),
        .AllowedVocations = SorcererOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Paralyse Rune"),
        .Description = TEXT("This rune is etched with a sigil that slows down the target's movement significantly."),
        .Words = TEXT(""), // Runes don't have magic words
        .bIsSelfTarget = false,
        .bMustBeTargeted = true,
        .MaxTargetDistance = 1200.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = MagicSparkles.Object,
        .Effects = {
            .SpeedMultiplier = -0.85f,
            .Duration = 6.0f},
        .SpellFormula = FSpellFormula(),
        .SpellType = ESpellType::TargetedRune,
        .ManaCost = 1400,
        .IconPath = TEXT("/Game/HUDs/Images/paralyse_rune_icon.paralyse_rune_icon"),
        .AllowedVocations = DruidOnly});

    RegisterSpell(FSpellDefinition{
        .Name = TEXT("Whirlwind Throw"),
        .Description = TEXT("A spinning attack that damages and slows down the target."),
        .Words = TEXT("exori hur"),
        .bIsSelfTarget = false,
        .bMustBeTargeted = true,
        .MaxTargetDistance = 1400.0f,
        .Area = {.AreaGrid = SingleTargetArea},
        .NiagaraEffect = BloodHitEffect.Object,
        .NiagaraHomingEffect = ThrowingAxeEffect.Object,
        .HomingSound = RotatingWooshSound.Object,
        .HitSound = ThumpSound.Object,
        .Effects = {
            .SpeedMultiplier = -0.25f,
            .Duration = 6.0f},
        .SpellFormula = {.MinMagicLevelRatio = 0.15f, .MaxMagicLevelRatio = 0.3f},
        .bIsSkillBasedSpell = true,
        .SpellType = ESpellType::TargetedSpell,
        .Element = ESpellElement::Physical,
        .ManaCost = 40,
        .IconPath = TEXT("/Game/HUDs/Images/whirlwind_throw_icon.whirlwind_throw_icon"),
        .AllowedVocations = KnightOnly});
}

void USpellDatabase::RegisterSpell(const FSpellDefinition &SpellDef)
{
    FSpellDefinition NewSpell = SpellDef;
    NewSpell.Area.GridSize = FVector2D(SpellDef.Area.AreaGrid.Num(),
                                       SpellDef.Area.AreaGrid.Num() > 0 ? SpellDef.Area.AreaGrid[0].Num() : 0);

    // Always use Name as the key
    Spells.Emplace(SpellDef.Name.ToLower(), MoveTemp(NewSpell));
}

const FSpellDefinition *USpellDatabase::GetSpell(const FString &Key)
{
    return Spells.Find(Key.ToLower());
}

const FSpellDefinition *USpellDatabase::GetSpellByWords(const FString &Words)
{
    for (const auto &SpellPair : Spells)
    {
        if (SpellPair.Value.Words.Equals(Words))
        {
            return &SpellPair.Value;
        }
    }

    return nullptr;
}

const TMap<FString, FSpellDefinition> &USpellDatabase::GetAllSpells(const EVocation Vocation)
{
    static TMap<FString, FSpellDefinition> FilteredSpells;
    FilteredSpells.Empty();

    // If no vocation specified or None, return all spells
    if (Vocation == EVocation::None)
    {
        return Spells;
    }

    // Filter spells based on vocation
    for (const auto &SpellPair : Spells)
    {
        if (SpellPair.Value.AllowedVocations.Contains(Vocation))
        {
            FilteredSpells.Add(SpellPair.Key, SpellPair.Value);
        }
    }

    return FilteredSpells;
}