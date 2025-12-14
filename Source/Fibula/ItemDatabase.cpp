#include "ItemDatabase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

TMap<FString, FGameItem> UItemDatabase::Items;
TMap<EItemRarity, TArray<FGameItem>> UItemDatabase::ItemsByRarity;

void UItemDatabase::Initialize()
{
    RegisterRunes();
    RegisterConsumables();

    
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PlainArrowFinder(TEXT("/Game/Effects/NS_PlainArrow.NS_PlainArrow"));
    if (!PlainArrowFinder.Succeeded())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FlamingArrowEffectFinder(TEXT("/Game/Effects/NS_FireArrow.NS_FireArrow"));
    if (!FlamingArrowEffectFinder.Succeeded())
    {
        return;
    }

    
    FGameItem AncientTiara;
    AncientTiara.Name = TEXT("Ancient Tiara");
    AncientTiara.Description = TEXT("A beautiful tiara worn by ancient queens.");
    AncientTiara.Rarity = EItemRarity::Legendary;
    AncientTiara.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ancient_tiara_icon.ancient_tiara_icon")));
    AncientTiara.Weight = 8.2f;
    AncientTiara.ItemType = EItemType::Equipment;
    AncientTiara.bIsStackable = false;
    AncientTiara.StackCount = 1;
    AncientTiara.UseType = EItemUseType::Equipment;
    AncientTiara.UseAction = TEXT("Equip Helmet");
    AncientTiara.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    AncientTiara.EquipmentAttributes.Armor = 7;
    AncientTiara.EquipmentAttributes.MagicLevelBonus = 8;
    RegisterItem(AncientTiara);

    FGameItem Arbalest;
    Arbalest.Name = TEXT("Arbalest");
    Arbalest.Description = TEXT("A powerful crossbow that can pierce through heavy armor.");
    Arbalest.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/arbalest_icon.arbalest_icon")));
    Arbalest.Rarity = EItemRarity::Legendary;
    Arbalest.Weight = 95.0f;
    Arbalest.ItemType = EItemType::Equipment;
    Arbalest.bIsStackable = false;
    Arbalest.StackCount = 1;
    Arbalest.UseType = EItemUseType::Equipment;
    Arbalest.UseAction = TEXT("Equip Crossbow");
    Arbalest.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Arbalest.EquipmentAttributes.Attack = 32;
    Arbalest.EquipmentAttributes.bCanFireProjectiles = true;
    Arbalest.EquipmentAttributes.bIsTwoHanded = true;
    Arbalest.EquipmentAttributes.DistanceSkillBonus = 6;
    RegisterItem(Arbalest);

    FGameItem BlueLegs;
    BlueLegs.Name = TEXT("Blue Legs");
    BlueLegs.Description = TEXT("Leg armor imbued with magical properties.");
    BlueLegs.Rarity = EItemRarity::Epic;
    BlueLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/blue_legs_icon.blue_legs_icon")));
    BlueLegs.Weight = 18.0f;
    BlueLegs.ItemType = EItemType::Equipment;
    BlueLegs.bIsStackable = false;
    BlueLegs.StackCount = 1;
    BlueLegs.UseType = EItemUseType::Equipment;
    BlueLegs.UseAction = TEXT("Equip Legs");
    BlueLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    BlueLegs.EquipmentAttributes.Armor = 8;
    BlueLegs.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(BlueLegs);

    FGameItem BlueRobe;
    BlueRobe.Name = TEXT("Blue Robe");
    BlueRobe.Description = TEXT("A magical robe that enhances spellcasting abilities.");
    BlueRobe.Rarity = EItemRarity::Legendary;
    BlueRobe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/blue_robe_icon.blue_robe_icon")));
    BlueRobe.Weight = 22.0f;
    BlueRobe.ItemType = EItemType::Equipment;
    BlueRobe.bIsStackable = false;
    BlueRobe.StackCount = 1;
    BlueRobe.UseType = EItemUseType::Equipment;
    BlueRobe.UseAction = TEXT("Equip Armor");
    BlueRobe.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    BlueRobe.EquipmentAttributes.Armor = 8;
    BlueRobe.EquipmentAttributes.MagicLevelBonus = 9;
    RegisterItem(BlueRobe);

    FGameItem CalopteryxCape;
    CalopteryxCape.Name = TEXT("Calopteryx Cape");
    CalopteryxCape.Description = TEXT("A mystical cape woven from rare materials.");
    CalopteryxCape.Rarity = EItemRarity::Rare;
    CalopteryxCape.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/calopteryx_cape_icon.calopteryx_cape_icon")));
    CalopteryxCape.Weight = 19.0f;
    CalopteryxCape.ItemType = EItemType::Equipment;
    CalopteryxCape.bIsStackable = false;
    CalopteryxCape.StackCount = 1;
    CalopteryxCape.UseType = EItemUseType::Equipment;
    CalopteryxCape.UseAction = TEXT("Equip Cape");
    CalopteryxCape.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    CalopteryxCape.EquipmentAttributes.Armor = 5;
    CalopteryxCape.EquipmentAttributes.MagicLevelBonus = 5;
    RegisterItem(CalopteryxCape);

    FGameItem CrownHelmet;
    CrownHelmet.Name = TEXT("Crown Helmet");
    CrownHelmet.Description = TEXT("A royal helmet worn by elite knights.");
    CrownHelmet.Rarity = EItemRarity::Common;
    CrownHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crown_helmet_icon.crown_helmet_icon")));
    CrownHelmet.Weight = 29.5f;
    CrownHelmet.ItemType = EItemType::Equipment;
    CrownHelmet.bIsStackable = false;
    CrownHelmet.StackCount = 1;
    CrownHelmet.UseType = EItemUseType::Equipment;
    CrownHelmet.UseAction = TEXT("Equip Helmet");
    CrownHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    CrownHelmet.EquipmentAttributes.Armor = 8;
    CrownHelmet.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(CrownHelmet);

    FGameItem CrownArmor;
    CrownArmor.Name = TEXT("Crown Armor");
    CrownArmor.Description = TEXT("A royal armor worn by elite knights.");
    CrownArmor.Rarity = EItemRarity::Rare;
    CrownArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crown_armor_icon.crown_armor_icon")));
    CrownArmor.Weight = 99.0f;
    CrownArmor.ItemType = EItemType::Equipment;
    CrownArmor.bIsStackable = false;
    CrownArmor.StackCount = 1;
    CrownArmor.UseType = EItemUseType::Equipment;
    CrownArmor.UseAction = TEXT("Equip Armor");
    CrownArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    CrownArmor.EquipmentAttributes.Armor = 16;
    CrownArmor.EquipmentAttributes.DistanceSkillBonus = 2;
    CrownArmor.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(CrownArmor);

    FGameItem CrownLegs;
    CrownLegs.Name = TEXT("Crown Legs");
    CrownLegs.Description = TEXT("Royal leg protection worn by elite knights.");
    CrownLegs.Rarity = EItemRarity::Rare;
    CrownLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crown_legs_icon.crown_legs_icon")));
    CrownLegs.Weight = 65.0f;
    CrownLegs.ItemType = EItemType::Equipment;
    CrownLegs.bIsStackable = false;
    CrownLegs.StackCount = 1;
    CrownLegs.UseType = EItemUseType::Equipment;
    CrownLegs.UseAction = TEXT("Equip Legs");
    CrownLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    CrownLegs.EquipmentAttributes.Armor = 13;
    RegisterItem(CrownLegs);

    FGameItem ZaoanLegs;
    ZaoanLegs.Name = TEXT("Zaoan Legs");
    ZaoanLegs.Description = TEXT("Leg protection crafted by the Zaoan artisans.");
    ZaoanLegs.Rarity = EItemRarity::Rare;
    ZaoanLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/zaoan_legs_icon.zaoan_legs_icon")));
    ZaoanLegs.Weight = 64.0f;
    ZaoanLegs.ItemType = EItemType::Equipment;
    ZaoanLegs.bIsStackable = false;
    ZaoanLegs.StackCount = 1;
    ZaoanLegs.UseType = EItemUseType::Equipment;
    ZaoanLegs.UseAction = TEXT("Equip Legs");
    ZaoanLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    ZaoanLegs.EquipmentAttributes.Armor = 8;
    ZaoanLegs.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(ZaoanLegs);

    FGameItem CrusaderHelmet;
    CrusaderHelmet.Name = TEXT("Crusader Helmet");
    CrusaderHelmet.Description = TEXT("A helmet worn by holy warriors.");
    CrusaderHelmet.Rarity = EItemRarity::Rare;
    CrusaderHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crusader_helmet_icon.crusader_helmet_icon")));
    CrusaderHelmet.Weight = 52.0f;
    CrusaderHelmet.ItemType = EItemType::Equipment;
    CrusaderHelmet.bIsStackable = false;
    CrusaderHelmet.StackCount = 1;
    CrusaderHelmet.UseType = EItemUseType::Equipment;
    CrusaderHelmet.UseAction = TEXT("Equip Helmet");
    CrusaderHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    CrusaderHelmet.EquipmentAttributes.Armor = 12;
    CrusaderHelmet.EquipmentAttributes.DistanceSkillBonus = 3;
    CrusaderHelmet.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(CrusaderHelmet);

    FGameItem DemonArmor;
    DemonArmor.Name = TEXT("Demon Armor");
    DemonArmor.Description = TEXT("Armor forged in demonic fires.");
    DemonArmor.Rarity = EItemRarity::Artifact;
    DemonArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/demon_armor_icon.demon_armor_icon")));
    DemonArmor.Weight = 80.0f;
    DemonArmor.ItemType = EItemType::Equipment;
    DemonArmor.bIsStackable = false;
    DemonArmor.StackCount = 1;
    DemonArmor.UseType = EItemUseType::Equipment;
    DemonArmor.UseAction = TEXT("Equip Armor");
    DemonArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    DemonArmor.EquipmentAttributes.Armor = 30;
    DemonArmor.EquipmentAttributes.DistanceSkillBonus = 5;
    DemonArmor.EquipmentAttributes.MeleeSkillBonus = 6;
    RegisterItem(DemonArmor);

    FGameItem DemonHelmet;
    DemonHelmet.Name = TEXT("Demon Helmet");
    DemonHelmet.Description = TEXT("A helmet infused with demonic power.");
    DemonHelmet.Rarity = EItemRarity::Legendary;
    DemonHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/demon_helmet_icon.demon_helmet_icon")));
    DemonHelmet.Weight = 29.5f;
    DemonHelmet.ItemType = EItemType::Equipment;
    DemonHelmet.bIsStackable = false;
    DemonHelmet.StackCount = 1;
    DemonHelmet.UseType = EItemUseType::Equipment;
    DemonHelmet.UseAction = TEXT("Equip Helmet");
    DemonHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    DemonHelmet.EquipmentAttributes.Armor = 18;
    DemonHelmet.EquipmentAttributes.DistanceSkillBonus = 6;
    DemonHelmet.EquipmentAttributes.MeleeSkillBonus = 6;
    RegisterItem(DemonHelmet);

    FGameItem DemonLegs;
    DemonLegs.Name = TEXT("Demon Legs");
    DemonLegs.Description = TEXT("Leg armor forged with demonic essence.");
    DemonLegs.Rarity = EItemRarity::Artifact;
    DemonLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/demon_legs_icon.demon_legs_icon")));
    DemonLegs.Weight = 70.0f;
    DemonLegs.ItemType = EItemType::Equipment;
    DemonLegs.bIsStackable = false;
    DemonLegs.StackCount = 1;
    DemonLegs.UseType = EItemUseType::Equipment;
    DemonLegs.UseAction = TEXT("Equip Legs");
    DemonLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    DemonLegs.EquipmentAttributes.Armor = 15;
    DemonLegs.EquipmentAttributes.DistanceSkillBonus = 5;
    DemonLegs.EquipmentAttributes.MeleeSkillBonus = 2;
    RegisterItem(DemonLegs);

    FGameItem DevilHelmet;
    DevilHelmet.Name = TEXT("Devil Helmet");
    DevilHelmet.Description = TEXT("A helmet that radiates evil power.");
    DevilHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/devil_helmet_icon.devil_helmet_icon")));
    DevilHelmet.Weight = 48.0f;
    DevilHelmet.ItemType = EItemType::Equipment;
    DevilHelmet.bIsStackable = false;
    DevilHelmet.StackCount = 1;
    DevilHelmet.UseType = EItemUseType::Equipment;
    DevilHelmet.UseAction = TEXT("Equip Helmet");
    DevilHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    DevilHelmet.EquipmentAttributes.Armor = 10;
    DevilHelmet.EquipmentAttributes.DistanceSkillBonus = 2;
    DevilHelmet.EquipmentAttributes.MeleeSkillBonus = 2;
    RegisterItem(DevilHelmet);

    FGameItem DragonLance;
    DragonLance.Name = TEXT("Dragon Lance");
    DragonLance.Description = TEXT("A legendary lance used to slay dragons.");
    DragonLance.Rarity = EItemRarity::Epic;
    DragonLance.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_lance_icon.dragon_lance_icon")));
    DragonLance.Weight = 67.0f;
    DragonLance.ItemType = EItemType::Equipment;
    DragonLance.bIsStackable = false;
    DragonLance.StackCount = 1;
    DragonLance.UseType = EItemUseType::Equipment;
    DragonLance.UseAction = TEXT("Equip Lance");
    DragonLance.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DragonLance.EquipmentAttributes.Attack = 50;
    DragonLance.EquipmentAttributes.Defense = 16;
    DragonLance.EquipmentAttributes.bIsTwoHanded = true;
    DragonLance.EquipmentAttributes.MeleeSkillBonus = 2;
    RegisterItem(DragonLance);

    FGameItem DragonRobe;
    DragonRobe.Name = TEXT("Dragon Robe");
    DragonRobe.Description = TEXT("A robe woven with dragon scales.");
    DragonRobe.Rarity = EItemRarity::Epic;
    DragonRobe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_robe_icon.dragon_robe_icon")));
    DragonRobe.Weight = 40.0f;
    DragonRobe.ItemType = EItemType::Equipment;
    DragonRobe.bIsStackable = false;
    DragonRobe.StackCount = 1;
    DragonRobe.UseType = EItemUseType::Equipment;
    DragonRobe.UseAction = TEXT("Equip Armor");
    DragonRobe.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    DragonRobe.EquipmentAttributes.Armor = 7;
    DragonRobe.EquipmentAttributes.MagicLevelBonus = 7;
    RegisterItem(DragonRobe);

    FGameItem DragonScaleLegs;
    DragonScaleLegs.Name = TEXT("Dragon Scale Legs");
    DragonScaleLegs.Description = TEXT("Leg armor crafted from dragon scales.");
    DragonScaleLegs.Rarity = EItemRarity::Artifact;
    DragonScaleLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_scale_legs_icon.dragon_scale_legs_icon")));
    DragonScaleLegs.Weight = 48.0f;
    DragonScaleLegs.ItemType = EItemType::Equipment;
    DragonScaleLegs.bIsStackable = false;
    DragonScaleLegs.StackCount = 1;
    DragonScaleLegs.UseType = EItemUseType::Equipment;
    DragonScaleLegs.UseAction = TEXT("Equip Legs");
    DragonScaleLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    DragonScaleLegs.EquipmentAttributes.Armor = 17;
    DragonScaleLegs.EquipmentAttributes.DistanceSkillBonus = 5;
    DragonScaleLegs.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(DragonScaleLegs);

    FGameItem DragonScaleMail;
    DragonScaleMail.Name = TEXT("Dragon Scale Mail");
    DragonScaleMail.Description = TEXT("Armor forged from the scales of ancient dragons.");
    DragonScaleMail.Rarity = EItemRarity::Artifact;
    DragonScaleMail.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_scale_mail_icon.dragon_scale_mail_icon")));
    DragonScaleMail.Weight = 114.0f;
    DragonScaleMail.ItemType = EItemType::Equipment;
    DragonScaleMail.bIsStackable = false;
    DragonScaleMail.StackCount = 1;
    DragonScaleMail.UseType = EItemUseType::Equipment;
    DragonScaleMail.UseAction = TEXT("Equip Armor");
    DragonScaleMail.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    DragonScaleMail.EquipmentAttributes.Armor = 20;
    DragonScaleMail.EquipmentAttributes.DistanceSkillBonus = 7;
    DragonScaleMail.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(DragonScaleMail);

    FGameItem EarthbornTitanArmor;
    EarthbornTitanArmor.Name = TEXT("Earthborn Titan Armor");
    EarthbornTitanArmor.Description = TEXT("Armor forged from earthen materials by ancient titans.");
    EarthbornTitanArmor.Rarity = EItemRarity::Epic;
    EarthbornTitanArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/earthborn_titan_armor_icon.earthborn_titan_armor_icon")));
    EarthbornTitanArmor.Weight = 95.0f;
    EarthbornTitanArmor.ItemType = EItemType::Equipment;
    EarthbornTitanArmor.bIsStackable = false;
    EarthbornTitanArmor.StackCount = 1;
    EarthbornTitanArmor.UseType = EItemUseType::Equipment;
    EarthbornTitanArmor.UseAction = TEXT("Equip Armor");
    EarthbornTitanArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    EarthbornTitanArmor.EquipmentAttributes.Armor = 24;
    EarthbornTitanArmor.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(EarthbornTitanArmor);

    FGameItem ElvenLegs;
    ElvenLegs.Name = TEXT("Elven Legs");
    ElvenLegs.Description = TEXT("Light and elegant leg protection crafted by elves.");
    ElvenLegs.Rarity = EItemRarity::Legendary;
    ElvenLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/elven_legs_icon.elven_legs_icon")));
    ElvenLegs.Weight = 33.0f;
    ElvenLegs.ItemType = EItemType::Equipment;
    ElvenLegs.bIsStackable = false;
    ElvenLegs.StackCount = 1;
    ElvenLegs.UseType = EItemUseType::Equipment;
    ElvenLegs.UseAction = TEXT("Equip Legs");
    ElvenLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    ElvenLegs.EquipmentAttributes.Armor = 16;
    ElvenLegs.EquipmentAttributes.DistanceSkillBonus = 4;
    RegisterItem(ElvenLegs);

    FGameItem ElvenMail;
    ElvenMail.Name = TEXT("Elven Mail");
    ElvenMail.Description = TEXT("Lightweight armor crafted by elven smiths.");
    ElvenMail.Rarity = EItemRarity::Legendary;
    ElvenMail.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/elven_mail_icon.elven_mail_icon")));
    ElvenMail.Weight = 90.0f;
    ElvenMail.ItemType = EItemType::Equipment;
    ElvenMail.bIsStackable = false;
    ElvenMail.StackCount = 1;
    ElvenMail.UseType = EItemUseType::Equipment;
    ElvenMail.UseAction = TEXT("Equip Armor");
    ElvenMail.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    ElvenMail.EquipmentAttributes.Armor = 17;
    ElvenMail.EquipmentAttributes.DistanceSkillBonus = 6;
    RegisterItem(ElvenMail);

    FGameItem FalconCirclet;
    FalconCirclet.Name = TEXT("Falcon Circlet");
    FalconCirclet.Description = TEXT("A magical circlet blessed with the power of falcons.");
    FalconCirclet.Rarity = EItemRarity::Legendary;
    FalconCirclet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_circlet_icon.falcon_circlet_icon")));
    FalconCirclet.Weight = 25.0f;
    FalconCirclet.ItemType = EItemType::Equipment;
    FalconCirclet.bIsStackable = false;
    FalconCirclet.StackCount = 1;
    FalconCirclet.UseType = EItemUseType::Equipment;
    FalconCirclet.UseAction = TEXT("Equip Helmet");
    FalconCirclet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    FalconCirclet.EquipmentAttributes.Armor = 7;
    FalconCirclet.EquipmentAttributes.MagicLevelBonus = 9;
    RegisterItem(FalconCirclet);

    FGameItem FalconCoif;
    FalconCoif.Name = TEXT("Falcon Coif");
    FalconCoif.Description = TEXT("A protective headpiece imbued with falcon magic.");
    FalconCoif.Rarity = EItemRarity::Epic;
    FalconCoif.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_coif_icon.falcon_coif_icon")));
    FalconCoif.Weight = 30.0f;
    FalconCoif.ItemType = EItemType::Equipment;
    FalconCoif.bIsStackable = false;
    FalconCoif.StackCount = 1;
    FalconCoif.UseType = EItemUseType::Equipment;
    FalconCoif.UseAction = TEXT("Equip Helmet");
    FalconCoif.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    FalconCoif.EquipmentAttributes.Armor = 16;
    FalconCoif.EquipmentAttributes.DistanceSkillBonus = 5;
    FalconCoif.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(FalconCoif);

    FGameItem FalconGreaves;
    FalconGreaves.Name = TEXT("Falcon Greaves");
    FalconGreaves.Description = TEXT("Leg armor blessed with falcon magic.");
    FalconGreaves.Rarity = EItemRarity::Epic;
    FalconGreaves.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_greaves_icon.falcon_greaves_icon")));
    FalconGreaves.Weight = 36.0f;
    FalconGreaves.ItemType = EItemType::Equipment;
    FalconGreaves.bIsStackable = false;
    FalconGreaves.StackCount = 1;
    FalconGreaves.UseType = EItemUseType::Equipment;
    FalconGreaves.UseAction = TEXT("Equip Legs");
    FalconGreaves.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    FalconGreaves.EquipmentAttributes.Armor = 14;
    FalconGreaves.EquipmentAttributes.DistanceSkillBonus = 3;
    FalconGreaves.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(FalconGreaves);

    FGameItem FalconPlate;
    FalconPlate.Name = TEXT("Falcon Plate");
    FalconPlate.Description = TEXT("A masterpiece of armor blessed with falcon magic.");
    FalconPlate.Rarity = EItemRarity::Legendary;
    FalconPlate.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_plate_icon.falcon_plate_icon")));
    FalconPlate.Weight = 75.0f;
    FalconPlate.ItemType = EItemType::Equipment;
    FalconPlate.bIsStackable = false;
    FalconPlate.StackCount = 1;
    FalconPlate.UseType = EItemUseType::Equipment;
    FalconPlate.UseAction = TEXT("Equip Armor");
    FalconPlate.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    FalconPlate.EquipmentAttributes.Armor = 26;
    FalconPlate.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(FalconPlate);

    FGameItem FirebornGiantArmor;
    FirebornGiantArmor.Name = TEXT("Fireborn Giant Armor");
    FirebornGiantArmor.Description = TEXT("Armor forged in the flames of ancient fire giants.");
    FirebornGiantArmor.Rarity = EItemRarity::Legendary;
    FirebornGiantArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/fireborn_giant_armor_icon.fireborn_giant_armor_icon")));
    FirebornGiantArmor.Weight = 95.0f;
    FirebornGiantArmor.ItemType = EItemType::Equipment;
    FirebornGiantArmor.bIsStackable = false;
    FirebornGiantArmor.StackCount = 1;
    FirebornGiantArmor.UseType = EItemUseType::Equipment;
    FirebornGiantArmor.UseAction = TEXT("Equip Armor");
    FirebornGiantArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    FirebornGiantArmor.EquipmentAttributes.Armor = 28;
    FirebornGiantArmor.EquipmentAttributes.MeleeSkillBonus = 6;
    RegisterItem(FirebornGiantArmor);

    FGameItem FocusCape;
    FocusCape.Name = TEXT("Focus Cape");
    FocusCape.Description = TEXT("A cape that enhances magical focus.");
    FocusCape.Rarity = EItemRarity::Uncommon;
    FocusCape.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/focus_cape_icon.focus_cape_icon")));
    FocusCape.Weight = 19.0f;
    FocusCape.ItemType = EItemType::Equipment;
    FocusCape.bIsStackable = false;
    FocusCape.StackCount = 1;
    FocusCape.UseType = EItemUseType::Equipment;
    FocusCape.UseAction = TEXT("Equip Cape");
    FocusCape.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    FocusCape.EquipmentAttributes.Armor = 5;
    FocusCape.EquipmentAttributes.MagicLevelBonus = 3;
    RegisterItem(FocusCape);

    FGameItem GhostChestplate;
    GhostChestplate.Name = TEXT("Ghost Chestplate");
    GhostChestplate.Description = TEXT("A mysterious armor that seems partially ethereal.");
    GhostChestplate.Rarity = EItemRarity::Legendary;
    GhostChestplate.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ghost_chestplate_icon.ghost_chestplate_icon")));
    GhostChestplate.Weight = 85.0f;
    GhostChestplate.ItemType = EItemType::Equipment;
    GhostChestplate.bIsStackable = false;
    GhostChestplate.StackCount = 1;
    GhostChestplate.UseType = EItemUseType::Equipment;
    GhostChestplate.UseAction = TEXT("Equip Armor");
    GhostChestplate.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    GhostChestplate.EquipmentAttributes.Armor = 16;
    GhostChestplate.EquipmentAttributes.DistanceSkillBonus = 5;
    RegisterItem(GhostChestplate);

    FGameItem GoldenArmor;
    GoldenArmor.Name = TEXT("Golden Armor");
    GoldenArmor.Description = TEXT("A magnificent armor made of pure gold.");
    GoldenArmor.Rarity = EItemRarity::Artifact;
    GoldenArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/golden_armor_icon.golden_armor_icon")));
    GoldenArmor.Weight = 80.0f;
    GoldenArmor.ItemType = EItemType::Equipment;
    GoldenArmor.bIsStackable = false;
    GoldenArmor.StackCount = 1;
    GoldenArmor.UseType = EItemUseType::Equipment;
    GoldenArmor.UseAction = TEXT("Equip Armor");
    GoldenArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    GoldenArmor.EquipmentAttributes.Armor = 32;
    GoldenArmor.EquipmentAttributes.DistanceSkillBonus = 5;
    GoldenArmor.EquipmentAttributes.MeleeSkillBonus = 6;
    RegisterItem(GoldenArmor);

    FGameItem GoldenHelmet;
    GoldenHelmet.Name = TEXT("Golden Helmet");
    GoldenHelmet.Description = TEXT("A helmet crafted from pure gold.");
    GoldenHelmet.Rarity = EItemRarity::Artifact;
    GoldenHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/golden_helmet_icon.golden_helmet_icon")));
    GoldenHelmet.Weight = 32.0f;
    GoldenHelmet.ItemType = EItemType::Equipment;
    GoldenHelmet.bIsStackable = false;
    GoldenHelmet.StackCount = 1;
    GoldenHelmet.UseType = EItemUseType::Equipment;
    GoldenHelmet.UseAction = TEXT("Equip Helmet");
    GoldenHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    GoldenHelmet.EquipmentAttributes.Armor = 20;
    GoldenHelmet.EquipmentAttributes.DistanceSkillBonus = 8;
    GoldenHelmet.EquipmentAttributes.MeleeSkillBonus = 8;
    RegisterItem(GoldenHelmet);

    FGameItem GoldenLegs;
    GoldenLegs.Name = TEXT("Golden Legs");
    GoldenLegs.Description = TEXT("Leg protection made of pure gold.");
    GoldenLegs.Rarity = EItemRarity::Artifact;
    GoldenLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/golden_legs_icon.golden_legs_icon")));
    GoldenLegs.Weight = 40.0f;
    GoldenLegs.ItemType = EItemType::Equipment;
    GoldenLegs.bIsStackable = false;
    GoldenLegs.StackCount = 1;
    GoldenLegs.UseType = EItemUseType::Equipment;
    GoldenLegs.UseAction = TEXT("Equip Legs");
    GoldenLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    GoldenLegs.EquipmentAttributes.Armor = 16;
    GoldenLegs.EquipmentAttributes.DistanceSkillBonus = 3;
    GoldenLegs.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(GoldenLegs);

    FGameItem HornedHelmet;
    HornedHelmet.Name = TEXT("Horned Helmet");
    HornedHelmet.Description = TEXT("A fierce helmet adorned with mighty horns.");
    HornedHelmet.Rarity = EItemRarity::Artifact;
    HornedHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/horned_helmet_icon.horned_helmet_icon")));
    HornedHelmet.Weight = 42.0f;
    HornedHelmet.ItemType = EItemType::Equipment;
    HornedHelmet.bIsStackable = false;
    HornedHelmet.StackCount = 1;
    HornedHelmet.UseType = EItemUseType::Equipment;
    HornedHelmet.UseAction = TEXT("Equip Helmet");
    HornedHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    HornedHelmet.EquipmentAttributes.Armor = 22;
    HornedHelmet.EquipmentAttributes.MeleeSkillBonus = 10;
    RegisterItem(HornedHelmet);

    FGameItem IcyCulottes;
    IcyCulottes.Name = TEXT("Icy Culottes");
    IcyCulottes.Description = TEXT("Leg armor imbued with frost magic.");
    IcyCulottes.Rarity = EItemRarity::Legendary;
    IcyCulottes.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/icy_culottes_icon.icy_culottes_icon")));
    IcyCulottes.Weight = 38.0f;
    IcyCulottes.ItemType = EItemType::Equipment;
    IcyCulottes.bIsStackable = false;
    IcyCulottes.StackCount = 1;
    IcyCulottes.UseType = EItemUseType::Equipment;
    IcyCulottes.UseAction = TEXT("Equip Legs");
    IcyCulottes.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    IcyCulottes.EquipmentAttributes.Armor = 9;
    IcyCulottes.EquipmentAttributes.MagicLevelBonus = 3;
    RegisterItem(IcyCulottes);

    FGameItem KnightArmor;
    KnightArmor.Name = TEXT("Knight Armor");
    KnightArmor.Description = TEXT("A well-crafted armor worn by noble knights.");
    KnightArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/knight_armor_icon.knight_armor_icon")));
    KnightArmor.Weight = 120.0f;
    KnightArmor.ItemType = EItemType::Equipment;
    KnightArmor.bIsStackable = false;
    KnightArmor.StackCount = 1;
    KnightArmor.UseType = EItemUseType::Equipment;
    KnightArmor.UseAction = TEXT("Equip Armor");
    KnightArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    KnightArmor.EquipmentAttributes.Armor = 14;
    KnightArmor.EquipmentAttributes.MeleeSkillBonus = 2;
    RegisterItem(KnightArmor);

    FGameItem KnightLegs;
    KnightLegs.Name = TEXT("Knight Legs");
    KnightLegs.Description = TEXT("Leg protection worn by noble knights.");
    KnightLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/knight_legs_icon.knight_legs_icon")));
    KnightLegs.Weight = 70.0f;
    KnightLegs.ItemType = EItemType::Equipment;
    KnightLegs.bIsStackable = false;
    KnightLegs.StackCount = 1;
    KnightLegs.UseType = EItemUseType::Equipment;
    KnightLegs.UseAction = TEXT("Equip Legs");
    KnightLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    KnightLegs.EquipmentAttributes.Armor = 9;
    RegisterItem(KnightLegs);

    FGameItem LionSpangenhelm;
    LionSpangenhelm.Name = TEXT("Lion Spangenhelm");
    LionSpangenhelm.Description = TEXT("A majestic helmet adorned with a lion's emblem.");
    LionSpangenhelm.Rarity = EItemRarity::Rare;
    LionSpangenhelm.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/lion_spangenhelm_icon.lion_spangenhelm_icon")));
    LionSpangenhelm.Weight = 43.0f;
    LionSpangenhelm.ItemType = EItemType::Equipment;
    LionSpangenhelm.bIsStackable = false;
    LionSpangenhelm.StackCount = 1;
    LionSpangenhelm.UseType = EItemUseType::Equipment;
    LionSpangenhelm.UseAction = TEXT("Equip Helmet");
    LionSpangenhelm.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    LionSpangenhelm.EquipmentAttributes.Armor = 14;
    LionSpangenhelm.EquipmentAttributes.DistanceSkillBonus = 4;
    RegisterItem(LionSpangenhelm);

    FGameItem MagicPlateArmor;
    MagicPlateArmor.Name = TEXT("Magic Plate Armor");
    MagicPlateArmor.Description = TEXT("A powerful armor enchanted with protective spells.");
    MagicPlateArmor.Rarity = EItemRarity::Artifact;
    MagicPlateArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magic_plate_armor_icon.magic_plate_armor_icon")));
    MagicPlateArmor.Weight = 85.0f;
    MagicPlateArmor.ItemType = EItemType::Equipment;
    MagicPlateArmor.bIsStackable = false;
    MagicPlateArmor.StackCount = 1;
    MagicPlateArmor.UseType = EItemUseType::Equipment;
    MagicPlateArmor.UseAction = TEXT("Equip Armor");
    MagicPlateArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    MagicPlateArmor.EquipmentAttributes.Armor = 35;
    MagicPlateArmor.EquipmentAttributes.DistanceSkillBonus = 6;
    MagicPlateArmor.EquipmentAttributes.MeleeSkillBonus = 8;
    RegisterItem(MagicPlateArmor);

    FGameItem MagicianHat;
    MagicianHat.Name = TEXT("Magician Hat");
    MagicianHat.Description = TEXT("A hat imbued with magical properties.");
    MagicianHat.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magician_hat_icon.magician_hat_icon")));
    MagicianHat.Weight = 7.5f;
    MagicianHat.ItemType = EItemType::Equipment;
    MagicianHat.bIsStackable = false;
    MagicianHat.StackCount = 1;
    MagicianHat.UseType = EItemUseType::Equipment;
    MagicianHat.UseAction = TEXT("Equip Helmet");
    MagicianHat.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    MagicianHat.EquipmentAttributes.Armor = 2;
    MagicianHat.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(MagicianHat);

    FGameItem MagmaBoots;
    MagmaBoots.Name = TEXT("Magma Boots");
    MagmaBoots.Description = TEXT("Boots forged from hardened magma.");
    MagmaBoots.Rarity = EItemRarity::Uncommon;
    MagmaBoots.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magma_boots_icon.magma_boots_icon")));
    MagmaBoots.Weight = 28.0f;
    MagmaBoots.ItemType = EItemType::Equipment;
    MagmaBoots.bIsStackable = false;
    MagmaBoots.StackCount = 1;
    MagmaBoots.UseType = EItemUseType::Equipment;
    MagmaBoots.UseAction = TEXT("Equip Boots");
    MagmaBoots.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    MagmaBoots.EquipmentAttributes.Armor = 2;
    MagmaBoots.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(MagmaBoots);

    FGameItem MagmaCoat;
    MagmaCoat.Name = TEXT("Magma Coat");
    MagmaCoat.Description = TEXT("A coat made from cooled magma, still radiating heat.");
    MagmaCoat.Rarity = EItemRarity::Rare;
    MagmaCoat.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magma_coat_icon.magma_coat_icon")));
    MagmaCoat.Weight = 95.0f;
    MagmaCoat.ItemType = EItemType::Equipment;
    MagmaCoat.bIsStackable = false;
    MagmaCoat.StackCount = 1;
    MagmaCoat.UseType = EItemUseType::Equipment;
    MagmaCoat.UseAction = TEXT("Equip Armor");
    MagmaCoat.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    MagmaCoat.EquipmentAttributes.Armor = 6;
    MagmaCoat.EquipmentAttributes.MagicLevelBonus = 4;
    RegisterItem(MagmaCoat);

    FGameItem MagmaLegs;
    MagmaLegs.Name = TEXT("Magma Legs");
    MagmaLegs.Description = TEXT("Leg protection forged from hardened magma.");
    MagmaLegs.Rarity = EItemRarity::Common;
    MagmaLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magma_legs_icon.magma_legs_icon")));
    MagmaLegs.Weight = 45.0f;
    MagmaLegs.ItemType = EItemType::Equipment;
    MagmaLegs.bIsStackable = false;
    MagmaLegs.StackCount = 1;
    MagmaLegs.UseType = EItemUseType::Equipment;
    MagmaLegs.UseAction = TEXT("Equip Legs");
    MagmaLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    MagmaLegs.EquipmentAttributes.Armor = 7;
    MagmaLegs.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(MagmaLegs);

    FGameItem MagmaMonocle;
    MagmaMonocle.Name = TEXT("Magma Monocle");
    MagmaMonocle.Description = TEXT("A monocle that glows with inner fire.");
    MagmaMonocle.Rarity = EItemRarity::Uncommon;
    MagmaMonocle.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magma_monocle_icon.magma_monocle_icon")));
    MagmaMonocle.Weight = 4.5f;
    MagmaMonocle.ItemType = EItemType::Equipment;
    MagmaMonocle.bIsStackable = false;
    MagmaMonocle.StackCount = 1;
    MagmaMonocle.UseType = EItemUseType::Equipment;
    MagmaMonocle.UseAction = TEXT("Equip Helmet");
    MagmaMonocle.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    MagmaMonocle.EquipmentAttributes.Armor = 5;
    MagmaMonocle.EquipmentAttributes.MagicLevelBonus = 5;
    RegisterItem(MagmaMonocle);

    FGameItem MasterArcherArmor;
    MasterArcherArmor.Name = TEXT("Master Archer's Armor");
    MasterArcherArmor.Description = TEXT("A lightweight armor favored by expert archers.");
    MasterArcherArmor.Rarity = EItemRarity::Epic;
    MasterArcherArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/master_archer_s_armor_icon.master_archer_s_armor_icon")));
    MasterArcherArmor.Weight = 75.0f;
    MasterArcherArmor.ItemType = EItemType::Equipment;
    MasterArcherArmor.bIsStackable = false;
    MasterArcherArmor.StackCount = 1;
    MasterArcherArmor.UseType = EItemUseType::Equipment;
    MasterArcherArmor.UseAction = TEXT("Equip Armor");
    MasterArcherArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    MasterArcherArmor.EquipmentAttributes.Armor = 15;
    MasterArcherArmor.EquipmentAttributes.DistanceSkillBonus = 4;
    RegisterItem(MasterArcherArmor);

    FGameItem MoltenPlate;
    MoltenPlate.Name = TEXT("Molten Plate");
    MoltenPlate.Description = TEXT("A plate armor forged in volcanic fires.");
    MoltenPlate.Rarity = EItemRarity::Epic;
    MoltenPlate.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/molten_plate_icon.molten_plate_icon")));
    MoltenPlate.Weight = 92.0f;
    MoltenPlate.ItemType = EItemType::Equipment;
    MoltenPlate.bIsStackable = false;
    MoltenPlate.StackCount = 1;
    MoltenPlate.UseType = EItemUseType::Equipment;
    MoltenPlate.UseAction = TEXT("Equip Armor");
    MoltenPlate.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    MoltenPlate.EquipmentAttributes.Armor = 14;
    MoltenPlate.EquipmentAttributes.DistanceSkillBonus = 3;
    RegisterItem(MoltenPlate);

    FGameItem OrientalShoes;
    OrientalShoes.Name = TEXT("Oriental Shoes");
    OrientalShoes.Description = TEXT("Elegant shoes from the eastern lands.");
    OrientalShoes.Rarity = EItemRarity::Rare;
    OrientalShoes.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/oriental_shoes_icon.oriental_shoes_icon")));
    OrientalShoes.Weight = 15.0f;
    OrientalShoes.ItemType = EItemType::Equipment;
    OrientalShoes.bIsStackable = false;
    OrientalShoes.StackCount = 1;
    OrientalShoes.UseType = EItemUseType::Equipment;
    OrientalShoes.UseAction = TEXT("Equip Boots");
    OrientalShoes.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    OrientalShoes.EquipmentAttributes.Armor = 2;
    OrientalShoes.EquipmentAttributes.MagicLevelBonus = 3;
    OrientalShoes.EquipmentAttributes.SpeedBonus = 0.02;
    RegisterItem(OrientalShoes);

    FGameItem Soulstalkers;
    Soulstalkers.Name = TEXT("Pair of Soulstalkers");
    Soulstalkers.Description = TEXT("Boots imbued with soul essence.");
    Soulstalkers.Rarity = EItemRarity::Legendary;
    Soulstalkers.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/pair_of_soulstalkers_icon.pair_of_soulstalkers_icon")));
    Soulstalkers.Weight = 24.0f;
    Soulstalkers.ItemType = EItemType::Equipment;
    Soulstalkers.bIsStackable = false;
    Soulstalkers.StackCount = 1;
    Soulstalkers.UseType = EItemUseType::Equipment;
    Soulstalkers.UseAction = TEXT("Equip Boots");
    Soulstalkers.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    Soulstalkers.EquipmentAttributes.Armor = 6;
    Soulstalkers.EquipmentAttributes.DistanceSkillBonus = 2;
    Soulstalkers.EquipmentAttributes.SpeedBonus = 0.05;
    RegisterItem(Soulstalkers);

    FGameItem Soulwalkers;
    Soulwalkers.Name = TEXT("Pair of Soulwalkers");
    Soulwalkers.Description = TEXT("Boots that carry the essence of wandering souls.");
    Soulwalkers.Rarity = EItemRarity::Legendary;
    Soulwalkers.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/soulwalkers_icon.soulwalkers_icon")));
    Soulwalkers.Weight = 26.0f;
    Soulwalkers.ItemType = EItemType::Equipment;
    Soulwalkers.bIsStackable = false;
    Soulwalkers.StackCount = 1;
    Soulwalkers.UseType = EItemUseType::Equipment;
    Soulwalkers.UseAction = TEXT("Equip Boots");
    Soulwalkers.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    Soulwalkers.EquipmentAttributes.Armor = 6;
    Soulwalkers.EquipmentAttributes.MeleeSkillBonus = 2;
    Soulwalkers.EquipmentAttributes.SpeedBonus = 0.06;
    RegisterItem(Soulwalkers);

    FGameItem PaladinArmor;
    PaladinArmor.Name = TEXT("Paladin Armor");
    PaladinArmor.Description = TEXT("Sacred armor worn by holy warriors.");
    PaladinArmor.Rarity = EItemRarity::Rare;
    PaladinArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/paladin_armor_icon.paladin_armor_icon")));
    PaladinArmor.Weight = 85.0f;
    PaladinArmor.ItemType = EItemType::Equipment;
    PaladinArmor.bIsStackable = false;
    PaladinArmor.StackCount = 1;
    PaladinArmor.UseType = EItemUseType::Equipment;
    PaladinArmor.UseAction = TEXT("Equip Armor");
    PaladinArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    PaladinArmor.EquipmentAttributes.Armor = 12;
    PaladinArmor.EquipmentAttributes.DistanceSkillBonus = 2;
    RegisterItem(PaladinArmor);

    FGameItem PlateArmor;
    PlateArmor.Name = TEXT("Plate Armor");
    PlateArmor.Description = TEXT("Heavy armor made of solid steel plates.");
    PlateArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/plate_armor_icon.plate_armor_icon")));
    PlateArmor.Weight = 120.0f;
    PlateArmor.ItemType = EItemType::Equipment;
    PlateArmor.bIsStackable = false;
    PlateArmor.StackCount = 1;
    PlateArmor.UseType = EItemUseType::Equipment;
    PlateArmor.UseAction = TEXT("Equip Armor");
    PlateArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    PlateArmor.EquipmentAttributes.Armor = 10;
    RegisterItem(PlateArmor);

    FGameItem PlateLegs;
    PlateLegs.Name = TEXT("Plate Legs");
    PlateLegs.Description = TEXT("Heavy leg protection made of steel plates.");
    PlateLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/plate_legs_icon.plate_legs_icon")));
    PlateLegs.Weight = 70.0f;
    PlateLegs.ItemType = EItemType::Equipment;
    PlateLegs.bIsStackable = false;
    PlateLegs.StackCount = 1;
    PlateLegs.UseType = EItemUseType::Equipment;
    PlateLegs.UseAction = TEXT("Equip Legs");
    PlateLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    PlateLegs.EquipmentAttributes.Armor = 7;
    RegisterItem(PlateLegs);

    FGameItem Sandals;
    Sandals.Name = TEXT("Sandals");
    Sandals.Description = TEXT("Light and comfortable footwear.");
    Sandals.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/sandals_icon.sandals_icon")));
    Sandals.Weight = 6.0f;
    Sandals.ItemType = EItemType::Equipment;
    Sandals.bIsStackable = false;
    Sandals.StackCount = 1;
    Sandals.UseType = EItemUseType::Equipment;
    Sandals.UseAction = TEXT("Equip Boots");
    Sandals.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    Sandals.EquipmentAttributes.Armor = 1;
    RegisterItem(Sandals);

    FGameItem Soulmantle;
    Soulmantle.Name = TEXT("Soulmantle");
    Soulmantle.Description = TEXT("A mystical mantle infused with soul energy.");
    Soulmantle.Rarity = EItemRarity::Artifact;
    Soulmantle.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/soulmantle_icon.soulmantle_icon")));
    Soulmantle.Weight = 85.0f;
    Soulmantle.ItemType = EItemType::Equipment;
    Soulmantle.bIsStackable = false;
    Soulmantle.StackCount = 1;
    Soulmantle.UseType = EItemUseType::Equipment;
    Soulmantle.UseAction = TEXT("Equip Armor");
    Soulmantle.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    Soulmantle.EquipmentAttributes.Armor = 9;
    Soulmantle.EquipmentAttributes.MagicLevelBonus = 11;
    RegisterItem(Soulmantle);

    FGameItem Soulshanks;
    Soulshanks.Name = TEXT("Soulshanks");
    Soulshanks.Description = TEXT("Leg armor imbued with soul essence.");
    Soulshanks.Rarity = EItemRarity::Artifact;
    Soulshanks.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/soulshanks_icon.soulshanks_icon")));
    Soulshanks.Weight = 45.0f;
    Soulshanks.ItemType = EItemType::Equipment;
    Soulshanks.bIsStackable = false;
    Soulshanks.StackCount = 1;
    Soulshanks.UseType = EItemUseType::Equipment;
    Soulshanks.UseAction = TEXT("Equip Legs");
    Soulshanks.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    Soulshanks.EquipmentAttributes.Armor = 10;
    Soulshanks.EquipmentAttributes.MagicLevelBonus = 4;
    RegisterItem(Soulshanks);

    FGameItem Soulshell;
    Soulshell.Name = TEXT("Soulshell");
    Soulshell.Description = TEXT("A protective shell infused with soul power.");
    Soulshell.Rarity = EItemRarity::Legendary;
    Soulshell.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/soulshell_icon.soulshell_icon")));
    Soulshell.Weight = 90.0f;
    Soulshell.ItemType = EItemType::Equipment;
    Soulshell.bIsStackable = false;
    Soulshell.StackCount = 1;
    Soulshell.UseType = EItemUseType::Equipment;
    Soulshell.UseAction = TEXT("Equip Armor");
    Soulshell.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    Soulshell.EquipmentAttributes.Armor = 16;
    Soulshell.EquipmentAttributes.DistanceSkillBonus = 4;
    RegisterItem(Soulshell);

    FGameItem Soulshroud;
    Soulshroud.Name = TEXT("Soulshroud");
    Soulshroud.Description = TEXT("A mystical shroud that channels soul energy.");
    Soulshroud.Rarity = EItemRarity::Artifact;
    Soulshroud.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/soulshroud_icon.soulshroud_icon")));
    Soulshroud.Weight = 88.0f;
    Soulshroud.ItemType = EItemType::Equipment;
    Soulshroud.bIsStackable = false;
    Soulshroud.StackCount = 1;
    Soulshroud.UseType = EItemUseType::Equipment;
    Soulshroud.UseAction = TEXT("Equip Armor");
    Soulshroud.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    Soulshroud.EquipmentAttributes.Armor = 9;
    Soulshroud.EquipmentAttributes.MagicLevelBonus = 12;
    RegisterItem(Soulshroud);

    FGameItem DemonShield;
    DemonShield.Name = TEXT("Demon Shield");
    DemonShield.Description = TEXT("This powerful shield is as light as air.");
    DemonShield.Rarity = EItemRarity::Legendary;
    DemonShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/demon_shield_icon.demon_shield_icon")));
    DemonShield.Weight = 26.0f;
    DemonShield.ItemType = EItemType::Equipment;
    DemonShield.bIsStackable = false;
    DemonShield.StackCount = 1;
    DemonShield.UseType = EItemUseType::Equipment;
    DemonShield.UseAction = TEXT("Equip Shield");
    DemonShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    DemonShield.EquipmentAttributes.Defense = 72;
    DemonShield.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(DemonShield);

    FGameItem RoyalHelmet;
    RoyalHelmet.Name = TEXT("Royal Helmet");
    RoyalHelmet.Description = TEXT("An excellent masterpiece of a smith.");
    RoyalHelmet.Rarity = EItemRarity::Legendary;
    RoyalHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/royal_helmet_icon.royal_helmet_icon")));
    RoyalHelmet.Weight = 48.0f;
    RoyalHelmet.ItemType = EItemType::Equipment;
    RoyalHelmet.bIsStackable = false;
    RoyalHelmet.StackCount = 1;
    RoyalHelmet.UseType = EItemUseType::Equipment;
    RoyalHelmet.UseAction = TEXT("Equip Helmet");
    RoyalHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    RoyalHelmet.EquipmentAttributes.Armor = 16;
    RoyalHelmet.EquipmentAttributes.DistanceSkillBonus = 6;
    RegisterItem(RoyalHelmet);

    
    FGameItem Arrow;
    Arrow.Name = TEXT("Arrow");
    Arrow.Description = TEXT("");
    Arrow.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/arrow_icon.arrow_icon")));
    Arrow.Weight = 0.2f;
    Arrow.ItemType = EItemType::Equipment;
    Arrow.bIsStackable = true;
    Arrow.StackCount = 1;
    Arrow.UseType = EItemUseType::Equipment;
    Arrow.UseAction = TEXT("Load Arrow");
    Arrow.EquipmentAttributes.EquipmentType = EEquipmentType::Ammunition;
    Arrow.EquipmentAttributes.ProjectileEffect = PlainArrowFinder.Object;
    Arrow.EquipmentAttributes.Attack = 20;
    RegisterItem(Arrow);

    FGameItem InfernalArrow;
    InfernalArrow.Name = TEXT("Infernal Arrow");
    InfernalArrow.Description = TEXT("An arrow engulfed by hell fire.");
    InfernalArrow.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/infernal_bolt_icon.infernal_bolt_icon")));
    InfernalArrow.Weight = 1.5f;
    InfernalArrow.ItemType = EItemType::Equipment;
    InfernalArrow.bIsStackable = true;
    InfernalArrow.StackCount = 1;
    InfernalArrow.UseType = EItemUseType::Equipment;
    InfernalArrow.UseAction = TEXT("Load Infernal Arrow");
    InfernalArrow.EquipmentAttributes.EquipmentType = EEquipmentType::Ammunition;
    InfernalArrow.EquipmentAttributes.ProjectileEffect = FlamingArrowEffectFinder.Object;
    InfernalArrow.EquipmentAttributes.Attack = 33;
    RegisterItem(InfernalArrow);

    FGameItem GiantSword;
    GiantSword.Name = TEXT("Giant Sword");
    GiantSword.Description = TEXT("This sword has been forged by ancient giants.");
    GiantSword.Rarity = EItemRarity::Epic;
    GiantSword.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/giant_sword_icon.giant_sword_icon")));
    GiantSword.Weight = 180.0f;
    GiantSword.ItemType = EItemType::Equipment;
    GiantSword.bIsStackable = false;
    GiantSword.StackCount = 1;
    GiantSword.UseType = EItemUseType::Equipment;
    GiantSword.UseAction = TEXT("Equip Sword");
    GiantSword.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    GiantSword.EquipmentAttributes.Attack = 49;
    GiantSword.EquipmentAttributes.Defense = 22;
    GiantSword.EquipmentAttributes.bIsTwoHanded = true;
    GiantSword.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(GiantSword);

    FGameItem WarHammer;
    WarHammer.Name = TEXT("War Hammer");
    WarHammer.Description = TEXT("A heavy hammer designed for crushing armor.");
    WarHammer.Rarity = EItemRarity::Junk;
    WarHammer.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/war_hammer_icon.war_hammer_icon")));
    WarHammer.Weight = 85.0f;
    WarHammer.ItemType = EItemType::Equipment;
    WarHammer.bIsStackable = false;
    WarHammer.StackCount = 1;
    WarHammer.UseType = EItemUseType::Equipment;
    WarHammer.UseAction = TEXT("Equip Hammer");
    WarHammer.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    WarHammer.EquipmentAttributes.Attack = 45;
    WarHammer.EquipmentAttributes.Defense = 20;
    WarHammer.EquipmentAttributes.bIsTwoHanded = true;
    RegisterItem(WarHammer);

    FGameItem ElvishBow;
    ElvishBow.Name = TEXT("Elvish Bow");
    ElvishBow.Description = TEXT("A light and elegant bow crafted by elven masters.");
    ElvishBow.Rarity = EItemRarity::Junk;
    ElvishBow.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/elvish_bow_icon.elvish_bow_icon")));
    ElvishBow.Weight = 90.0f;
    ElvishBow.ItemType = EItemType::Equipment;
    ElvishBow.bIsStackable = false;
    ElvishBow.StackCount = 1;
    ElvishBow.UseType = EItemUseType::Equipment;
    ElvishBow.UseAction = TEXT("Equip Bow");
    ElvishBow.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    ElvishBow.EquipmentAttributes.Attack = 25;
    ElvishBow.EquipmentAttributes.bCanFireProjectiles = true;
    ElvishBow.EquipmentAttributes.bIsTwoHanded = true;
    RegisterItem(ElvishBow);

    FGameItem BootsOfHaste;
    BootsOfHaste.Name = TEXT("Boots of Haste");
    BootsOfHaste.Description = TEXT("A pair of boots with enchanted wings that increase the wearer's speed.");
    BootsOfHaste.Rarity = EItemRarity::Rare;
    BootsOfHaste.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/boots_of_haste_icon.boots_of_haste_icon")));
    BootsOfHaste.Weight = 7.5f;
    BootsOfHaste.ItemType = EItemType::Equipment;
    BootsOfHaste.bIsStackable = false;
    BootsOfHaste.StackCount = 1;
    BootsOfHaste.UseType = EItemUseType::Equipment;
    BootsOfHaste.UseAction = TEXT("Equip Boots");
    BootsOfHaste.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    BootsOfHaste.EquipmentAttributes.SpeedBonus = 0.10f;
    RegisterItem(BootsOfHaste);

    
    FGameItem WarlordSword;
    WarlordSword.Name = TEXT("Warlord Sword");
    WarlordSword.Description = TEXT("A mighty sword wielded by legendary warlords.");
    WarlordSword.Rarity = EItemRarity::Artifact;
    WarlordSword.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/warlord_sword_icon.warlord_sword_icon")));
    WarlordSword.Weight = 65.0f;
    WarlordSword.ItemType = EItemType::Equipment;
    WarlordSword.bIsStackable = false;
    WarlordSword.StackCount = 1;
    WarlordSword.UseType = EItemUseType::Equipment;
    WarlordSword.UseAction = TEXT("Equip Sword");
    WarlordSword.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    WarlordSword.EquipmentAttributes.bIsTwoHanded = true;
    WarlordSword.EquipmentAttributes.Attack = 59;
    WarlordSword.EquipmentAttributes.Defense = 38;
    WarlordSword.EquipmentAttributes.MeleeSkillBonus = 10;
    RegisterItem(WarlordSword);

    
    FGameItem ThunderHammer;
    ThunderHammer.Name = TEXT("Thunder Hammer");
    ThunderHammer.Description = TEXT("It is blessed by the gods of Tib... I mean, it's just a hammer.");
    ThunderHammer.Rarity = EItemRarity::Artifact;
    ThunderHammer.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/thunder_hammer_icon.thunder_hammer_icon")));
    ThunderHammer.Weight = 120.0f;
    ThunderHammer.ItemType = EItemType::Equipment;
    ThunderHammer.bIsStackable = false;
    ThunderHammer.StackCount = 1;
    ThunderHammer.UseType = EItemUseType::Equipment;
    ThunderHammer.UseAction = TEXT("Equip Hammer");
    ThunderHammer.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    ThunderHammer.EquipmentAttributes.Attack = 54;
    ThunderHammer.EquipmentAttributes.Defense = 14;
    ThunderHammer.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(ThunderHammer);

    
    FGameItem Epiphany;
    Epiphany.Name = TEXT("The Epiphany");
    Epiphany.Description = TEXT("A legendary sword of divine inspiration.");
    Epiphany.Rarity = EItemRarity::Legendary;
    Epiphany.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/the_epiphany_icon.the_epiphany_icon")));
    Epiphany.Weight = 95.0f;
    Epiphany.ItemType = EItemType::Equipment;
    Epiphany.bIsStackable = false;
    Epiphany.StackCount = 1;
    Epiphany.UseType = EItemUseType::Equipment;
    Epiphany.UseAction = TEXT("Equip Sword");
    Epiphany.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Epiphany.EquipmentAttributes.Attack = 54;
    Epiphany.EquipmentAttributes.Defense = 35;
    Epiphany.EquipmentAttributes.MeleeSkillBonus = 6;
    RegisterItem(Epiphany);

    
    FGameItem Avenger;
    Avenger.Name = TEXT("The Avenger");
    Avenger.Description = TEXT("A sword that seeks vengeance for its wielder.");
    Avenger.Rarity = EItemRarity::Legendary;
    Avenger.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/the_avenger_icon.the_avenger_icon")));
    Avenger.Weight = 85.0f;
    Avenger.ItemType = EItemType::Equipment;
    Avenger.bIsStackable = false;
    Avenger.StackCount = 1;
    Avenger.UseType = EItemUseType::Equipment;
    Avenger.UseAction = TEXT("Equip Sword");
    Avenger.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Avenger.EquipmentAttributes.bIsTwoHanded = true;
    Avenger.EquipmentAttributes.Attack = 52;
    Avenger.EquipmentAttributes.Defense = 38;
    Avenger.EquipmentAttributes.MeleeSkillBonus = 4;
    RegisterItem(Avenger);

    
    FGameItem Calamity;
    Calamity.Name = TEXT("The Calamity");
    Calamity.Description = TEXT("A devastating weapon that brings doom to its enemies.");
    Calamity.Rarity = EItemRarity::Legendary;
    Calamity.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/the_calamity_icon.the_calamity_icon")));
    Calamity.Weight = 105.0f;
    Calamity.ItemType = EItemType::Equipment;
    Calamity.bIsStackable = false;
    Calamity.StackCount = 1;
    Calamity.UseType = EItemUseType::Equipment;
    Calamity.UseAction = TEXT("Equip Sword");
    Calamity.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Calamity.EquipmentAttributes.bIsTwoHanded = true;
    Calamity.EquipmentAttributes.Attack = 53;
    Calamity.EquipmentAttributes.Defense = 35;
    Calamity.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(Calamity);

    
    FGameItem ThaianSword;
    ThaianSword.Name = TEXT("Thaian Sword");
    ThaianSword.Description = TEXT("A masterfully crafted sword from the city of Thais.");
    ThaianSword.Rarity = EItemRarity::Legendary;
    ThaianSword.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/thaian_sword_icon.thaian_sword_icon")));
    ThaianSword.Weight = 75.0f;
    ThaianSword.ItemType = EItemType::Equipment;
    ThaianSword.bIsStackable = false;
    ThaianSword.StackCount = 1;
    ThaianSword.UseType = EItemUseType::Equipment;
    ThaianSword.UseAction = TEXT("Equip Sword");
    ThaianSword.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    ThaianSword.EquipmentAttributes.Attack = 45;
    ThaianSword.EquipmentAttributes.Defense = 30;
    RegisterItem(ThaianSword);

    
    FGameItem StonecutterAxe;
    StonecutterAxe.Name = TEXT("Stonecutter Axe");
    StonecutterAxe.Description = TEXT("You feel the power of this mighty axe.");
    StonecutterAxe.Rarity = EItemRarity::Legendary;
    StonecutterAxe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/stonecutter_axe_icon.stonecutter_axe_icon")));
    StonecutterAxe.Weight = 110.0f;
    StonecutterAxe.ItemType = EItemType::Equipment;
    StonecutterAxe.bIsStackable = false;
    StonecutterAxe.StackCount = 1;
    StonecutterAxe.UseType = EItemUseType::Equipment;
    StonecutterAxe.UseAction = TEXT("Equip Axe");
    StonecutterAxe.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    StonecutterAxe.EquipmentAttributes.Attack = 51;
    StonecutterAxe.EquipmentAttributes.Defense = 30;
    StonecutterAxe.EquipmentAttributes.MeleeSkillBonus = 4;
    RegisterItem(StonecutterAxe);

    
    FGameItem Soulcutter;
    Soulcutter.Name = TEXT("Soulcutter");
    Soulcutter.Description = TEXT("A mystical sword said to cleave through both flesh and spirit.");
    Soulcutter.Rarity = EItemRarity::Artifact;
    Soulcutter.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/soulcutter_icon.soulcutter_icon")));
    Soulcutter.Weight = 95.0f;
    Soulcutter.ItemType = EItemType::Equipment;
    Soulcutter.bIsStackable = false;
    Soulcutter.StackCount = 1;
    Soulcutter.UseType = EItemUseType::Equipment;
    Soulcutter.UseAction = TEXT("Equip Sword");
    Soulcutter.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Soulcutter.EquipmentAttributes.Attack = 54;
    Soulcutter.EquipmentAttributes.Defense = 36;
    Soulcutter.EquipmentAttributes.MeleeSkillBonus = 6;
    RegisterItem(Soulcutter);

    
    FGameItem MagicSword;
    MagicSword.Name = TEXT("Magic Sword");
    MagicSword.Description = TEXT("It's the Sword of Valor.");
    MagicSword.Rarity = EItemRarity::Legendary;
    MagicSword.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magic_sword_icon.magic_sword_icon")));
    MagicSword.Weight = 42.0f;
    MagicSword.ItemType = EItemType::Equipment;
    MagicSword.bIsStackable = false;
    MagicSword.StackCount = 1;
    MagicSword.UseType = EItemUseType::Equipment;
    MagicSword.UseAction = TEXT("Equip Sword");
    MagicSword.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    MagicSword.EquipmentAttributes.Attack = 50;
    MagicSword.EquipmentAttributes.Defense = 35;
    MagicSword.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(MagicSword);

    
    FGameItem HellforgedAxe;
    HellforgedAxe.Name = TEXT("Hellforged Axe");
    HellforgedAxe.Description = TEXT("An axe forged in the fires of the underworld.");
    HellforgedAxe.Rarity = EItemRarity::Legendary;
    HellforgedAxe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/hellforged_axe_icon.hellforged_axe_icon")));
    HellforgedAxe.Weight = 115.0f;
    HellforgedAxe.ItemType = EItemType::Equipment;
    HellforgedAxe.bIsStackable = false;
    HellforgedAxe.StackCount = 1;
    HellforgedAxe.UseType = EItemUseType::Equipment;
    HellforgedAxe.UseAction = TEXT("Equip Axe");
    HellforgedAxe.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    HellforgedAxe.EquipmentAttributes.Attack = 51;
    HellforgedAxe.EquipmentAttributes.Defense = 22;
    HellforgedAxe.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(HellforgedAxe);

    
    FGameItem GoldenWarlordSword;
    GoldenWarlordSword.Name = TEXT("Golden Warlord Sword");
    GoldenWarlordSword.Description = TEXT("A magnificent golden version of the legendary warlord sword.");
    GoldenWarlordSword.Rarity = EItemRarity::Artifact;
    GoldenWarlordSword.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/golden_warlord_sword_icon.golden_warlord_sword_icon")));
    GoldenWarlordSword.Weight = 70.0f;
    GoldenWarlordSword.ItemType = EItemType::Equipment;
    GoldenWarlordSword.bIsStackable = false;
    GoldenWarlordSword.StackCount = 1;
    GoldenWarlordSword.UseType = EItemUseType::Equipment;
    GoldenWarlordSword.UseAction = TEXT("Equip Sword");
    GoldenWarlordSword.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    GoldenWarlordSword.EquipmentAttributes.bIsTwoHanded = true;
    GoldenWarlordSword.EquipmentAttributes.Attack = 59;
    GoldenWarlordSword.EquipmentAttributes.Defense = 38;
    GoldenWarlordSword.EquipmentAttributes.MeleeSkillBonus = 10;
    RegisterItem(GoldenWarlordSword);

    
    FGameItem BrightSword;
    BrightSword.Name = TEXT("Bright Sword");
    BrightSword.Description = TEXT("A sword that gleams with an inner light.");
    BrightSword.Rarity = EItemRarity::Uncommon;
    BrightSword.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/bright_sword_icon.bright_sword_icon")));
    BrightSword.Weight = 45.0f;
    BrightSword.ItemType = EItemType::Equipment;
    BrightSword.bIsStackable = false;
    BrightSword.StackCount = 1;
    BrightSword.UseType = EItemUseType::Equipment;
    BrightSword.UseAction = TEXT("Equip Sword");
    BrightSword.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    BrightSword.EquipmentAttributes.Attack = 36;
    BrightSword.EquipmentAttributes.Defense = 30;
    RegisterItem(BrightSword);

    FGameItem Soulstrider;
    Soulstrider.Name = TEXT("Soulstrider");
    Soulstrider.Description = TEXT("Mage legs that harness the power of wandering souls.");
    Soulstrider.Rarity = EItemRarity::Epic;
    Soulstrider.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/soulstrider_icon.soulstrider_icon")));
    Soulstrider.Weight = 28.0f;
    Soulstrider.ItemType = EItemType::Equipment;
    Soulstrider.bIsStackable = false;
    Soulstrider.StackCount = 1;
    Soulstrider.UseType = EItemUseType::Equipment;
    Soulstrider.UseAction = TEXT("Equip Legs");
    Soulstrider.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    Soulstrider.EquipmentAttributes.Armor = 15;
    Soulstrider.EquipmentAttributes.MagicLevelBonus = 3;
    Soulstrider.EquipmentAttributes.DistanceSkillBonus = 3;
    RegisterItem(Soulstrider);

    FGameItem SteelBoots;
    SteelBoots.Name = TEXT("Steel Boots");
    SteelBoots.Description = TEXT("Heavy boots made of solid steel.");
    SteelBoots.Rarity = EItemRarity::Epic;
    SteelBoots.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/steel_boots_icon.steel_boots_icon")));
    SteelBoots.Weight = 29.0f;
    SteelBoots.ItemType = EItemType::Equipment;
    SteelBoots.bIsStackable = false;
    SteelBoots.StackCount = 1;
    SteelBoots.UseType = EItemUseType::Equipment;
    SteelBoots.UseAction = TEXT("Equip Boots");
    SteelBoots.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    SteelBoots.EquipmentAttributes.Armor = 5;
    RegisterItem(SteelBoots);

    FGameItem SteelHelmet;
    SteelHelmet.Name = TEXT("Steel Helmet");
    SteelHelmet.Description = TEXT("A sturdy helmet made of steel.");
    SteelHelmet.Rarity = EItemRarity::Junk;
    SteelHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/steel_helmet_icon.steel_helmet_icon")));
    SteelHelmet.Weight = 46.0f;
    SteelHelmet.ItemType = EItemType::Equipment;
    SteelHelmet.bIsStackable = false;
    SteelHelmet.StackCount = 1;
    SteelHelmet.UseType = EItemUseType::Equipment;
    SteelHelmet.UseAction = TEXT("Equip Helmet");
    SteelHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    SteelHelmet.EquipmentAttributes.Armor = 6;
    RegisterItem(SteelHelmet);

    FGameItem TerraBoots;
    TerraBoots.Name = TEXT("Terra Boots");
    TerraBoots.Description = TEXT("Boots imbued with earth magic.");
    TerraBoots.Rarity = EItemRarity::Rare;
    TerraBoots.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/terra_boots_icon.terra_boots_icon")));
    TerraBoots.Weight = 24.0f;
    TerraBoots.ItemType = EItemType::Equipment;
    TerraBoots.bIsStackable = false;
    TerraBoots.StackCount = 1;
    TerraBoots.UseType = EItemUseType::Equipment;
    TerraBoots.UseAction = TEXT("Equip Boots");
    TerraBoots.EquipmentAttributes.EquipmentType = EEquipmentType::Boots;
    TerraBoots.EquipmentAttributes.Armor = 2;
    TerraBoots.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(TerraBoots);

    FGameItem TerraHood;
    TerraHood.Name = TEXT("Terra Hood");
    TerraHood.Description = TEXT("A hood enchanted with earth magic.");
    TerraHood.Rarity = EItemRarity::Uncommon;
    TerraHood.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/terra_hood_icon.terra_hood_icon")));
    TerraHood.Weight = 18.0f;
    TerraHood.ItemType = EItemType::Equipment;
    TerraHood.bIsStackable = false;
    TerraHood.StackCount = 1;
    TerraHood.UseType = EItemUseType::Equipment;
    TerraHood.UseAction = TEXT("Equip Helmet");
    TerraHood.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    TerraHood.EquipmentAttributes.Armor = 4;
    TerraHood.EquipmentAttributes.MagicLevelBonus = 4;
    RegisterItem(TerraHood);

    FGameItem TerraLegs;
    TerraLegs.Name = TEXT("Terra Legs");
    TerraLegs.Description = TEXT("Leg protection imbued with earth magic.");
    TerraLegs.Rarity = EItemRarity::Common;
    TerraLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/terra_legs_icon.terra_legs_icon")));
    TerraLegs.Weight = 42.0f;
    TerraLegs.ItemType = EItemType::Equipment;
    TerraLegs.bIsStackable = false;
    TerraLegs.StackCount = 1;
    TerraLegs.UseType = EItemUseType::Equipment;
    TerraLegs.UseAction = TEXT("Equip Legs");
    TerraLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    TerraLegs.EquipmentAttributes.Armor = 6;
    TerraLegs.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(TerraLegs);

    FGameItem TogaMortis;
    TogaMortis.Name = TEXT("Toga Mortis");
    TogaMortis.Description = TEXT("A dark toga that radiates death magic.");
    TogaMortis.Rarity = EItemRarity::Legendary;
    TogaMortis.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/toga_mortis_icon.toga_mortis_icon")));
    TogaMortis.Weight = 45.0f;
    TogaMortis.ItemType = EItemType::Equipment;
    TogaMortis.bIsStackable = false;
    TogaMortis.StackCount = 1;
    TogaMortis.UseType = EItemUseType::Equipment;
    TogaMortis.UseAction = TEXT("Equip Armor");
    TogaMortis.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    TogaMortis.EquipmentAttributes.Armor = 9;
    TogaMortis.EquipmentAttributes.MagicLevelBonus = 10;
    RegisterItem(TogaMortis);

    FGameItem WarriorHelmet;
    WarriorHelmet.Name = TEXT("Warrior Helmet");
    WarriorHelmet.Description = TEXT("A sturdy helmet worn by seasoned warriors.");
    WarriorHelmet.Rarity = EItemRarity::Uncommon;
    WarriorHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/warrior_helmet_icon.warrior_helmet_icon")));
    WarriorHelmet.Weight = 68.0f;
    WarriorHelmet.ItemType = EItemType::Equipment;
    WarriorHelmet.bIsStackable = false;
    WarriorHelmet.StackCount = 1;
    WarriorHelmet.UseType = EItemUseType::Equipment;
    WarriorHelmet.UseAction = TEXT("Equip Helmet");
    WarriorHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    WarriorHelmet.EquipmentAttributes.Armor = 10;
    WarriorHelmet.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(WarriorHelmet);

    FGameItem WingedHelmet;
    WingedHelmet.Name = TEXT("Winged Helmet");
    WingedHelmet.Description = TEXT("A majestic helmet adorned with wings.");
    WingedHelmet.Rarity = EItemRarity::Artifact;
    WingedHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/winged_helmet_icon.winged_helmet_icon")));
    WingedHelmet.Weight = 32.0f;
    WingedHelmet.ItemType = EItemType::Equipment;
    WingedHelmet.bIsStackable = false;
    WingedHelmet.StackCount = 1;
    WingedHelmet.UseType = EItemUseType::Equipment;
    WingedHelmet.UseAction = TEXT("Equip Helmet");
    WingedHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    WingedHelmet.EquipmentAttributes.Armor = 18;
    WingedHelmet.EquipmentAttributes.DistanceSkillBonus = 10;
    RegisterItem(WingedHelmet);

    FGameItem ZaoanHelmet;
    ZaoanHelmet.Name = TEXT("Zaoan Helmet");
    ZaoanHelmet.Description = TEXT("A helmet crafted by the ancient Zaoan civilization.");
    ZaoanHelmet.Rarity = EItemRarity::Epic;
    ZaoanHelmet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/zaoan_helmet_icon.zaoan_helmet_icon")));
    ZaoanHelmet.Weight = 55.0f;
    ZaoanHelmet.ItemType = EItemType::Equipment;
    ZaoanHelmet.bIsStackable = false;
    ZaoanHelmet.StackCount = 1;
    ZaoanHelmet.UseType = EItemUseType::Equipment;
    ZaoanHelmet.UseAction = TEXT("Equip Helmet");
    ZaoanHelmet.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    ZaoanHelmet.EquipmentAttributes.Armor = 14;
    ZaoanHelmet.EquipmentAttributes.MeleeSkillBonus = 4;
    ZaoanHelmet.EquipmentAttributes.DistanceSkillBonus = 2;
    RegisterItem(ZaoanHelmet);

    FGameItem StoneSkinAmulet;
    StoneSkinAmulet.Name = TEXT("Stone Skin Amulet");
    StoneSkinAmulet.Description = TEXT("A magical amulet that hardens your skin like stone.");
    StoneSkinAmulet.Rarity = EItemRarity::Common;
    StoneSkinAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/stone_skin_amulet_icon.stone_skin_amulet_icon")));
    StoneSkinAmulet.Weight = 7.2f;
    StoneSkinAmulet.ItemType = EItemType::Equipment;
    StoneSkinAmulet.bIsStackable = false;
    StoneSkinAmulet.StackCount = 1;
    StoneSkinAmulet.UseType = EItemUseType::Equipment;
    StoneSkinAmulet.UseAction = TEXT("Equip Amulet");
    StoneSkinAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    StoneSkinAmulet.EquipmentAttributes.DamageReductionPercent = 0.8f;
    StoneSkinAmulet.EquipmentAttributes.Charges = 5;
    RegisterItem(StoneSkinAmulet);

    
    FGameItem AmuletOfLoss;
    AmuletOfLoss.Name = TEXT("Amulet of Loss");
    AmuletOfLoss.Description = TEXT("A mysterious amulet that protects against item loss on death.");
    AmuletOfLoss.Rarity = EItemRarity::Uncommon;
    AmuletOfLoss.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/amulet_of_loss_icon.amulet_of_loss_icon")));
    AmuletOfLoss.Weight = 2.8f;
    AmuletOfLoss.ItemType = EItemType::Equipment;
    AmuletOfLoss.bIsStackable = false;
    AmuletOfLoss.StackCount = 1;
    AmuletOfLoss.UseType = EItemUseType::Equipment;
    AmuletOfLoss.UseAction = TEXT("Equip Amulet");
    AmuletOfLoss.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    AmuletOfLoss.EquipmentAttributes.bPreventItemLoss = true;
    RegisterItem(AmuletOfLoss);

    FGameItem AncientAmulet;
    AncientAmulet.Name = TEXT("Ancient Amulet");
    AncientAmulet.Description = TEXT("An amulet embued with mystical powers from ancient Egypt.");
    AncientAmulet.Rarity = EItemRarity::Rare;
    AncientAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ancient_amulet_icon.ancient_amulet_icon")));
    AncientAmulet.Weight = 4.1f;
    AncientAmulet.ItemType = EItemType::Equipment;
    AncientAmulet.bIsStackable = false;
    AncientAmulet.StackCount = 1;
    AncientAmulet.UseType = EItemUseType::Equipment;
    AncientAmulet.UseAction = TEXT("Equip Amulet");
    AncientAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    AncientAmulet.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(AncientAmulet);

    FGameItem CrystalNecklace;
    CrystalNecklace.Name = TEXT("Crystal Necklace");
    CrystalNecklace.Description = TEXT("A necklace made of pure crystal that enhances magical abilities.");
    CrystalNecklace.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crystal_necklace_icon.crystal_necklace_icon")));
    CrystalNecklace.Weight = 4.9f;
    CrystalNecklace.ItemType = EItemType::Equipment;
    CrystalNecklace.bIsStackable = false;
    CrystalNecklace.StackCount = 1;
    CrystalNecklace.UseType = EItemUseType::Equipment;
    CrystalNecklace.UseAction = TEXT("Equip Amulet");
    CrystalNecklace.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    CrystalNecklace.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(CrystalNecklace);

    FGameItem DragonNecklace;
    DragonNecklace.Name = TEXT("Dragon Necklace");
    DragonNecklace.Description = TEXT("A necklace forged with dragon magic.");
    DragonNecklace.Rarity = EItemRarity::Epic;
    DragonNecklace.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_necklace_icon.dragon_necklace_icon")));
    DragonNecklace.Weight = 5.2f;
    DragonNecklace.ItemType = EItemType::Equipment;
    DragonNecklace.bIsStackable = false;
    DragonNecklace.StackCount = 1;
    DragonNecklace.UseType = EItemUseType::Equipment;
    DragonNecklace.UseAction = TEXT("Equip Amulet");
    DragonNecklace.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    DragonNecklace.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(DragonNecklace);

    FGameItem KosheiAncientAmulet;
    KosheiAncientAmulet.Name = TEXT("Koshei's Ancient Amulet");
    KosheiAncientAmulet.Description = TEXT("A powerful amulet once worn by the dreaded Koshei.");
    KosheiAncientAmulet.Rarity = EItemRarity::Legendary;
    KosheiAncientAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/koshei_ancient_amulet_icon.koshei_ancient_amulet_icon")));
    KosheiAncientAmulet.Weight = 4.7f;
    KosheiAncientAmulet.ItemType = EItemType::Equipment;
    KosheiAncientAmulet.bIsStackable = false;
    KosheiAncientAmulet.StackCount = 1;
    KosheiAncientAmulet.UseType = EItemUseType::Equipment;
    KosheiAncientAmulet.UseAction = TEXT("Equip Amulet");
    KosheiAncientAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    KosheiAncientAmulet.EquipmentAttributes.MagicLevelBonus = 3;
    KosheiAncientAmulet.EquipmentAttributes.DamageReductionPercent = 0.1f;
    RegisterItem(KosheiAncientAmulet);

    FGameItem ProtectionAmulet;
    ProtectionAmulet.Name = TEXT("Protection Amulet");
    ProtectionAmulet.Description = TEXT("An amulet that provides general protection against harm.");
    ProtectionAmulet.Rarity = EItemRarity::Rare;
    ProtectionAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/protection_amulet_icon.protection_amulet_icon")));
    ProtectionAmulet.Weight = 4.0f;
    ProtectionAmulet.ItemType = EItemType::Equipment;
    ProtectionAmulet.bIsStackable = false;
    ProtectionAmulet.StackCount = 1;
    ProtectionAmulet.UseType = EItemUseType::Equipment;
    ProtectionAmulet.UseAction = TEXT("Equip Amulet");
    ProtectionAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    ProtectionAmulet.EquipmentAttributes.DamageReductionPercent = 0.05f;
    ProtectionAmulet.EquipmentAttributes.Charges = 100;
    RegisterItem(ProtectionAmulet);

    FGameItem SapphireAmulet;
    SapphireAmulet.Name = TEXT("Sapphire Amulet");
    SapphireAmulet.Description = TEXT("An amulet made with a pure sapphire gem.");
    SapphireAmulet.Rarity = EItemRarity::Epic;
    SapphireAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/sapphire_amulet_icon.sapphire_amulet_icon")));
    SapphireAmulet.Weight = 4.2f;
    SapphireAmulet.ItemType = EItemType::Equipment;
    SapphireAmulet.bIsStackable = false;
    SapphireAmulet.StackCount = 1;
    SapphireAmulet.UseType = EItemUseType::Equipment;
    SapphireAmulet.UseAction = TEXT("Equip Amulet");
    SapphireAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    SapphireAmulet.EquipmentAttributes.MagicLevelBonus = 3;
    RegisterItem(SapphireAmulet);

    FGameItem ScarabAmulet;
    ScarabAmulet.Name = TEXT("Scarab Amulet");
    ScarabAmulet.Description = TEXT("An ancient amulet in the shape of a scarab beetle.");
    ScarabAmulet.Rarity = EItemRarity::Epic;
    ScarabAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/scarab_amulet_icon.scarab_amulet_icon")));
    ScarabAmulet.Weight = 3.9f;
    ScarabAmulet.ItemType = EItemType::Equipment;
    ScarabAmulet.bIsStackable = false;
    ScarabAmulet.StackCount = 1;
    ScarabAmulet.UseType = EItemUseType::Equipment;
    ScarabAmulet.UseAction = TEXT("Equip Amulet");
    ScarabAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    ScarabAmulet.EquipmentAttributes.DistanceSkillBonus = 3;
    RegisterItem(ScarabAmulet);

    FGameItem TheCobraAmulet;
    TheCobraAmulet.Name = TEXT("The Cobra Amulet");
    TheCobraAmulet.Description = TEXT("An amulet that grants the agility and swiftness of a cobra.");
    TheCobraAmulet.Rarity = EItemRarity::Legendary;
    TheCobraAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/the_cobra_amulet_icon.the_cobra_amulet_icon")));
    TheCobraAmulet.Weight = 4.5f;
    TheCobraAmulet.ItemType = EItemType::Equipment;
    TheCobraAmulet.bIsStackable = false;
    TheCobraAmulet.StackCount = 1;
    TheCobraAmulet.UseType = EItemUseType::Equipment;
    TheCobraAmulet.UseAction = TEXT("Equip Amulet");
    TheCobraAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    ScarabAmulet.EquipmentAttributes.DistanceSkillBonus = 4;
    RegisterItem(TheCobraAmulet);

    FGameItem ButterflyRing;
    ButterflyRing.Name = TEXT("Butterfly Ring");
    ButterflyRing.Description = TEXT("A delicate ring that enhances the wearer's swiftness.");
    ButterflyRing.Rarity = EItemRarity::Rare;
    ButterflyRing.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/butterfly_ring_icon.butterfly_ring_icon")));
    ButterflyRing.Weight = 0.7f;
    ButterflyRing.ItemType = EItemType::Equipment;
    ButterflyRing.bIsStackable = false;
    ButterflyRing.StackCount = 1;
    ButterflyRing.UseType = EItemUseType::Equipment;
    ButterflyRing.UseAction = TEXT("Equip Ring");
    ButterflyRing.EquipmentAttributes.EquipmentType = EEquipmentType::Ring;
    ButterflyRing.EquipmentAttributes.DistanceSkillBonus = 1;
    ButterflyRing.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(ButterflyRing);

    FGameItem CrystalRing;
    CrystalRing.Name = TEXT("Crystal Ring");
    CrystalRing.Description = TEXT("A ring made of pure crystal that enhances magical power.");
    CrystalRing.Rarity = EItemRarity::Epic;
    CrystalRing.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crystal_ring_icon.crystal_ring_icon")));
    CrystalRing.Weight = 0.9f;
    CrystalRing.ItemType = EItemType::Equipment;
    CrystalRing.bIsStackable = false;
    CrystalRing.StackCount = 1;
    CrystalRing.UseType = EItemUseType::Equipment;
    CrystalRing.UseAction = TEXT("Equip Ring");
    CrystalRing.EquipmentAttributes.EquipmentType = EEquipmentType::Ring;
    CrystalRing.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(CrystalRing);

    FGameItem GoldRing;
    GoldRing.Name = TEXT("Gold Ring");
    GoldRing.Description = TEXT("A simple ring made of pure gold.");
    GoldRing.Rarity = EItemRarity::Rare;
    GoldRing.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/gold_ring_icon.gold_ring_icon")));
    GoldRing.Weight = 1.0f;
    GoldRing.ItemType = EItemType::Equipment;
    GoldRing.bIsStackable = false;
    GoldRing.StackCount = 1;
    GoldRing.UseType = EItemUseType::Equipment;
    GoldRing.UseAction = TEXT("Equip Ring");
    GoldRing.EquipmentAttributes.EquipmentType = EEquipmentType::Ring;
    RegisterItem(GoldRing);

    FGameItem MightRing;
    MightRing.Name = TEXT("Might Ring");
    MightRing.Description = TEXT("A ring that enhances physical strength.");
    MightRing.Rarity = EItemRarity::Rare;
    MightRing.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/might_ring_icon.might_ring_icon")));
    MightRing.Weight = 0.9f;
    MightRing.ItemType = EItemType::Equipment;
    MightRing.bIsStackable = false;
    MightRing.StackCount = 1;
    MightRing.UseType = EItemUseType::Equipment;
    MightRing.UseAction = TEXT("Equip Ring");
    MightRing.EquipmentAttributes.EquipmentType = EEquipmentType::Ring;
    MightRing.EquipmentAttributes.DamageReductionPercent = 0.20f;
    MightRing.EquipmentAttributes.Charges = 20;
    RegisterItem(MightRing);

    FGameItem RingOfTheSky;
    RingOfTheSky.Name = TEXT("Ring of the Sky");
    RingOfTheSky.Description = TEXT("A mystical ring with the power of the heavens.");
    RingOfTheSky.Rarity = EItemRarity::Legendary;
    RingOfTheSky.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ring_of_the_sky_icon.ring_of_the_sky_icon")));
    RingOfTheSky.Weight = 0.8f;
    RingOfTheSky.ItemType = EItemType::Equipment;
    RingOfTheSky.bIsStackable = false;
    RingOfTheSky.StackCount = 1;
    RingOfTheSky.UseType = EItemUseType::Equipment;
    RingOfTheSky.UseAction = TEXT("Equip Ring");
    RingOfTheSky.EquipmentAttributes.EquipmentType = EEquipmentType::Ring;
    RingOfTheSky.EquipmentAttributes.MagicLevelBonus = 3;
    RegisterItem(RingOfTheSky);

    
    FGameItem LionSpellbook;
    LionSpellbook.Name = TEXT("Lion Spellbook");
    LionSpellbook.Description = TEXT("A spellbook adorned with a majestic lion emblem.");
    LionSpellbook.Rarity = EItemRarity::Rare;
    LionSpellbook.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/lion_spellbook_icon.lion_spellbook_icon")));
    LionSpellbook.Weight = 28.0f;
    LionSpellbook.ItemType = EItemType::Equipment;
    LionSpellbook.bIsStackable = false;
    LionSpellbook.StackCount = 1;
    LionSpellbook.UseType = EItemUseType::Equipment;
    LionSpellbook.UseAction = TEXT("Equip Spellbook");
    LionSpellbook.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    LionSpellbook.EquipmentAttributes.MagicLevelBonus = 7;
    LionSpellbook.EquipmentAttributes.Defense = 14;
    RegisterItem(LionSpellbook);

    FGameItem SpellbookOfDarkMysteries;
    SpellbookOfDarkMysteries.Name = TEXT("Spellbook of Dark Mysteries");
    SpellbookOfDarkMysteries.Description = TEXT("A spellbook containing forbidden knowledge.");
    SpellbookOfDarkMysteries.Rarity = EItemRarity::Artifact;
    SpellbookOfDarkMysteries.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spellbook_of_dark_mysteries_icon.spellbook_of_dark_mysteries_icon")));
    SpellbookOfDarkMysteries.Weight = 32.0f;
    SpellbookOfDarkMysteries.ItemType = EItemType::Equipment;
    SpellbookOfDarkMysteries.bIsStackable = false;
    SpellbookOfDarkMysteries.StackCount = 1;
    SpellbookOfDarkMysteries.UseType = EItemUseType::Equipment;
    SpellbookOfDarkMysteries.UseAction = TEXT("Equip Spellbook");
    SpellbookOfDarkMysteries.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SpellbookOfDarkMysteries.EquipmentAttributes.MagicLevelBonus = 15;
    SpellbookOfDarkMysteries.EquipmentAttributes.Defense = 16;
    RegisterItem(SpellbookOfDarkMysteries);

    FGameItem SpellbookOfEnlightenment;
    SpellbookOfEnlightenment.Name = TEXT("Spellbook of Enlightenment");
    SpellbookOfEnlightenment.Description = TEXT("A spellbook radiating with divine wisdom.");
    SpellbookOfEnlightenment.Rarity = EItemRarity::Common;
    SpellbookOfEnlightenment.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spellbook_of_enlightenment_icon.spellbook_of_enlightenment_icon")));
    SpellbookOfEnlightenment.Weight = 29.0f;
    SpellbookOfEnlightenment.ItemType = EItemType::Equipment;
    SpellbookOfEnlightenment.bIsStackable = false;
    SpellbookOfEnlightenment.StackCount = 1;
    SpellbookOfEnlightenment.UseType = EItemUseType::Equipment;
    SpellbookOfEnlightenment.UseAction = TEXT("Equip Spellbook");
    SpellbookOfEnlightenment.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SpellbookOfEnlightenment.EquipmentAttributes.MagicLevelBonus = 2;
    SpellbookOfEnlightenment.EquipmentAttributes.Defense = 15;
    RegisterItem(SpellbookOfEnlightenment);

    FGameItem SpellbookOfLostSouls;
    SpellbookOfLostSouls.Name = TEXT("Spellbook of Lost Souls");
    SpellbookOfLostSouls.Description = TEXT("A spellbook containing the essence of lost souls.");
    SpellbookOfLostSouls.Rarity = EItemRarity::Epic;
    SpellbookOfLostSouls.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spellbook_of_lost_souls_icon.spellbook_of_lost_souls_icon")));
    SpellbookOfLostSouls.Weight = 31.0f;
    SpellbookOfLostSouls.ItemType = EItemType::Equipment;
    SpellbookOfLostSouls.bIsStackable = false;
    SpellbookOfLostSouls.StackCount = 1;
    SpellbookOfLostSouls.UseType = EItemUseType::Equipment;
    SpellbookOfLostSouls.UseAction = TEXT("Equip Spellbook");
    SpellbookOfLostSouls.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SpellbookOfLostSouls.EquipmentAttributes.MagicLevelBonus = 9;
    SpellbookOfLostSouls.EquipmentAttributes.Defense = 16;
    RegisterItem(SpellbookOfLostSouls);

    FGameItem SpellbookOfMindControl;
    SpellbookOfMindControl.Name = TEXT("Spellbook of Mind Control");
    SpellbookOfMindControl.Description = TEXT("A spellbook containing powerful mind-altering spells.");
    SpellbookOfMindControl.Rarity = EItemRarity::Uncommon;
    SpellbookOfMindControl.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spellbook_of_mind_control_icon.spellbook_of_mind_control_icon")));
    SpellbookOfMindControl.Weight = 30.0f;
    SpellbookOfMindControl.ItemType = EItemType::Equipment;
    SpellbookOfMindControl.bIsStackable = false;
    SpellbookOfMindControl.StackCount = 1;
    SpellbookOfMindControl.UseType = EItemUseType::Equipment;
    SpellbookOfMindControl.UseAction = TEXT("Equip Spellbook");
    SpellbookOfMindControl.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SpellbookOfMindControl.EquipmentAttributes.MagicLevelBonus = 4;
    SpellbookOfMindControl.EquipmentAttributes.Defense = 15;
    RegisterItem(SpellbookOfMindControl);

    FGameItem SpellbookOfVigilance;
    SpellbookOfVigilance.Name = TEXT("Spellbook of Vigilance");
    SpellbookOfVigilance.Description = TEXT("A spellbook enhancing awareness and protective magic.");
    SpellbookOfVigilance.Rarity = EItemRarity::Rare;
    SpellbookOfVigilance.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spellbook_of_vigilance_icon.spellbook_of_vigilance_icon")));
    SpellbookOfVigilance.Weight = 29.0f;
    SpellbookOfVigilance.ItemType = EItemType::Equipment;
    SpellbookOfVigilance.bIsStackable = false;
    SpellbookOfVigilance.StackCount = 1;
    SpellbookOfVigilance.UseType = EItemUseType::Equipment;
    SpellbookOfVigilance.UseAction = TEXT("Equip Spellbook");
    SpellbookOfVigilance.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SpellbookOfVigilance.EquipmentAttributes.MagicLevelBonus = 6;
    SpellbookOfVigilance.EquipmentAttributes.Defense = 15;
    RegisterItem(SpellbookOfVigilance);

    FGameItem SpellscrollOfProphecies;
    SpellscrollOfProphecies.Name = TEXT("Spellscroll of Prophecies");
    SpellscrollOfProphecies.Description = TEXT("An ancient scroll containing prophetic magic.");
    SpellscrollOfProphecies.Rarity = EItemRarity::Rare;
    SpellscrollOfProphecies.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spellscroll_of_prophecies_icon.spellscroll_of_prophecies_icon")));
    SpellscrollOfProphecies.Weight = 15.0f;
    SpellscrollOfProphecies.ItemType = EItemType::Equipment;
    SpellscrollOfProphecies.bIsStackable = false;
    SpellscrollOfProphecies.StackCount = 1;
    SpellscrollOfProphecies.UseType = EItemUseType::Equipment;
    SpellscrollOfProphecies.UseAction = TEXT("Equip Scroll");
    SpellscrollOfProphecies.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SpellscrollOfProphecies.EquipmentAttributes.MagicLevelBonus = 8;
    SpellscrollOfProphecies.EquipmentAttributes.Defense = 12;
    RegisterItem(SpellscrollOfProphecies);

    FGameItem WoodenSpellbook;
    WoodenSpellbook.Name = TEXT("Wooden Spellbook");
    WoodenSpellbook.Description = TEXT("A basic spellbook bound in wooden covers.");
    WoodenSpellbook.Rarity = EItemRarity::Junk;
    WoodenSpellbook.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/wooden_spellbook_icon.wooden_spellbook_icon")));
    WoodenSpellbook.Weight = 23.0f;
    WoodenSpellbook.ItemType = EItemType::Equipment;
    WoodenSpellbook.bIsStackable = false;
    WoodenSpellbook.StackCount = 1;
    WoodenSpellbook.UseType = EItemUseType::Equipment;
    WoodenSpellbook.UseAction = TEXT("Equip Spellbook");
    WoodenSpellbook.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    WoodenSpellbook.EquipmentAttributes.MagicLevelBonus = 1;
    WoodenSpellbook.EquipmentAttributes.Defense = 10;
    RegisterItem(WoodenSpellbook);

    
    FGameItem BatwingHat;
    BatwingHat.Name = TEXT("Batwing Hat");
    BatwingHat.Description = TEXT("A mysterious hat made from bat wings.");
    BatwingHat.Rarity = EItemRarity::Epic;
    BatwingHat.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/batwing_hat_icon.batwing_hat_icon")));
    BatwingHat.Weight = 4.2f;
    BatwingHat.ItemType = EItemType::Equipment;
    BatwingHat.bIsStackable = false;
    BatwingHat.StackCount = 1;
    BatwingHat.UseType = EItemUseType::Equipment;
    BatwingHat.UseAction = TEXT("Equip Hat");
    BatwingHat.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    BatwingHat.EquipmentAttributes.MagicLevelBonus = 6;
    BatwingHat.EquipmentAttributes.Armor = 7;
    RegisterItem(BatwingHat);

    FGameItem FerumbrasHat;
    FerumbrasHat.Name = TEXT("Ferumbras' Hat");
    FerumbrasHat.Description = TEXT("The legendary hat of the dark wizard Ferumbras.");
    FerumbrasHat.Rarity = EItemRarity::Artifact;
    FerumbrasHat.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ferumbras_hat_icon.ferumbras_hat_icon")));
    FerumbrasHat.Weight = 8.5f;
    FerumbrasHat.ItemType = EItemType::Equipment;
    FerumbrasHat.bIsStackable = false;
    FerumbrasHat.StackCount = 1;
    FerumbrasHat.UseType = EItemUseType::Equipment;
    FerumbrasHat.UseAction = TEXT("Equip Hat");
    FerumbrasHat.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    FerumbrasHat.EquipmentAttributes.MagicLevelBonus = 12;
    FerumbrasHat.EquipmentAttributes.Armor = 4;
    RegisterItem(FerumbrasHat);

    FGameItem FurCap;
    FurCap.Name = TEXT("Fur Cap");
    FurCap.Description = TEXT("A warm cap made from animal fur.");
    FurCap.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/fur_cap_icon.fur_cap_icon")));
    FurCap.Weight = 2.8f;
    FurCap.ItemType = EItemType::Equipment;
    FurCap.bIsStackable = false;
    FurCap.StackCount = 1;
    FurCap.UseType = EItemUseType::Equipment;
    FurCap.UseAction = TEXT("Equip Cap");
    FurCap.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    FurCap.EquipmentAttributes.Armor = 1;
    FurCap.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(FurCap);

    FGameItem GaleaMortis;
    GaleaMortis.Name = TEXT("Galea Mortis");
    GaleaMortis.Description = TEXT("A helmet infused with death magic.");
    GaleaMortis.Rarity = EItemRarity::Legendary;
    GaleaMortis.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/galea_mortis_icon.galea_mortis_icon")));
    GaleaMortis.Weight = 29.5f;
    GaleaMortis.ItemType = EItemType::Equipment;
    GaleaMortis.bIsStackable = false;
    GaleaMortis.StackCount = 1;
    GaleaMortis.UseType = EItemUseType::Equipment;
    GaleaMortis.UseAction = TEXT("Equip Helmet");
    GaleaMortis.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    GaleaMortis.EquipmentAttributes.Armor = 8;
    GaleaMortis.EquipmentAttributes.MagicLevelBonus = 8;
    RegisterItem(GaleaMortis);

    FGameItem HatOfTheMad;
    HatOfTheMad.Name = TEXT("Hat of the Mad");
    HatOfTheMad.Description = TEXT("A hat that seems to whisper madness.");
    HatOfTheMad.Rarity = EItemRarity::Rare;
    HatOfTheMad.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/hat_of_the_mad_icon.hat_of_the_mad_icon")));
    HatOfTheMad.Weight = 4.5f;
    HatOfTheMad.ItemType = EItemType::Equipment;
    HatOfTheMad.bIsStackable = false;
    HatOfTheMad.StackCount = 1;
    HatOfTheMad.UseType = EItemUseType::Equipment;
    HatOfTheMad.UseAction = TEXT("Equip Hat");
    HatOfTheMad.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    HatOfTheMad.EquipmentAttributes.MagicLevelBonus = 6;
    HatOfTheMad.EquipmentAttributes.Armor = 6;
    RegisterItem(HatOfTheMad);

    FGameItem MysticTurban;
    MysticTurban.Name = TEXT("Mystic Turban");
    MysticTurban.Description = TEXT("A turban imbued with mystical energies.");
    MysticTurban.Rarity = EItemRarity::Uncommon;
    MysticTurban.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/mystic_turban_icon.mystic_turban_icon")));
    MysticTurban.Weight = 3.8f;
    MysticTurban.ItemType = EItemType::Equipment;
    MysticTurban.bIsStackable = false;
    MysticTurban.StackCount = 1;
    MysticTurban.UseType = EItemUseType::Equipment;
    MysticTurban.UseAction = TEXT("Equip Turban");
    MysticTurban.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    MysticTurban.EquipmentAttributes.MagicLevelBonus = 6;
    MysticTurban.EquipmentAttributes.Armor = 5;
    RegisterItem(MysticTurban);

    FGameItem PairOfEarmuffs;
    PairOfEarmuffs.Name = TEXT("Pair of Earmuffs");
    PairOfEarmuffs.Description = TEXT("Warm and cozy earmuffs.");
    PairOfEarmuffs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/pair_of_earmuffs_icon.pair_of_earmuffs_icon")));
    PairOfEarmuffs.Weight = 1.2f;
    PairOfEarmuffs.ItemType = EItemType::Equipment;
    PairOfEarmuffs.bIsStackable = false;
    PairOfEarmuffs.StackCount = 1;
    PairOfEarmuffs.UseType = EItemUseType::Equipment;
    PairOfEarmuffs.UseAction = TEXT("Equip Earmuffs");
    PairOfEarmuffs.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    PairOfEarmuffs.EquipmentAttributes.Armor = 1;
    PairOfEarmuffs.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(PairOfEarmuffs);

    FGameItem VisageOfTheEndDays;
    VisageOfTheEndDays.Name = TEXT("Visage of the End Days");
    VisageOfTheEndDays.Description = TEXT("A mysterious mask showing apocalyptic visions.");
    VisageOfTheEndDays.Rarity = EItemRarity::Epic;
    VisageOfTheEndDays.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/visage_of_the_end_days_icon.visage_of_the_end_days_icon")));
    VisageOfTheEndDays.Weight = 8.5f;
    VisageOfTheEndDays.ItemType = EItemType::Equipment;
    VisageOfTheEndDays.bIsStackable = false;
    VisageOfTheEndDays.StackCount = 1;
    VisageOfTheEndDays.UseType = EItemUseType::Equipment;
    VisageOfTheEndDays.UseAction = TEXT("Equip Mask");
    VisageOfTheEndDays.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    VisageOfTheEndDays.EquipmentAttributes.MagicLevelBonus = 7;
    VisageOfTheEndDays.EquipmentAttributes.Armor = 7;
    RegisterItem(VisageOfTheEndDays);

    FGameItem WitchHat;
    WitchHat.Name = TEXT("Witch Hat");
    WitchHat.Description = TEXT("A classic pointed hat worn by witches.");
    WitchHat.Rarity = EItemRarity::Uncommon;
    WitchHat.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/witch_hat_icon.witch_hat_icon")));
    WitchHat.Weight = 3.5f;
    WitchHat.ItemType = EItemType::Equipment;
    WitchHat.bIsStackable = false;
    WitchHat.StackCount = 1;
    WitchHat.UseType = EItemUseType::Equipment;
    WitchHat.UseAction = TEXT("Equip Hat");
    WitchHat.EquipmentAttributes.EquipmentType = EEquipmentType::Helm;
    WitchHat.EquipmentAttributes.MagicLevelBonus = 3;
    WitchHat.EquipmentAttributes.Armor = 3;
    RegisterItem(WitchHat);

    FGameItem GoldenHyaenaPendant;
    GoldenHyaenaPendant.Name = TEXT("Golden Hyaena Pendant");
    GoldenHyaenaPendant.Description = TEXT("A golden pendant in the shape of a hyaena.");
    GoldenHyaenaPendant.Rarity = EItemRarity::Epic;
    GoldenHyaenaPendant.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/golden_hyaena_pendant_icon.golden_hyaena_pendant_icon")));
    GoldenHyaenaPendant.Weight = 3.2f;
    GoldenHyaenaPendant.ItemType = EItemType::Equipment;
    GoldenHyaenaPendant.bIsStackable = false;
    GoldenHyaenaPendant.StackCount = 1;
    GoldenHyaenaPendant.UseType = EItemUseType::Equipment;
    GoldenHyaenaPendant.UseAction = TEXT("Equip Pendant");
    GoldenHyaenaPendant.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    GoldenHyaenaPendant.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(GoldenHyaenaPendant);

    FGameItem Scarf;
    Scarf.Name = TEXT("Scarf");
    Scarf.Description = TEXT("A warm and stylish scarf.");
    Scarf.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/scarf_icon.scarf_icon")));
    Scarf.Weight = 1.8f;
    Scarf.ItemType = EItemType::Equipment;
    Scarf.bIsStackable = false;
    Scarf.StackCount = 1;
    Scarf.UseType = EItemUseType::Equipment;
    Scarf.UseAction = TEXT("Equip Scarf");
    Scarf.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    Scarf.EquipmentAttributes.Defense = 1;
    RegisterItem(Scarf);

    FGameItem TraditionalNeckerchief;
    TraditionalNeckerchief.Name = TEXT("Traditional Neckerchief");
    TraditionalNeckerchief.Description = TEXT("Pass the beer, opa.");
    TraditionalNeckerchief.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/traditional_neckerchief_icon.traditional_neckerchief_icon")));
    TraditionalNeckerchief.Weight = 1.5f;
    TraditionalNeckerchief.ItemType = EItemType::Equipment;
    TraditionalNeckerchief.bIsStackable = false;
    TraditionalNeckerchief.StackCount = 1;
    TraditionalNeckerchief.UseType = EItemUseType::Equipment;
    TraditionalNeckerchief.UseAction = TEXT("Equip Neckerchief");
    TraditionalNeckerchief.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    RegisterItem(TraditionalNeckerchief);

    FGameItem WolfToothChain;
    WolfToothChain.Name = TEXT("Wolf Tooth Chain");
    WolfToothChain.Description = TEXT("A necklace made from wolf teeth.");
    WolfToothChain.Rarity = EItemRarity::Epic;
    WolfToothChain.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/wolf_tooth_chain_icon.wolf_tooth_chain_icon")));
    WolfToothChain.Weight = 3.3f;
    WolfToothChain.ItemType = EItemType::Equipment;
    WolfToothChain.bIsStackable = false;
    WolfToothChain.StackCount = 1;
    WolfToothChain.UseType = EItemUseType::Equipment;
    WolfToothChain.UseAction = TEXT("Equip Chain");
    WolfToothChain.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    WolfToothChain.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(WolfToothChain);

    FGameItem TerraMantle;
    TerraMantle.Name = TEXT("Terra Mantle");
    TerraMantle.Description = TEXT("A powerful mantle imbued with earth magic.");
    TerraMantle.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/terra_mantle_icon.terra_mantle_icon")));
    TerraMantle.Weight = 85.0f;
    TerraMantle.ItemType = EItemType::Equipment;
    TerraMantle.bIsStackable = false;
    TerraMantle.StackCount = 1;
    TerraMantle.UseType = EItemUseType::Equipment;
    TerraMantle.UseAction = TEXT("Equip Armor");
    TerraMantle.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    TerraMantle.EquipmentAttributes.Armor = 6;
    TerraMantle.EquipmentAttributes.MagicLevelBonus = 3;
    RegisterItem(TerraMantle);

    FGameItem Backpack;
    Backpack.Name = TEXT("Backpack");
    Backpack.Description = TEXT("A sturdy leather backpack for carrying items.");
    Backpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/backpack_icon.backpack_icon")));
    Backpack.Weight = 5.0f;
    Backpack.ItemType = EItemType::Equipment;
    Backpack.bIsStackable = false;
    Backpack.StackCount = 1;
    Backpack.UseType = EItemUseType::Equipment;
    Backpack.UseAction = TEXT("Equip Backpack");
    Backpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    Backpack.EquipmentAttributes.CapacityBonus = 20;
    RegisterItem(Backpack);

    
    FGameItem OrnamentedShield;
    OrnamentedShield.Name = TEXT("Ornamented Shield");
    OrnamentedShield.Description = TEXT("A beautifully decorated shield.");
    OrnamentedShield.Rarity = EItemRarity::Epic;
    OrnamentedShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ornamented_shield_icon.ornamented_shield_icon")));
    OrnamentedShield.Weight = 52.0f;
    OrnamentedShield.ItemType = EItemType::Equipment;
    OrnamentedShield.bIsStackable = false;
    OrnamentedShield.StackCount = 1;
    OrnamentedShield.UseType = EItemUseType::Equipment;
    OrnamentedShield.UseAction = TEXT("Equip Shield");
    OrnamentedShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    OrnamentedShield.EquipmentAttributes.Defense = 31;
    OrnamentedShield.EquipmentAttributes.MagicLevelBonus = 4;
    RegisterItem(OrnamentedShield);

    
    FGameItem OrnateShield;
    OrnateShield.Name = TEXT("Ornate Shield");
    OrnateShield.Description = TEXT("An intricately decorated shield.");
    OrnateShield.Rarity = EItemRarity::Rare;
    OrnateShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ornate_shield_icon.ornate_shield_icon")));
    OrnateShield.Weight = 54.0f;
    OrnateShield.ItemType = EItemType::Equipment;
    OrnateShield.bIsStackable = false;
    OrnateShield.StackCount = 1;
    OrnateShield.UseType = EItemUseType::Equipment;
    OrnateShield.UseAction = TEXT("Equip Shield");
    OrnateShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    OrnateShield.EquipmentAttributes.Defense = 52;
    RegisterItem(OrnateShield);

    
    FGameItem PhoenixShield;
    PhoenixShield.Name = TEXT("Phoenix Shield");
    PhoenixShield.Description = TEXT("A shield blessed with the power of the phoenix.");
    PhoenixShield.Rarity = EItemRarity::Uncommon;
    PhoenixShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/phoenix_shield_icon.phoenix_shield_icon")));
    PhoenixShield.Weight = 47.0f;
    PhoenixShield.ItemType = EItemType::Equipment;
    PhoenixShield.bIsStackable = false;
    PhoenixShield.StackCount = 1;
    PhoenixShield.UseType = EItemUseType::Equipment;
    PhoenixShield.UseAction = TEXT("Equip Shield");
    PhoenixShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    PhoenixShield.EquipmentAttributes.Defense = 42;
    RegisterItem(PhoenixShield);

    
    FGameItem PlatinumAmulet;
    PlatinumAmulet.Name = TEXT("Platinum Amulet");
    PlatinumAmulet.Description = TEXT("An amulet made of pure platinum.");
    PlatinumAmulet.Rarity = EItemRarity::Uncommon;
    PlatinumAmulet.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/platinum_amulet_icon.platinum_amulet_icon")));
    PlatinumAmulet.Weight = 5.2f;
    PlatinumAmulet.ItemType = EItemType::Equipment;
    PlatinumAmulet.bIsStackable = false;
    PlatinumAmulet.StackCount = 1;
    PlatinumAmulet.UseType = EItemUseType::Equipment;
    PlatinumAmulet.UseAction = TEXT("Equip Amulet");
    PlatinumAmulet.EquipmentAttributes.EquipmentType = EEquipmentType::Amulet;
    PlatinumAmulet.EquipmentAttributes.Defense = 2;
    RegisterItem(PlatinumAmulet);

    
    FGameItem RendingInferniarchRod;
    RendingInferniarchRod.Name = TEXT("Rending Inferniarch Rod");
    RendingInferniarchRod.Description = TEXT("A rod infused with destructive infernal magic.");
    RendingInferniarchRod.Rarity = EItemRarity::Legendary;
    RendingInferniarchRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/rending_inferniarch_rod_icon.rending_inferniarch_rod_icon")));
    RendingInferniarchRod.Weight = 28.0f;
    RendingInferniarchRod.ItemType = EItemType::Equipment;
    RendingInferniarchRod.bIsStackable = false;
    RendingInferniarchRod.StackCount = 1;
    RendingInferniarchRod.UseType = EItemUseType::Equipment;
    RendingInferniarchRod.UseAction = TEXT("Equip Rod");
    RendingInferniarchRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    RendingInferniarchRod.EquipmentAttributes.MagicLevelBonus = 10;
    RegisterItem(RendingInferniarchRod);

    
    FGameItem RoseShield;
    RoseShield.Name = TEXT("Rose Shield");
    RoseShield.Description = TEXT("A shield adorned with a beautiful rose pattern.");
    RoseShield.Rarity = EItemRarity::Uncommon;
    RoseShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/rose_shield_icon.rose_shield_icon")));
    RoseShield.Weight = 52.0f;
    RoseShield.ItemType = EItemType::Equipment;
    RoseShield.bIsStackable = false;
    RoseShield.StackCount = 1;
    RoseShield.UseType = EItemUseType::Equipment;
    RoseShield.UseAction = TEXT("Equip Shield");
    RoseShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    RoseShield.EquipmentAttributes.Defense = 38;
    RegisterItem(RoseShield);

    
    FGameItem ShimmerRod;
    ShimmerRod.Name = TEXT("Shimmer Rod");
    ShimmerRod.Description = TEXT("A rod that glows with magical energy.");
    ShimmerRod.Rarity = EItemRarity::Rare;
    ShimmerRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/shimmer_rod_icon.shimmer_rod_icon")));
    ShimmerRod.Weight = 26.0f;
    ShimmerRod.ItemType = EItemType::Equipment;
    ShimmerRod.bIsStackable = false;
    ShimmerRod.StackCount = 1;
    ShimmerRod.UseType = EItemUseType::Equipment;
    ShimmerRod.UseAction = TEXT("Equip Rod");
    ShimmerRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    ShimmerRod.EquipmentAttributes.MagicLevelBonus = 6;
    RegisterItem(ShimmerRod);

    
    FGameItem SiphoningInferniarchRod;
    SiphoningInferniarchRod.Name = TEXT("Siphoning Inferniarch Rod");
    SiphoningInferniarchRod.Description = TEXT("A rod that drains life force from enemies.");
    SiphoningInferniarchRod.Rarity = EItemRarity::Legendary;
    SiphoningInferniarchRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/siphoning_inferniarch_rod_icon.siphoning_inferniarch_rod_icon")));
    SiphoningInferniarchRod.Weight = 27.0f;
    SiphoningInferniarchRod.ItemType = EItemType::Equipment;
    SiphoningInferniarchRod.bIsStackable = false;
    SiphoningInferniarchRod.StackCount = 1;
    SiphoningInferniarchRod.UseType = EItemUseType::Equipment;
    SiphoningInferniarchRod.UseAction = TEXT("Equip Rod");
    SiphoningInferniarchRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    SiphoningInferniarchRod.EquipmentAttributes.MagicLevelBonus = 10;
    RegisterItem(SiphoningInferniarchRod);

    
    FGameItem SnakeGodsWristguard;
    SnakeGodsWristguard.Name = TEXT("Snake God's Wristguard");
    SnakeGodsWristguard.Description = TEXT("A powerful wristguard blessed by the Snake God.");
    SnakeGodsWristguard.Rarity = EItemRarity::Legendary;
    SnakeGodsWristguard.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/snake_gods_wristguard_icon.snake_gods_wristguard_icon")));
    SnakeGodsWristguard.Weight = 25.0f;
    SnakeGodsWristguard.ItemType = EItemType::Equipment;
    SnakeGodsWristguard.bIsStackable = false;
    SnakeGodsWristguard.StackCount = 1;
    SnakeGodsWristguard.UseType = EItemUseType::Equipment;
    SnakeGodsWristguard.UseAction = TEXT("Equip Wristguard");
    SnakeGodsWristguard.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SnakeGodsWristguard.EquipmentAttributes.Defense = 28;
    SnakeGodsWristguard.EquipmentAttributes.MagicLevelBonus = 13;
    RegisterItem(SnakeGodsWristguard);

    
    FGameItem SpiritGuide;
    SpiritGuide.Name = TEXT("Spirit Guide");
    SpiritGuide.Description = TEXT("A mystical tome that channels spirit energy.");
    SpiritGuide.Rarity = EItemRarity::Rare;
    SpiritGuide.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spirit_guide_icon.spirit_guide_icon")));
    SpiritGuide.Weight = 28.0f;
    SpiritGuide.ItemType = EItemType::Equipment;
    SpiritGuide.bIsStackable = false;
    SpiritGuide.StackCount = 1;
    SpiritGuide.UseType = EItemUseType::Equipment;
    SpiritGuide.UseAction = TEXT("Equip Guide");
    SpiritGuide.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    SpiritGuide.EquipmentAttributes.Defense = 16;
    SpiritGuide.EquipmentAttributes.MagicLevelBonus = 5;
    RegisterItem(SpiritGuide);

    
    FGameItem SpringsproutRod;
    SpringsproutRod.Name = TEXT("Springsprout Rod");
    SpringsproutRod.Description = TEXT("A rod imbued with nature's magic.");
    SpringsproutRod.Rarity = EItemRarity::Rare;
    SpringsproutRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/springsprout_rod_icon.springsprout_rod_icon")));
    SpringsproutRod.Weight = 27.0f;
    SpringsproutRod.ItemType = EItemType::Equipment;
    SpringsproutRod.bIsStackable = false;
    SpringsproutRod.StackCount = 1;
    SpringsproutRod.UseType = EItemUseType::Equipment;
    SpringsproutRod.UseAction = TEXT("Equip Rod");
    SpringsproutRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    SpringsproutRod.EquipmentAttributes.MagicLevelBonus = 4;
    RegisterItem(SpringsproutRod);

    
    FGameItem TerraRod;
    TerraRod.Name = TEXT("Terra Rod");
    TerraRod.Description = TEXT("A rod that channels earth magic.");
    TerraRod.Rarity = EItemRarity::Junk;
    TerraRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/terra_rod_icon.terra_rod_icon")));
    TerraRod.Weight = 27.0f;
    TerraRod.ItemType = EItemType::Equipment;
    TerraRod.bIsStackable = false;
    TerraRod.StackCount = 1;
    TerraRod.UseType = EItemUseType::Equipment;
    TerraRod.UseAction = TEXT("Equip Rod");
    TerraRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    TerraRod.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(TerraRod);

    
    FGameItem UnderworldRod;
    UnderworldRod.Name = TEXT("Underworld Rod");
    UnderworldRod.Description = TEXT("A rod that channels the power of the underworld.");
    UnderworldRod.Rarity = EItemRarity::Rare;
    UnderworldRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/underworld_rod_icon.underworld_rod_icon")));
    UnderworldRod.Weight = 27.0f;
    UnderworldRod.ItemType = EItemType::Equipment;
    UnderworldRod.bIsStackable = false;
    UnderworldRod.StackCount = 1;
    UnderworldRod.UseType = EItemUseType::Equipment;
    UnderworldRod.UseAction = TEXT("Equip Rod");
    UnderworldRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    UnderworldRod.EquipmentAttributes.MagicLevelBonus = 6;
    RegisterItem(UnderworldRod);

    
    FGameItem AdamantShield;
    AdamantShield.Name = TEXT("Adamant Shield");
    AdamantShield.Description = TEXT("A shield forged from adamantium.");
    AdamantShield.Rarity = EItemRarity::Epic;
    AdamantShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/adamant_shield_icon.adamant_shield_icon")));
    AdamantShield.Weight = 65.0f;
    AdamantShield.ItemType = EItemType::Equipment;
    AdamantShield.bIsStackable = false;
    AdamantShield.StackCount = 1;
    AdamantShield.UseType = EItemUseType::Equipment;
    AdamantShield.UseAction = TEXT("Equip Shield");
    AdamantShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    AdamantShield.EquipmentAttributes.Defense = 54;
    RegisterItem(AdamantShield);

    
    FGameItem AmazonShield;
    AmazonShield.Name = TEXT("Amazon Shield");
    AmazonShield.Description = TEXT("A shield favored by amazon warriors.");
    AmazonShield.Rarity = EItemRarity::Rare;
    AmazonShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/amazon_shield_icon.amazon_shield_icon")));
    AmazonShield.Weight = 62.0f;
    AmazonShield.ItemType = EItemType::Equipment;
    AmazonShield.bIsStackable = false;
    AmazonShield.StackCount = 1;
    AmazonShield.UseType = EItemUseType::Equipment;
    AmazonShield.UseAction = TEXT("Equip Shield");
    AmazonShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    AmazonShield.EquipmentAttributes.Defense = 44;
    RegisterItem(AmazonShield);

    
    FGameItem AncientShield;
    AncientShield.Name = TEXT("Ancient Shield");
    AncientShield.Description = TEXT("A shield from an ancient civilization.");
    AncientShield.Rarity = EItemRarity::Uncommon;
    AncientShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ancient_shield_icon.ancient_shield_icon")));
    AncientShield.Weight = 61.0f;
    AncientShield.ItemType = EItemType::Equipment;
    AncientShield.bIsStackable = false;
    AncientShield.StackCount = 1;
    AncientShield.UseType = EItemUseType::Equipment;
    AncientShield.UseAction = TEXT("Equip Shield");
    AncientShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    AncientShield.EquipmentAttributes.Defense = 40;
    RegisterItem(AncientShield);

    
    FGameItem BlackShield;
    BlackShield.Name = TEXT("Black Shield");
    BlackShield.Description = TEXT("A shield made of darkened steel.");
    BlackShield.Rarity = EItemRarity::Common;
    BlackShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/black_shield_icon.black_shield_icon")));
    BlackShield.Weight = 59.0f;
    BlackShield.ItemType = EItemType::Equipment;
    BlackShield.bIsStackable = false;
    BlackShield.StackCount = 1;
    BlackShield.UseType = EItemUseType::Equipment;
    BlackShield.UseAction = TEXT("Equip Shield");
    BlackShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    BlackShield.EquipmentAttributes.Defense = 34;
    RegisterItem(BlackShield);

    
    FGameItem BlessedShield;
    BlessedShield.Name = TEXT("Blessed Shield");
    BlessedShield.Description = TEXT("A shield blessed with holy magic.");
    BlessedShield.Rarity = EItemRarity::Artifact;
    BlessedShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/blessed_shield_icon.blessed_shield_icon")));
    BlessedShield.Weight = 55.0f;
    BlessedShield.ItemType = EItemType::Equipment;
    BlessedShield.bIsStackable = false;
    BlessedShield.StackCount = 1;
    BlessedShield.UseType = EItemUseType::Equipment;
    BlessedShield.UseAction = TEXT("Equip Shield");
    BlessedShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    BlessedShield.EquipmentAttributes.Defense = 78;
    BlessedShield.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(BlessedShield);

    
    FGameItem BookOfLies;
    BookOfLies.Name = TEXT("Book of Lies");
    BookOfLies.Description = TEXT("A mysterious tome filled with deceptive magic.");
    BookOfLies.Rarity = EItemRarity::Legendary;
    BookOfLies.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/book_of_lies_icon.book_of_lies_icon")));
    BookOfLies.Weight = 28.0f;
    BookOfLies.ItemType = EItemType::Equipment;
    BookOfLies.bIsStackable = false;
    BookOfLies.StackCount = 1;
    BookOfLies.UseType = EItemUseType::Equipment;
    BookOfLies.UseAction = TEXT("Equip Book");
    BookOfLies.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    BookOfLies.EquipmentAttributes.Defense = 15;
    BookOfLies.EquipmentAttributes.MagicLevelBonus = 10;
    RegisterItem(BookOfLies);

    
    FGameItem CobraRod;
    CobraRod.Name = TEXT("Cobra Rod");
    CobraRod.Description = TEXT("A rod infused with serpentine magic.");
    CobraRod.Rarity = EItemRarity::Rare;
    CobraRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/cobra_rod_icon.cobra_rod_icon")));
    CobraRod.Weight = 27.0f;
    CobraRod.ItemType = EItemType::Equipment;
    CobraRod.bIsStackable = false;
    CobraRod.StackCount = 1;
    CobraRod.UseType = EItemUseType::Equipment;
    CobraRod.UseAction = TEXT("Equip Rod");
    CobraRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    CobraRod.EquipmentAttributes.MagicLevelBonus = 3;
    RegisterItem(CobraRod);

    
    FGameItem CobraWand;
    CobraWand.Name = TEXT("Cobra Wand");
    CobraWand.Description = TEXT("A wand with the power of the cobra.");
    CobraWand.Rarity = EItemRarity::Epic;
    CobraWand.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/cobra_wand_icon.cobra_wand_icon")));
    CobraWand.Weight = 24.0f;
    CobraWand.ItemType = EItemType::Equipment;
    CobraWand.bIsStackable = false;
    CobraWand.StackCount = 1;
    CobraWand.UseType = EItemUseType::Equipment;
    CobraWand.UseAction = TEXT("Equip Wand");
    CobraWand.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    CobraWand.EquipmentAttributes.MagicLevelBonus = 8;
    RegisterItem(CobraWand);

    
    FGameItem CrownShield;
    CrownShield.Name = TEXT("Crown Shield");
    CrownShield.Description = TEXT("A shield adorned with a royal crown.");
    CrownShield.Rarity = EItemRarity::Epic;
    CrownShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crown_shield_icon.crown_shield_icon")));
    CrownShield.Weight = 62.0f;
    CrownShield.ItemType = EItemType::Equipment;
    CrownShield.bIsStackable = false;
    CrownShield.StackCount = 1;
    CrownShield.UseType = EItemUseType::Equipment;
    CrownShield.UseAction = TEXT("Equip Shield");
    CrownShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    CrownShield.EquipmentAttributes.Defense = 58;
    RegisterItem(CrownShield);

    
    FGameItem DarkShield;
    DarkShield.Name = TEXT("Dark Shield");
    DarkShield.Description = TEXT("A shield imbued with dark energy.");
    DarkShield.Rarity = EItemRarity::Common;
    DarkShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dark_shield_icon.dark_shield_icon")));
    DarkShield.Weight = 52.0f;
    DarkShield.ItemType = EItemType::Equipment;
    DarkShield.bIsStackable = false;
    DarkShield.StackCount = 1;
    DarkShield.UseType = EItemUseType::Equipment;
    DarkShield.UseAction = TEXT("Equip Shield");
    DarkShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    DarkShield.EquipmentAttributes.Defense = 36;
    RegisterItem(DarkShield);

    
    FGameItem DeathGaze;
    DeathGaze.Name = TEXT("Death Gaze");
    DeathGaze.Description = TEXT("A mystical item that channels death magic.");
    DeathGaze.Rarity = EItemRarity::Legendary;
    DeathGaze.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/death_gaze_icon.death_gaze_icon")));
    DeathGaze.Weight = 25.0f;
    DeathGaze.ItemType = EItemType::Equipment;
    DeathGaze.bIsStackable = false;
    DeathGaze.StackCount = 1;
    DeathGaze.UseType = EItemUseType::Equipment;
    DeathGaze.UseAction = TEXT("Equip Death Gaze");
    DeathGaze.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    DeathGaze.EquipmentAttributes.Defense = 38;
    DeathGaze.EquipmentAttributes.MeleeSkillBonus = 4;
    RegisterItem(DeathGaze);

    
    FGameItem DeeplingCeremonialDagger;
    DeeplingCeremonialDagger.Name = TEXT("Deepling Ceremonial Dagger");
    DeeplingCeremonialDagger.Description = TEXT("A sacred dagger used in deepling ceremonies.");
    DeeplingCeremonialDagger.Rarity = EItemRarity::Epic;
    DeeplingCeremonialDagger.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/deepling_ceremonial_dagger_icon.deepling_ceremonial_dagger_icon")));
    DeeplingCeremonialDagger.Weight = 15.0f;
    DeeplingCeremonialDagger.ItemType = EItemType::Equipment;
    DeeplingCeremonialDagger.bIsStackable = false;
    DeeplingCeremonialDagger.StackCount = 1;
    DeeplingCeremonialDagger.UseType = EItemUseType::Equipment;
    DeeplingCeremonialDagger.UseAction = TEXT("Equip Dagger");
    DeeplingCeremonialDagger.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DeeplingCeremonialDagger.EquipmentAttributes.MagicLevelBonus = 7;
    RegisterItem(DeeplingCeremonialDagger);

    
    FGameItem DragonShield;
    DragonShield.Name = TEXT("Dragon Shield");
    DragonShield.Description = TEXT("A shield forged from dragon scales.");
    DragonShield.Rarity = EItemRarity::Epic;
    DragonShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_shield_icon.dragon_shield_icon")));
    DragonShield.Weight = 65.0f;
    DragonShield.ItemType = EItemType::Equipment;
    DragonShield.bIsStackable = false;
    DragonShield.StackCount = 1;
    DragonShield.UseType = EItemUseType::Equipment;
    DragonShield.UseAction = TEXT("Equip Shield");
    DragonShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    DragonShield.EquipmentAttributes.Defense = 56;
    DragonShield.EquipmentAttributes.MeleeSkillBonus = 2;
    RegisterItem(DragonShield);

    
    FGameItem DrainingInferniarchRod;
    DrainingInferniarchRod.Name = TEXT("Draining Inferniarch Rod");
    DrainingInferniarchRod.Description = TEXT("A rod that drains energy from enemies.");
    DrainingInferniarchRod.Rarity = EItemRarity::Epic;
    DrainingInferniarchRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/draining_inferniarch_rod_icon.draining_inferniarch_rod_icon")));
    DrainingInferniarchRod.Weight = 28.0f;
    DrainingInferniarchRod.ItemType = EItemType::Equipment;
    DrainingInferniarchRod.bIsStackable = false;
    DrainingInferniarchRod.StackCount = 1;
    DrainingInferniarchRod.UseType = EItemUseType::Equipment;
    DrainingInferniarchRod.UseAction = TEXT("Equip Rod");
    DrainingInferniarchRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DrainingInferniarchRod.EquipmentAttributes.MagicLevelBonus = 8;
    RegisterItem(DrainingInferniarchRod);

    
    FGameItem DreamBlossomStaff;
    DreamBlossomStaff.Name = TEXT("Dream Blossom Staff");
    DreamBlossomStaff.Description = TEXT("A staff made from dream-infused wood.");
    DreamBlossomStaff.Rarity = EItemRarity::Rare;
    DreamBlossomStaff.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dream_blossom_staff_icon.dream_blossom_staff_icon")));
    DreamBlossomStaff.Weight = 39.0f;
    DreamBlossomStaff.ItemType = EItemType::Equipment;
    DreamBlossomStaff.bIsStackable = false;
    DreamBlossomStaff.StackCount = 1;
    DreamBlossomStaff.UseType = EItemUseType::Equipment;
    DreamBlossomStaff.UseAction = TEXT("Equip Staff");
    DreamBlossomStaff.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DreamBlossomStaff.EquipmentAttributes.MagicLevelBonus = 4;
    RegisterItem(DreamBlossomStaff);

    
    FGameItem DwarvenShield;
    DwarvenShield.Name = TEXT("Dwarven Shield");
    DwarvenShield.Description = TEXT("A shield crafted by master dwarven smiths.");
    DwarvenShield.Rarity = EItemRarity::Epic;
    DwarvenShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dwarven_shield_icon.dwarven_shield_icon")));
    DwarvenShield.Weight = 69.0f;
    DwarvenShield.ItemType = EItemType::Equipment;
    DwarvenShield.bIsStackable = false;
    DwarvenShield.StackCount = 1;
    DwarvenShield.UseType = EItemUseType::Equipment;
    DwarvenShield.UseAction = TEXT("Equip Shield");
    DwarvenShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    DwarvenShield.EquipmentAttributes.Defense = 33;
    RegisterItem(DwarvenShield);

    
    FGameItem EldritchFolio;
    EldritchFolio.Name = TEXT("Eldritch Folio");
    EldritchFolio.Description = TEXT("An ancient tome containing eldritch knowledge.");
    EldritchFolio.Rarity = EItemRarity::Legendary;
    EldritchFolio.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/eldritch_folio_icon.eldritch_folio_icon")));
    EldritchFolio.Weight = 28.0f;
    EldritchFolio.ItemType = EItemType::Equipment;
    EldritchFolio.bIsStackable = false;
    EldritchFolio.StackCount = 1;
    EldritchFolio.UseType = EItemUseType::Equipment;
    EldritchFolio.UseAction = TEXT("Equip Folio");
    EldritchFolio.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    EldritchFolio.EquipmentAttributes.Defense = 18;
    EldritchFolio.EquipmentAttributes.MagicLevelBonus = 11;
    RegisterItem(EldritchFolio);

    
    FGameItem EldritchTome;
    EldritchTome.Name = TEXT("Eldritch Tome");
    EldritchTome.Description = TEXT("A powerful tome of eldritch magic.");
    EldritchTome.Rarity = EItemRarity::Legendary;
    EldritchTome.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/eldritch_tome_icon.eldritch_tome_icon")));
    EldritchTome.Weight = 29.0f;
    EldritchTome.ItemType = EItemType::Equipment;
    EldritchTome.bIsStackable = false;
    EldritchTome.StackCount = 1;
    EldritchTome.UseType = EItemUseType::Equipment;
    EldritchTome.UseAction = TEXT("Equip Tome");
    EldritchTome.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    EldritchTome.EquipmentAttributes.Defense = 19;
    EldritchTome.EquipmentAttributes.MagicLevelBonus = 12;
    RegisterItem(EldritchTome);

    
    FGameItem EnergizedLimb;
    EnergizedLimb.Name = TEXT("Energized Limb");
    EnergizedLimb.Description = TEXT("A magical limb crackling with energy.");
    EnergizedLimb.Rarity = EItemRarity::Rare;
    EnergizedLimb.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/energized_limb_icon.energized_limb_icon")));
    EnergizedLimb.Weight = 25.0f;
    EnergizedLimb.ItemType = EItemType::Equipment;
    EnergizedLimb.bIsStackable = false;
    EnergizedLimb.StackCount = 1;
    EnergizedLimb.UseType = EItemUseType::Equipment;
    EnergizedLimb.UseAction = TEXT("Equip Limb");
    EnergizedLimb.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    EnergizedLimb.EquipmentAttributes.MagicLevelBonus = 3;
    RegisterItem(EnergizedLimb);

    
    FGameItem FalconEscutcheon;
    FalconEscutcheon.Name = TEXT("Falcon Escutcheon");
    FalconEscutcheon.Description = TEXT("A shield bearing the emblem of a falcon.");
    FalconEscutcheon.Rarity = EItemRarity::Legendary;
    FalconEscutcheon.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_escutcheon_icon.falcon_escutcheon_icon")));
    FalconEscutcheon.Weight = 55.0f;
    FalconEscutcheon.ItemType = EItemType::Equipment;
    FalconEscutcheon.bIsStackable = false;
    FalconEscutcheon.StackCount = 1;
    FalconEscutcheon.UseType = EItemUseType::Equipment;
    FalconEscutcheon.UseAction = TEXT("Equip Shield");
    FalconEscutcheon.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    FalconEscutcheon.EquipmentAttributes.Defense = 64;
    FalconEscutcheon.EquipmentAttributes.MeleeSkillBonus = 2;
    RegisterItem(FalconEscutcheon);

    
    FGameItem FalconRod;
    FalconRod.Name = TEXT("Falcon Rod");
    FalconRod.Description = TEXT("A rod imbued with the spirit of a falcon.");
    FalconRod.Rarity = EItemRarity::Legendary;
    FalconRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_rod_icon.falcon_rod_icon")));
    FalconRod.Weight = 27.0f;
    FalconRod.ItemType = EItemType::Equipment;
    FalconRod.bIsStackable = false;
    FalconRod.StackCount = 1;
    FalconRod.UseType = EItemUseType::Equipment;
    FalconRod.UseAction = TEXT("Equip Rod");
    FalconRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    FalconRod.EquipmentAttributes.MagicLevelBonus = 11;
    RegisterItem(FalconRod);

    
    FGameItem FalconShield;
    FalconShield.Name = TEXT("Falcon Shield");
    FalconShield.Description = TEXT("A shield blessed with falcon magic.");
    FalconShield.Rarity = EItemRarity::Legendary;
    FalconShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_shield_icon.falcon_shield_icon")));
    FalconShield.Weight = 57.0f;
    FalconShield.ItemType = EItemType::Equipment;
    FalconShield.bIsStackable = false;
    FalconShield.StackCount = 1;
    FalconShield.UseType = EItemUseType::Equipment;
    FalconShield.UseAction = TEXT("Equip Shield");
    FalconShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    FalconShield.EquipmentAttributes.Defense = 62;
    RegisterItem(FalconShield);

    
    FGameItem FalconWand;
    FalconWand.Name = TEXT("Falcon Wand");
    FalconWand.Description = TEXT("A wand that channels falcon magic.");
    FalconWand.Rarity = EItemRarity::Legendary;
    FalconWand.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/falcon_wand_icon.falcon_wand_icon")));
    FalconWand.Weight = 25.0f;
    FalconWand.ItemType = EItemType::Equipment;
    FalconWand.bIsStackable = false;
    FalconWand.StackCount = 1;
    FalconWand.UseType = EItemUseType::Equipment;
    FalconWand.UseAction = TEXT("Equip Wand");
    FalconWand.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    FalconWand.EquipmentAttributes.MagicLevelBonus = 11;
    RegisterItem(FalconWand);

    
    FGameItem FerumbrasStaff;
    FerumbrasStaff.Name = TEXT("Ferumbras' Staff");
    FerumbrasStaff.Description = TEXT("The legendary staff of the dark wizard Ferumbras.");
    FerumbrasStaff.Rarity = EItemRarity::Artifact;
    FerumbrasStaff.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ferumbras_staff_icon.ferumbras_staff_icon")));
    FerumbrasStaff.Weight = 45.0f;
    FerumbrasStaff.ItemType = EItemType::Equipment;
    FerumbrasStaff.bIsStackable = false;
    FerumbrasStaff.StackCount = 1;
    FerumbrasStaff.UseType = EItemUseType::Equipment;
    FerumbrasStaff.UseAction = TEXT("Equip Staff");
    FerumbrasStaff.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    FerumbrasStaff.EquipmentAttributes.MagicLevelBonus = 15;
    RegisterItem(FerumbrasStaff);

    
    FGameItem GreatShield;
    GreatShield.Name = TEXT("Great Shield");
    GreatShield.Description = TEXT("A massive shield of exceptional quality.");
    GreatShield.Rarity = EItemRarity::Artifact;
    GreatShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/great_shield_icon.great_shield_icon")));
    GreatShield.Weight = 84.0f;
    GreatShield.ItemType = EItemType::Equipment;
    GreatShield.bIsStackable = false;
    GreatShield.StackCount = 1;
    GreatShield.UseType = EItemUseType::Equipment;
    GreatShield.UseAction = TEXT("Equip Shield");
    GreatShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    GreatShield.EquipmentAttributes.Defense = 76;
    GreatShield.EquipmentAttributes.MeleeSkillBonus = 5;
    RegisterItem(GreatShield);

    
    FGameItem GuardianShield;
    GuardianShield.Name = TEXT("Guardian Shield");
    GuardianShield.Description = TEXT("A shield enchanted with protective magic.");
    GuardianShield.Rarity = EItemRarity::Junk;
    GuardianShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/guardian_shield_icon.guardian_shield_icon")));
    GuardianShield.Weight = 60.0f;
    GuardianShield.ItemType = EItemType::Equipment;
    GuardianShield.bIsStackable = false;
    GuardianShield.StackCount = 1;
    GuardianShield.UseType = EItemUseType::Equipment;
    GuardianShield.UseAction = TEXT("Equip Shield");
    GuardianShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    GuardianShield.EquipmentAttributes.Defense = 32;
    RegisterItem(GuardianShield);

    
    FGameItem Knife;
    Knife.Name = TEXT("Knife");
    Knife.Description = TEXT("A small but sharp knife.");
    Knife.Rarity = EItemRarity::Common;
    Knife.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/knife_icon.knife_icon")));
    Knife.Weight = 8.2f;
    Knife.ItemType = EItemType::Equipment;
    Knife.bIsStackable = false;
    Knife.StackCount = 1;
    Knife.UseType = EItemUseType::Equipment;
    Knife.UseAction = TEXT("Equip Knife");
    Knife.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Knife.EquipmentAttributes.Attack = 20;
    Knife.EquipmentAttributes.Defense = 12;
    RegisterItem(Knife);

    
    FGameItem LionShield;
    LionShield.Name = TEXT("Lion Shield");
    LionShield.Description = TEXT("A shield bearing the emblem of a lion.");
    LionShield.Rarity = EItemRarity::Epic;
    LionShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/lion_shield_icon.lion_shield_icon")));
    LionShield.Weight = 58.0f;
    LionShield.ItemType = EItemType::Equipment;
    LionShield.bIsStackable = false;
    LionShield.StackCount = 1;
    LionShield.UseType = EItemUseType::Equipment;
    LionShield.UseAction = TEXT("Equip Shield");
    LionShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    LionShield.EquipmentAttributes.Defense = 60;
    RegisterItem(LionShield);

    
    FGameItem LionWand;
    LionWand.Name = TEXT("Lion Wand");
    LionWand.Description = TEXT("A wand that channels the power of a lion.");
    LionWand.Rarity = EItemRarity::Legendary;
    LionWand.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/lion_wand_icon.lion_wand_icon")));
    LionWand.Weight = 24.0f;
    LionWand.ItemType = EItemType::Equipment;
    LionWand.bIsStackable = false;
    LionWand.StackCount = 1;
    LionWand.UseType = EItemUseType::Equipment;
    LionWand.UseAction = TEXT("Equip Wand");
    LionWand.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    LionWand.EquipmentAttributes.MagicLevelBonus = 9;
    RegisterItem(LionWand);

    
    FGameItem MastermindShield;
    MastermindShield.Name = TEXT("Mastermind Shield");
    MastermindShield.Description = TEXT("A shield that enhances mental prowess.");
    MastermindShield.Rarity = EItemRarity::Legendary;
    MastermindShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/mastermind_shield_icon.mastermind_shield_icon")));
    MastermindShield.Weight = 54.0f;
    MastermindShield.ItemType = EItemType::Equipment;
    MastermindShield.bIsStackable = false;
    MastermindShield.StackCount = 1;
    MastermindShield.UseType = EItemUseType::Equipment;
    MastermindShield.UseAction = TEXT("Equip Shield");
    MastermindShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    MastermindShield.EquipmentAttributes.Defense = 74;
    MastermindShield.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(MastermindShield);

    
    FGameItem MedusaShield;
    MedusaShield.Name = TEXT("Medusa Shield");
    MedusaShield.Description = TEXT("A shield with the power to petrify enemies.");
    MedusaShield.Rarity = EItemRarity::Rare;
    MedusaShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/medusa_shield_icon.medusa_shield_icon")));
    MedusaShield.Weight = 58.0f;
    MedusaShield.ItemType = EItemType::Equipment;
    MedusaShield.bIsStackable = false;
    MedusaShield.StackCount = 1;
    MedusaShield.UseType = EItemUseType::Equipment;
    MedusaShield.UseAction = TEXT("Equip Shield");
    MedusaShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    MedusaShield.EquipmentAttributes.Defense = 46;
    RegisterItem(MedusaShield);

    
    FGameItem MuckRod;
    MuckRod.Name = TEXT("Muck Rod");
    MuckRod.Description = TEXT("A rod that channels earth and decay magic.");
    MuckRod.Rarity = EItemRarity::Uncommon;
    MuckRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/muck_rod_icon.muck_rod_icon")));
    MuckRod.Weight = 27.0f;
    MuckRod.ItemType = EItemType::Equipment;
    MuckRod.bIsStackable = false;
    MuckRod.StackCount = 1;
    MuckRod.UseType = EItemUseType::Equipment;
    MuckRod.UseAction = TEXT("Equip Rod");
    MuckRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    MuckRod.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(MuckRod);

    
    FGameItem NagaWand;
    NagaWand.Name = TEXT("Naga Wand");
    NagaWand.Description = TEXT("A wand infused with serpentine magic.");
    NagaWand.Rarity = EItemRarity::Legendary;
    NagaWand.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/naga_wand_icon.naga_wand_icon")));
    NagaWand.Weight = 24.0f;
    NagaWand.ItemType = EItemType::Equipment;
    NagaWand.bIsStackable = false;
    NagaWand.StackCount = 1;
    NagaWand.UseType = EItemUseType::Equipment;
    NagaWand.UseAction = TEXT("Equip Wand");
    NagaWand.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    NagaWand.EquipmentAttributes.MagicLevelBonus = 9;
    RegisterItem(NagaWand);

    
    FGameItem NecromancerShield;
    NecromancerShield.Name = TEXT("Necromancer Shield");
    NecromancerShield.Description = TEXT("A shield imbued with dark necromantic magic.");
    NecromancerShield.Rarity = EItemRarity::Epic;
    NecromancerShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/necromancer_shield_icon.necromancer_shield_icon")));
    NecromancerShield.Weight = 55.0f;
    NecromancerShield.ItemType = EItemType::Equipment;
    NecromancerShield.bIsStackable = false;
    NecromancerShield.StackCount = 1;
    NecromancerShield.UseType = EItemUseType::Equipment;
    NecromancerShield.UseAction = TEXT("Equip Shield");
    NecromancerShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    NecromancerShield.EquipmentAttributes.Defense = 32;
    NecromancerShield.EquipmentAttributes.MagicLevelBonus = 5;
    RegisterItem(NecromancerShield);

    
    FGameItem NightmareShield;
    NightmareShield.Name = TEXT("Nightmare Shield");
    NightmareShield.Description = TEXT("A shield that instills fear in enemies.");
    NightmareShield.Rarity = EItemRarity::Legendary;
    NightmareShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/nightmare_shield_icon.nightmare_shield_icon")));
    NightmareShield.Weight = 56.0f;
    NightmareShield.ItemType = EItemType::Equipment;
    NightmareShield.bIsStackable = false;
    NightmareShield.StackCount = 1;
    NightmareShield.UseType = EItemUseType::Equipment;
    NightmareShield.UseAction = TEXT("Equip Shield");
    NightmareShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    NightmareShield.EquipmentAttributes.Defense = 66;
    NecromancerShield.EquipmentAttributes.MeleeSkillBonus = 3;
    RegisterItem(NightmareShield);

    
    FGameItem NorseShield;
    NorseShield.Name = TEXT("Norse Shield");
    NorseShield.Description = TEXT("A shield crafted in the style of the Norse warriors.");
    NorseShield.Rarity = EItemRarity::Rare;
    NorseShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/norse_shield_icon.norse_shield_icon")));
    NorseShield.Weight = 62.0f;
    NorseShield.ItemType = EItemType::Equipment;
    NorseShield.bIsStackable = false;
    NorseShield.StackCount = 1;
    NorseShield.UseType = EItemUseType::Equipment;
    NorseShield.UseAction = TEXT("Equip Shield");
    NorseShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    NorseShield.EquipmentAttributes.Defense = 48;
    NorseShield.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(NorseShield);

    
    FGameItem NorthwindRod;
    NorthwindRod.Name = TEXT("Northwind Rod");
    NorthwindRod.Description = TEXT("A rod that channels the power of the northern winds.");
    NorthwindRod.Rarity = EItemRarity::Rare;
    NorthwindRod.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/northwind_rod_icon.northwind_rod_icon")));
    NorthwindRod.Weight = 27.0f;
    NorthwindRod.ItemType = EItemType::Equipment;
    NorthwindRod.bIsStackable = false;
    NorthwindRod.StackCount = 1;
    NorthwindRod.UseType = EItemUseType::Equipment;
    NorthwindRod.UseAction = TEXT("Equip Rod");
    NorthwindRod.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    NorthwindRod.EquipmentAttributes.MagicLevelBonus = 5;
    RegisterItem(NorthwindRod);

    
    FGameItem TheScorcher;
    TheScorcher.Name = TEXT("The Scorcher");
    TheScorcher.Description = TEXT("A rod that burns with intense fire magic.");
    TheScorcher.Rarity = EItemRarity::Junk;
    TheScorcher.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/the_scorcher_icon.the_scorcher_icon")));
    TheScorcher.Weight = 28.0f;
    TheScorcher.ItemType = EItemType::Equipment;
    TheScorcher.bIsStackable = false;
    TheScorcher.StackCount = 1;
    TheScorcher.UseType = EItemUseType::Equipment;
    TheScorcher.UseAction = TEXT("Equip Rod");
    TheScorcher.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    TheScorcher.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(TheScorcher);

    
    FGameItem TowerShield;
    TowerShield.Name = TEXT("Tower Shield");
    TowerShield.Description = TEXT("A massive shield that provides excellent protection.");
    TowerShield.Rarity = EItemRarity::Rare;
    TowerShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/tower_shield_icon.tower_shield_icon")));
    TowerShield.Weight = 68.0f;
    TowerShield.ItemType = EItemType::Equipment;
    TowerShield.bIsStackable = false;
    TowerShield.StackCount = 1;
    TowerShield.UseType = EItemUseType::Equipment;
    TowerShield.UseAction = TEXT("Equip Shield");
    TowerShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    TowerShield.EquipmentAttributes.Defense = 50;
    RegisterItem(TowerShield);

    
    FGameItem VampireShield;
    VampireShield.Name = TEXT("Vampire Shield");
    VampireShield.Description = TEXT("A shield that drains life force from enemies.");
    VampireShield.Rarity = EItemRarity::Legendary;
    VampireShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/vampire_shield_icon.vampire_shield_icon")));
    VampireShield.Weight = 58.0f;
    VampireShield.ItemType = EItemType::Equipment;
    VampireShield.bIsStackable = false;
    VampireShield.StackCount = 1;
    VampireShield.UseType = EItemUseType::Equipment;
    VampireShield.UseAction = TEXT("Equip Shield");
    VampireShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    VampireShield.EquipmentAttributes.Defense = 59;
    RegisterItem(VampireShield);

    
    FGameItem WandOfDraconia;
    WandOfDraconia.Name = TEXT("Wand of Draconia");
    WandOfDraconia.Description = TEXT("A wand infused with dragon magic.");
    WandOfDraconia.Rarity = EItemRarity::Rare;
    WandOfDraconia.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/wand_of_draconia_icon.wand_of_draconia_icon")));
    WandOfDraconia.Weight = 27.0f;
    WandOfDraconia.ItemType = EItemType::Equipment;
    WandOfDraconia.bIsStackable = false;
    WandOfDraconia.StackCount = 1;
    WandOfDraconia.UseType = EItemUseType::Equipment;
    WandOfDraconia.UseAction = TEXT("Equip Wand");
    WandOfDraconia.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    WandOfDraconia.EquipmentAttributes.MagicLevelBonus = 5;
    RegisterItem(WandOfDraconia);

    
    FGameItem WandOfVoodoo;
    WandOfVoodoo.Name = TEXT("Wand of Voodoo");
    WandOfVoodoo.Description = TEXT("A wand that channels dark voodoo magic.");
    WandOfVoodoo.Rarity = EItemRarity::Uncommon;
    WandOfVoodoo.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/wand_of_voodoo_icon.wand_of_voodoo_icon")));
    WandOfVoodoo.Weight = 24.0f;
    WandOfVoodoo.ItemType = EItemType::Equipment;
    WandOfVoodoo.bIsStackable = false;
    WandOfVoodoo.StackCount = 1;
    WandOfVoodoo.UseType = EItemUseType::Equipment;
    WandOfVoodoo.UseAction = TEXT("Equip Wand");
    WandOfVoodoo.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    WandOfVoodoo.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(WandOfVoodoo);

    FGameItem ParalyseRune;
    ParalyseRune.Name = TEXT("Paralyse Rune");
    ParalyseRune.Description = TEXT("A magical rune that temporarily paralyses the target.");
    ParalyseRune.Rarity = EItemRarity::Junk;
    ParalyseRune.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/paralyse_rune_icon.paralyse_rune_icon")));
    ParalyseRune.Weight = 1.8f;
    ParalyseRune.ItemType = EItemType::Rune;
    ParalyseRune.bIsStackable = true;
    ParalyseRune.StackCount = 1;
    ParalyseRune.UseType = EItemUseType::RuneSpell;
    ParalyseRune.UseAction = TEXT("Paralyse Rune");
    RegisterItem(ParalyseRune);

    RegisterRunes();

    
    FGameItem MushroomBackpack;
    MushroomBackpack.Name = TEXT("Mushroom Backpack");
    MushroomBackpack.Description = TEXT("A backpack decorated with mushroom patterns.");
    MushroomBackpack.Rarity = EItemRarity::Common;
    MushroomBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/mushroom_backpack_icon.mushroom_backpack_icon")));
    MushroomBackpack.Weight = 8.0f;
    MushroomBackpack.ItemType = EItemType::Equipment;
    MushroomBackpack.bIsStackable = false;
    MushroomBackpack.StackCount = 1;
    MushroomBackpack.UseType = EItemUseType::Equipment;
    MushroomBackpack.UseAction = TEXT("Equip Backpack");
    MushroomBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    MushroomBackpack.EquipmentAttributes.CapacityBonus = 20;
    RegisterItem(MushroomBackpack);

    FGameItem BackpackOfHolding;
    BackpackOfHolding.Name = TEXT("Backpack of Holding");
    BackpackOfHolding.Description = TEXT("A magical backpack with enhanced carrying capacity.");
    BackpackOfHolding.Rarity = EItemRarity::Legendary;
    BackpackOfHolding.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/backpack_of_holding_icon.backpack_of_holding_icon")));
    BackpackOfHolding.Weight = 12.0f;
    BackpackOfHolding.ItemType = EItemType::Equipment;
    BackpackOfHolding.bIsStackable = false;
    BackpackOfHolding.StackCount = 1;
    BackpackOfHolding.UseType = EItemUseType::Equipment;
    BackpackOfHolding.UseAction = TEXT("Equip Backpack");
    BackpackOfHolding.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    BackpackOfHolding.EquipmentAttributes.CapacityBonus = 400;
    RegisterItem(BackpackOfHolding);

    
    FGameItem DaramianWaraxe;
    DaramianWaraxe.Name = TEXT("Daramian Waraxe");
    DaramianWaraxe.Description = TEXT("A heavy waraxe forged by Daramian smiths.");
    DaramianWaraxe.Rarity = EItemRarity::Common;
    DaramianWaraxe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/daramian_waraxe_icon.daramian_waraxe_icon")));
    DaramianWaraxe.Weight = 68.0f;
    DaramianWaraxe.ItemType = EItemType::Equipment;
    DaramianWaraxe.bIsStackable = false;
    DaramianWaraxe.StackCount = 1;
    DaramianWaraxe.UseType = EItemUseType::Equipment;
    DaramianWaraxe.UseAction = TEXT("Equip Weapon");
    DaramianWaraxe.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DaramianWaraxe.EquipmentAttributes.Attack = 43;
    DaramianWaraxe.EquipmentAttributes.Defense = 15;
    RegisterItem(DaramianWaraxe);

    FGameItem HeroicAxe;
    HeroicAxe.Name = TEXT("Heroic Axe");
    HeroicAxe.Description = TEXT("An axe wielded by legendary heroes.");
    HeroicAxe.Rarity = EItemRarity::Rare;
    HeroicAxe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/heroic_axe_icon.heroic_axe_icon")));
    HeroicAxe.Weight = 69.0f;
    HeroicAxe.ItemType = EItemType::Equipment;
    HeroicAxe.bIsStackable = false;
    HeroicAxe.StackCount = 1;
    HeroicAxe.UseType = EItemUseType::Equipment;
    HeroicAxe.UseAction = TEXT("Equip Weapon");
    HeroicAxe.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    HeroicAxe.EquipmentAttributes.Attack = 42;
    HeroicAxe.EquipmentAttributes.Defense = 16;
    RegisterItem(HeroicAxe);

    
    FGameItem RoyalScaleRobe;
    RoyalScaleRobe.Name = TEXT("Royal Scale Robe");
    RoyalScaleRobe.Description = TEXT("A robe made from enchanted dragon scales.");
    RoyalScaleRobe.Rarity = EItemRarity::Epic;
    RoyalScaleRobe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/royal_scale_robe_icon.royal_scale_robe_icon")));
    RoyalScaleRobe.Weight = 49.0f;
    RoyalScaleRobe.ItemType = EItemType::Equipment;
    RoyalScaleRobe.bIsStackable = false;
    RoyalScaleRobe.StackCount = 1;
    RoyalScaleRobe.UseType = EItemUseType::Equipment;
    RoyalScaleRobe.UseAction = TEXT("Equip Armor");
    RoyalScaleRobe.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    RoyalScaleRobe.EquipmentAttributes.Armor = 8;
    RoyalScaleRobe.EquipmentAttributes.MagicLevelBonus = 8;
    RegisterItem(RoyalScaleRobe);

    
    FGameItem SantaBackpack;
    SantaBackpack.Name = TEXT("Santa Backpack");
    SantaBackpack.Description = TEXT("A festive backpack in the style of Santa's sack.");
    SantaBackpack.Rarity = EItemRarity::Legendary;
    SantaBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/santa_backpack_icon.santa_backpack_icon")));
    SantaBackpack.Weight = 9.0f;
    SantaBackpack.ItemType = EItemType::Equipment;
    SantaBackpack.bIsStackable = false;
    SantaBackpack.StackCount = 1;
    SantaBackpack.UseType = EItemUseType::Equipment;
    SantaBackpack.UseAction = TEXT("Equip Backpack");
    SantaBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    SantaBackpack.EquipmentAttributes.CapacityBonus = 450;
    RegisterItem(SantaBackpack);

    
    FGameItem BattleHammer;
    BattleHammer.Name = TEXT("Battle Hammer");
    BattleHammer.Description = TEXT("A heavy hammer designed for combat.");
    BattleHammer.Rarity = EItemRarity::Common;
    BattleHammer.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/battle_hammer_icon.battle_hammer_icon")));
    BattleHammer.Weight = 68.0f;
    BattleHammer.ItemType = EItemType::Equipment;
    BattleHammer.bIsStackable = false;
    BattleHammer.StackCount = 1;
    BattleHammer.UseType = EItemUseType::Equipment;
    BattleHammer.UseAction = TEXT("Equip Weapon");
    BattleHammer.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    BattleHammer.EquipmentAttributes.Attack = 37;
    BattleHammer.EquipmentAttributes.Defense = 10;
    RegisterItem(BattleHammer);

    
    FGameItem DeeplingAxe;
    DeeplingAxe.Name = TEXT("Deepling Axe");
    DeeplingAxe.Description = TEXT("An axe crafted by the mysterious deeplings.");
    DeeplingAxe.Rarity = EItemRarity::Rare;
    DeeplingAxe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/deepling_axe_icon.deepling_axe_icon")));
    DeeplingAxe.Weight = 65.0f;
    DeeplingAxe.ItemType = EItemType::Equipment;
    DeeplingAxe.bIsStackable = false;
    DeeplingAxe.StackCount = 1;
    DeeplingAxe.UseType = EItemUseType::Equipment;
    DeeplingAxe.UseAction = TEXT("Equip Weapon");
    DeeplingAxe.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DeeplingAxe.EquipmentAttributes.Attack = 39;
    DeeplingAxe.EquipmentAttributes.Defense = 24;
    RegisterItem(DeeplingAxe);

    
    FGameItem BeachBackpack;
    BeachBackpack.Name = TEXT("Beach Backpack");
    BeachBackpack.Description = TEXT("A colorful backpack with beach-themed patterns.");
    BeachBackpack.Rarity = EItemRarity::Rare;
    BeachBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/beach_backpack_icon.beach_backpack_icon")));
    BeachBackpack.Weight = 7.0f;
    BeachBackpack.ItemType = EItemType::Equipment;
    BeachBackpack.bIsStackable = false;
    BeachBackpack.StackCount = 1;
    BeachBackpack.UseType = EItemUseType::Equipment;
    BeachBackpack.UseAction = TEXT("Equip Backpack");
    BeachBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    BeachBackpack.EquipmentAttributes.CapacityBonus = 100;
    RegisterItem(BeachBackpack);

    
    FGameItem BeltedCape;
    BeltedCape.Name = TEXT("Belted Cape");
    BeltedCape.Description = TEXT("A sturdy cape with reinforced belts.");
    BeltedCape.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/belted_cape_icon.belted_cape_icon")));
    BeltedCape.Weight = 19.0f;
    BeltedCape.ItemType = EItemType::Equipment;
    BeltedCape.bIsStackable = false;
    BeltedCape.StackCount = 1;
    BeltedCape.UseType = EItemUseType::Equipment;
    BeltedCape.UseAction = TEXT("Equip Armor");
    BeltedCape.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    BeltedCape.EquipmentAttributes.Armor = 11;
    BeltedCape.EquipmentAttributes.DistanceSkillBonus = 1;
    RegisterItem(BeltedCape);

    
    FGameItem BlueBackpack;
    BlueBackpack.Name = TEXT("Blue Backpack");
    BlueBackpack.Description = TEXT("A simple blue backpack.");
    BlueBackpack.Rarity = EItemRarity::Uncommon;
    BlueBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/blue_backpack_icon.blue_backpack_icon")));
    BlueBackpack.Weight = 7.0f;
    BlueBackpack.ItemType = EItemType::Equipment;
    BlueBackpack.bIsStackable = false;
    BlueBackpack.StackCount = 1;
    BlueBackpack.UseType = EItemUseType::Equipment;
    BlueBackpack.UseAction = TEXT("Equip Backpack");
    BlueBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    BlueBackpack.EquipmentAttributes.CapacityBonus = 30;
    RegisterItem(BlueBackpack);

    
    FGameItem DjinnBlade;
    DjinnBlade.Name = TEXT("Djinn Blade");
    DjinnBlade.Description = TEXT("A mystical blade infused with djinn magic.");
    DjinnBlade.Rarity = EItemRarity::Rare;
    DjinnBlade.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/djinn_blade_icon.djinn_blade_icon")));
    DjinnBlade.Weight = 36.0f;
    DjinnBlade.ItemType = EItemType::Equipment;
    DjinnBlade.bIsStackable = false;
    DjinnBlade.StackCount = 1;
    DjinnBlade.UseType = EItemUseType::Equipment;
    DjinnBlade.UseAction = TEXT("Equip Weapon");
    DjinnBlade.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DjinnBlade.EquipmentAttributes.Attack = 38;
    DjinnBlade.EquipmentAttributes.Defense = 29;
    RegisterItem(DjinnBlade);

    
    FGameItem BookBackpack;
    BookBackpack.Name = TEXT("Book Backpack");
    BookBackpack.Description = TEXT("A backpack designed to carry books and scrolls.");
    BookBackpack.Rarity = EItemRarity::Legendary;
    BookBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/book_backpack_icon.book_backpack_icon")));
    BookBackpack.Weight = 8.0f;
    BookBackpack.ItemType = EItemType::Equipment;
    BookBackpack.bIsStackable = false;
    BookBackpack.StackCount = 1;
    BookBackpack.UseType = EItemUseType::Equipment;
    BookBackpack.UseAction = TEXT("Equip Backpack");
    BookBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    BookBackpack.EquipmentAttributes.CapacityBonus = 500;
    RegisterItem(BookBackpack);

    

    
    FGameItem DragonBackpack;
    DragonBackpack.Name = TEXT("Dragon Backpack");
    DragonBackpack.Description = TEXT("A backpack made from dragon scales.");
    DragonBackpack.Rarity = EItemRarity::Legendary;
    DragonBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_backpack_icon.dragon_backpack_icon")));
    DragonBackpack.Weight = 12.0f;
    DragonBackpack.ItemType = EItemType::Equipment;
    DragonBackpack.bIsStackable = false;
    DragonBackpack.StackCount = 1;
    DragonBackpack.UseType = EItemUseType::Equipment;
    DragonBackpack.UseAction = TEXT("Equip Backpack");
    DragonBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    DragonBackpack.EquipmentAttributes.CapacityBonus = 550;
    RegisterItem(DragonBackpack);

    
    FGameItem GnomeArmor;
    GnomeArmor.Name = TEXT("Gnome Armor");
    GnomeArmor.Description = TEXT("Armor crafted by master gnome smiths.");
    GnomeArmor.Rarity = EItemRarity::Rare;
    GnomeArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/gnome_armor_icon.gnome_armor_icon")));
    GnomeArmor.Weight = 82.0f;
    GnomeArmor.ItemType = EItemType::Equipment;
    GnomeArmor.bIsStackable = false;
    GnomeArmor.StackCount = 1;
    GnomeArmor.UseType = EItemUseType::Equipment;
    GnomeArmor.UseAction = TEXT("Equip Armor");
    GnomeArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    GnomeArmor.EquipmentAttributes.Armor = 22;
    GnomeArmor.EquipmentAttributes.MeleeSkillBonus = 4;
    GnomeArmor.EquipmentAttributes.DistanceSkillBonus = 3;
    RegisterItem(GnomeArmor);

    
    FGameItem GhostBackpack;
    GhostBackpack.Name = TEXT("Ghost Backpack");
    GhostBackpack.Description = TEXT("A spectral backpack with an ethereal glow.");
    GhostBackpack.Rarity = EItemRarity::Legendary;
    GhostBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ghost_backpack_icon.ghost_backpack_icon")));
    GhostBackpack.Weight = 6.0f;
    GhostBackpack.ItemType = EItemType::Equipment;
    GhostBackpack.bIsStackable = false;
    GhostBackpack.StackCount = 1;
    GhostBackpack.UseType = EItemUseType::Equipment;
    GhostBackpack.UseAction = TEXT("Equip Backpack");
    GhostBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    GhostBackpack.EquipmentAttributes.CapacityBonus = 600;
    RegisterItem(GhostBackpack);

    
    FGameItem MagicianBreeches;
    MagicianBreeches.Name = TEXT("Magician Breeches");
    MagicianBreeches.Description = TEXT("Enchanted breeches worn by skilled magicians.");
    MagicianBreeches.Rarity = EItemRarity::Rare;
    MagicianBreeches.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magician_breeches_icon.magician_breeches_icon")));
    MagicianBreeches.Weight = 18.0f;
    MagicianBreeches.ItemType = EItemType::Equipment;
    MagicianBreeches.bIsStackable = false;
    MagicianBreeches.StackCount = 1;
    MagicianBreeches.UseType = EItemUseType::Equipment;
    MagicianBreeches.UseAction = TEXT("Equip Legs");
    MagicianBreeches.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    MagicianBreeches.EquipmentAttributes.Armor = 5;
    MagicianBreeches.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(MagicianBreeches);

    
    FGameItem PirateBackpack;
    PirateBackpack.Name = TEXT("Pirate Backpack");
    PirateBackpack.Description = TEXT("A weathered backpack with nautical designs.");
    PirateBackpack.Rarity = EItemRarity::Epic;
    PirateBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/pirate_backpack_icon.pirate_backpack_icon")));
    PirateBackpack.Weight = 8.0f;
    PirateBackpack.ItemType = EItemType::Equipment;
    PirateBackpack.bIsStackable = false;
    PirateBackpack.StackCount = 1;
    PirateBackpack.UseType = EItemUseType::Equipment;
    PirateBackpack.UseAction = TEXT("Equip Backpack");
    PirateBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    PirateBackpack.EquipmentAttributes.CapacityBonus = 200;
    RegisterItem(PirateBackpack);

    
    FGameItem SpiritCloak;
    SpiritCloak.Name = TEXT("Spirit Cloak");
    SpiritCloak.Description = TEXT("A mystical cloak imbued with spiritual energy.");
    SpiritCloak.Rarity = EItemRarity::Common;
    SpiritCloak.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/spirit_cloak_icon.spirit_cloak_icon")));
    SpiritCloak.Weight = 15.0f;
    SpiritCloak.ItemType = EItemType::Equipment;
    SpiritCloak.bIsStackable = false;
    SpiritCloak.StackCount = 1;
    SpiritCloak.UseType = EItemUseType::Equipment;
    SpiritCloak.UseAction = TEXT("Equip Armor");
    SpiritCloak.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    SpiritCloak.EquipmentAttributes.Armor = 4;
    SpiritCloak.EquipmentAttributes.MagicLevelBonus = 2;
    RegisterItem(SpiritCloak);

    
    FGameItem WingedBackpack;
    WingedBackpack.Name = TEXT("Winged Backpack");
    WingedBackpack.Description = TEXT("A backpack adorned with decorative wings.");
    WingedBackpack.Rarity = EItemRarity::Legendary;
    WingedBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/winged_backpack_icon.winged_backpack_icon")));
    WingedBackpack.Weight = 7.5f;
    WingedBackpack.ItemType = EItemType::Equipment;
    WingedBackpack.bIsStackable = false;
    WingedBackpack.StackCount = 1;
    WingedBackpack.UseType = EItemUseType::Equipment;
    WingedBackpack.UseAction = TEXT("Equip Backpack");
    WingedBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    WingedBackpack.EquipmentAttributes.CapacityBonus = 650;
    RegisterItem(WingedBackpack);

    
    FGameItem DepthLorica;
    DepthLorica.Name = TEXT("Depth Lorica");
    DepthLorica.Description = TEXT("Armor forged in the depths of the ocean.");
    DepthLorica.Rarity = EItemRarity::Rare;
    DepthLorica.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/depth_lorica_icon.depth_lorica_icon")));
    DepthLorica.Weight = 88.0f;
    DepthLorica.ItemType = EItemType::Equipment;
    DepthLorica.bIsStackable = false;
    DepthLorica.StackCount = 1;
    DepthLorica.UseType = EItemUseType::Equipment;
    DepthLorica.UseAction = TEXT("Equip Armor");
    DepthLorica.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    DepthLorica.EquipmentAttributes.Armor = 13;
    DepthLorica.EquipmentAttributes.MeleeSkillBonus = 2;
    DepthLorica.EquipmentAttributes.DistanceSkillBonus = 3;
    RegisterItem(DepthLorica);

    
    FGameItem FurBackpack;
    FurBackpack.Name = TEXT("Fur Backpack");
    FurBackpack.Description = TEXT("A warm backpack made from animal fur.");
    FurBackpack.Rarity = EItemRarity::Rare;
    FurBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/fur_backpack_icon.fur_backpack_icon")));
    FurBackpack.Weight = 9.0f;
    FurBackpack.ItemType = EItemType::Equipment;
    FurBackpack.bIsStackable = false;
    FurBackpack.StackCount = 1;
    FurBackpack.UseType = EItemUseType::Equipment;
    FurBackpack.UseAction = TEXT("Equip Backpack");
    FurBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    FurBackpack.EquipmentAttributes.CapacityBonus = 110;
    RegisterItem(FurBackpack);

    
    FGameItem MagiciansRobe;
    MagiciansRobe.Name = TEXT("Magicians Robe");
    MagiciansRobe.Description = TEXT("A robe worn by powerful magicians.");
    MagiciansRobe.Rarity = EItemRarity::Junk;
    MagiciansRobe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/magicians_robe_icon.magicians_robe_icon")));
    MagiciansRobe.Weight = 22.0f;
    MagiciansRobe.ItemType = EItemType::Equipment;
    MagiciansRobe.bIsStackable = false;
    MagiciansRobe.StackCount = 1;
    MagiciansRobe.UseType = EItemUseType::Equipment;
    MagiciansRobe.UseAction = TEXT("Equip Armor");
    MagiciansRobe.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    MagiciansRobe.EquipmentAttributes.Armor = 3;
    MagiciansRobe.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(MagiciansRobe);

    
    FGameItem PowerBolt;
    PowerBolt.Name = TEXT("Power Bolt");
    PowerBolt.Description = TEXT("A powerful crossbow bolt.");
    PowerBolt.Rarity = EItemRarity::Uncommon;
    PowerBolt.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/power_bolt_icon.power_bolt_icon")));
    PowerBolt.Weight = 0.8f;
    PowerBolt.ItemType = EItemType::Equipment;
    PowerBolt.bIsStackable = true;
    PowerBolt.StackCount = 1;
    PowerBolt.UseType = EItemUseType::Equipment;
    PowerBolt.UseAction = TEXT("Equip Ammunition");
    PowerBolt.EquipmentAttributes.EquipmentType = EEquipmentType::Ammunition;
    PowerBolt.EquipmentAttributes.Attack = 7;
    RegisterItem(PowerBolt);

    
    FGameItem YalahariArmor;
    YalahariArmor.Name = TEXT("Yalahari Armor");
    YalahariArmor.Description = TEXT("Ancient armor from the Yalahari civilization.");
    YalahariArmor.Rarity = EItemRarity::Rare;
    YalahariArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/yalahari_armor_icon.yalahari_armor_icon")));
    YalahariArmor.Weight = 92.0f;
    YalahariArmor.ItemType = EItemType::Equipment;
    YalahariArmor.bIsStackable = false;
    YalahariArmor.StackCount = 1;
    YalahariArmor.UseType = EItemUseType::Equipment;
    YalahariArmor.UseAction = TEXT("Equip Armor");
    YalahariArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    YalahariArmor.EquipmentAttributes.Armor = 18;
    YalahariArmor.EquipmentAttributes.MeleeSkillBonus = 4;
    YalahariArmor.EquipmentAttributes.DistanceSkillBonus = 3;
    RegisterItem(YalahariArmor);
    
    FGameItem YalahariLegPiece;
    YalahariLegPiece.Name = TEXT("Yalahari Leg Piece");
    YalahariLegPiece.Description = TEXT("Leg armor from the ancient Yalahari civilization.");
    YalahariLegPiece.Rarity = EItemRarity::Epic;
    YalahariLegPiece.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/yalahari_leg_piece_icon.yalahari_leg_piece_icon")));
    YalahariLegPiece.Weight = 45.0f;
    YalahariLegPiece.ItemType = EItemType::Equipment;
    YalahariLegPiece.bIsStackable = false;
    YalahariLegPiece.StackCount = 1;
    YalahariLegPiece.UseType = EItemUseType::Equipment;
    YalahariLegPiece.UseAction = TEXT("Equip Legs");
    YalahariLegPiece.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    YalahariLegPiece.EquipmentAttributes.Armor = 14;
    YalahariLegPiece.EquipmentAttributes.DistanceSkillBonus = 2;
    RegisterItem(YalahariLegPiece);

    
    FGameItem BrocadeBag;
    BrocadeBag.Name = TEXT("Brocade Bag");
    BrocadeBag.Description = TEXT("An elegant bag made of brocade fabric.");
    BrocadeBag.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/brocade_bag_icon.brocade_bag_icon")));
    BrocadeBag.Weight = 7.0f;
    BrocadeBag.ItemType = EItemType::Equipment;
    BrocadeBag.bIsStackable = false;
    BrocadeBag.StackCount = 1;
    BrocadeBag.UseType = EItemUseType::Equipment;
    BrocadeBag.UseAction = TEXT("Equip Backpack");
    BrocadeBag.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    BrocadeBag.EquipmentAttributes.CapacityBonus = 20;
    RegisterItem(BrocadeBag);

    
    FGameItem DragonHammer;
    DragonHammer.Name = TEXT("Dragon Hammer");
    DragonHammer.Description = TEXT("A massive hammer infused with dragon magic.");
    DragonHammer.Rarity = EItemRarity::Epic;
    DragonHammer.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_hammer_icon.dragon_hammer_icon")));
    DragonHammer.Weight = 96.0f;
    DragonHammer.ItemType = EItemType::Equipment;
    DragonHammer.bIsStackable = false;
    DragonHammer.StackCount = 1;
    DragonHammer.UseType = EItemUseType::Equipment;
    DragonHammer.UseAction = TEXT("Equip Weapon");
    DragonHammer.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DragonHammer.EquipmentAttributes.Attack = 44;
    DragonHammer.EquipmentAttributes.Defense = 13;
    RegisterItem(DragonHammer);

    
    FGameItem GoldenBackpack;
    GoldenBackpack.Name = TEXT("Golden Backpack");
    GoldenBackpack.Description = TEXT("A luxurious backpack made with golden thread.");
    GoldenBackpack.Rarity = EItemRarity::Epic;
    GoldenBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/golden_backpack_icon.golden_backpack_icon")));
    GoldenBackpack.Weight = 11.0f;
    GoldenBackpack.ItemType = EItemType::Equipment;
    GoldenBackpack.bIsStackable = false;
    GoldenBackpack.StackCount = 1;
    GoldenBackpack.UseType = EItemUseType::Equipment;
    GoldenBackpack.UseAction = TEXT("Equip Backpack");
    GoldenBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    GoldenBackpack.EquipmentAttributes.CapacityBonus = 340;
    RegisterItem(GoldenBackpack);

    
    FGameItem MercenarySword;
    MercenarySword.Name = TEXT("Mercenary Sword");
    MercenarySword.Description = TEXT("A reliable sword favored by mercenaries.");
    MercenarySword.Rarity = EItemRarity::Epic;
    MercenarySword.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/mercenary_sword_icon.mercenary_sword_icon")));
    MercenarySword.Weight = 59.0f;
    MercenarySword.ItemType = EItemType::Equipment;
    MercenarySword.bIsStackable = false;
    MercenarySword.StackCount = 1;
    MercenarySword.UseType = EItemUseType::Equipment;
    MercenarySword.UseAction = TEXT("Equip Weapon");
    MercenarySword.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    MercenarySword.EquipmentAttributes.Attack = 40;
    MercenarySword.EquipmentAttributes.Defense = 28;
    RegisterItem(MercenarySword);

    
    FGameItem RainbowShield;
    RainbowShield.Name = TEXT("Rainbow Shield");
    RainbowShield.Description = TEXT("A shield shimmering with prismatic colors.");
    RainbowShield.Rarity = EItemRarity::Legendary;
    RainbowShield.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/rainbow_shield_icon.rainbow_shield_icon")));
    RainbowShield.Weight = 62.0f;
    RainbowShield.ItemType = EItemType::Equipment;
    RainbowShield.bIsStackable = false;
    RainbowShield.StackCount = 1;
    RainbowShield.UseType = EItemUseType::Equipment;
    RainbowShield.UseAction = TEXT("Equip Shield");
    RainbowShield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    RainbowShield.EquipmentAttributes.Defense = 68;
    RainbowShield.EquipmentAttributes.MeleeSkillBonus = 2;
    RegisterItem(RainbowShield);

    
    FGameItem RedBackpack;
    RedBackpack.Name = TEXT("Red Backpack");
    RedBackpack.Description = TEXT("A simple red backpack.");
    RedBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/red_backpack_icon.red_backpack_icon")));
    RedBackpack.Weight = 7.0f;
    RedBackpack.ItemType = EItemType::Equipment;
    RedBackpack.bIsStackable = false;
    RedBackpack.StackCount = 1;
    RedBackpack.UseType = EItemUseType::Equipment;
    RedBackpack.UseAction = TEXT("Equip Backpack");
    RedBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    RedBackpack.EquipmentAttributes.CapacityBonus = 20;
    RegisterItem(RedBackpack);

    
    FGameItem Drakinata;
    Drakinata.Name = TEXT("Drakinata");
    Drakinata.Description = TEXT("A legendary weapon of the draken warriors.");
    Drakinata.Rarity = EItemRarity::Uncommon;
    Drakinata.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/drakinata_icon.drakinata_icon")));
    Drakinata.Weight = 73.0f;
    Drakinata.ItemType = EItemType::Equipment;
    Drakinata.bIsStackable = false;
    Drakinata.StackCount = 1;
    Drakinata.UseType = EItemUseType::Equipment;
    Drakinata.UseAction = TEXT("Equip Weapon");
    Drakinata.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Drakinata.EquipmentAttributes.Attack = 47;
    Drakinata.EquipmentAttributes.Defense = 28;
    RegisterItem(Drakinata);

    
    FGameItem GreenBackpack;
    GreenBackpack.Name = TEXT("Green Backpack");
    GreenBackpack.Description = TEXT("A simple green backpack.");
    GreenBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/green_backpack_icon.green_backpack_icon")));
    GreenBackpack.Weight = 7.0f;
    GreenBackpack.ItemType = EItemType::Equipment;
    GreenBackpack.bIsStackable = false;
    GreenBackpack.StackCount = 1;
    GreenBackpack.UseType = EItemUseType::Equipment;
    GreenBackpack.UseAction = TEXT("Equip Backpack");
    GreenBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    GreenBackpack.EquipmentAttributes.CapacityBonus = 20;
    RegisterItem(GreenBackpack);

    
    FGameItem GuardianHalberd;
    GuardianHalberd.Name = TEXT("Guardian Halberd");
    GuardianHalberd.Description = TEXT("A halberd used by elite guardians.");
    GuardianHalberd.Rarity = EItemRarity::Common;
    GuardianHalberd.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/guardian_halberd_icon.guardian_halberd_icon")));
    GuardianHalberd.Weight = 88.0f;
    GuardianHalberd.ItemType = EItemType::Equipment;
    GuardianHalberd.bIsStackable = false;
    GuardianHalberd.StackCount = 1;
    GuardianHalberd.UseType = EItemUseType::Equipment;
    GuardianHalberd.UseAction = TEXT("Equip Weapon");
    GuardianHalberd.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    GuardianHalberd.EquipmentAttributes.Attack = 44;
    GuardianHalberd.EquipmentAttributes.Defense = 25;
    RegisterItem(GuardianHalberd);

    
    FGameItem GreyBackpack;
    GreyBackpack.Name = TEXT("Grey Backpack");
    GreyBackpack.Description = TEXT("A simple grey backpack.");
    GreyBackpack.Rarity = EItemRarity::Uncommon;
    GreyBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/grey_backpack_icon.grey_backpack_icon")));
    GreyBackpack.Weight = 7.0f;
    GreyBackpack.ItemType = EItemType::Equipment;
    GreyBackpack.bIsStackable = false;
    GreyBackpack.StackCount = 1;
    GreyBackpack.UseType = EItemUseType::Equipment;
    GreyBackpack.UseAction = TEXT("Equip Backpack");
    GreyBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    GreyBackpack.EquipmentAttributes.CapacityBonus = 45;
    RegisterItem(GreyBackpack);

    
    FGameItem HeartBackpack;
    HeartBackpack.Name = TEXT("Heart Backpack");
    HeartBackpack.Description = TEXT("A backpack decorated with heart patterns.");
    HeartBackpack.Rarity = EItemRarity::Rare;
    HeartBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/heart_backpack_icon.heart_backpack_icon")));
    HeartBackpack.Weight = 7.0f;
    HeartBackpack.ItemType = EItemType::Equipment;
    HeartBackpack.bIsStackable = false;
    HeartBackpack.StackCount = 1;
    HeartBackpack.UseType = EItemUseType::Equipment;
    HeartBackpack.UseAction = TEXT("Equip Backpack");
    HeartBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    HeartBackpack.EquipmentAttributes.CapacityBonus = 170;
    RegisterItem(HeartBackpack);

    
    FGameItem MoonBackpack;
    MoonBackpack.Name = TEXT("Moon Backpack");
    MoonBackpack.Description = TEXT("A backpack decorated with lunar patterns.");
    MoonBackpack.Rarity = EItemRarity::Epic;
    MoonBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/moon_backpack_icon.moon_backpack_icon")));
    MoonBackpack.Weight = 7.0f;
    MoonBackpack.ItemType = EItemType::Equipment;
    MoonBackpack.bIsStackable = false;
    MoonBackpack.StackCount = 1;
    MoonBackpack.UseType = EItemUseType::Equipment;
    MoonBackpack.UseAction = TEXT("Equip Backpack");
    MoonBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    MoonBackpack.EquipmentAttributes.CapacityBonus = 360;
    RegisterItem(MoonBackpack);

    
    FGameItem RoyalAxe;
    RoyalAxe.Name = TEXT("Royal Axe");
    RoyalAxe.Description = TEXT("An axe worthy of royalty.");
    RoyalAxe.Rarity = EItemRarity::Legendary;
    RoyalAxe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/royal_axe_icon.royal_axe_icon")));
    RoyalAxe.Weight = 78.0f;
    RoyalAxe.ItemType = EItemType::Equipment;
    RoyalAxe.bIsStackable = false;
    RoyalAxe.StackCount = 1;
    RoyalAxe.UseType = EItemUseType::Equipment;
    RoyalAxe.UseAction = TEXT("Equip Weapon");
    RoyalAxe.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    RoyalAxe.EquipmentAttributes.Attack = 47;
    RoyalAxe.EquipmentAttributes.Defense = 26;
    RegisterItem(RoyalAxe);

    
    FGameItem DwarvenArmor;
    DwarvenArmor.Name = TEXT("Dwarven Armor");
    DwarvenArmor.Description = TEXT("Sturdy armor crafted by dwarven smiths.");
    DwarvenArmor.Rarity = EItemRarity::Rare;
    DwarvenArmor.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dwarven_armor_icon.dwarven_armor_icon")));
    DwarvenArmor.Weight = 82.0f;
    DwarvenArmor.ItemType = EItemType::Equipment;
    DwarvenArmor.bIsStackable = false;
    DwarvenArmor.StackCount = 1;
    DwarvenArmor.UseType = EItemUseType::Equipment;
    DwarvenArmor.UseAction = TEXT("Equip Armor");
    DwarvenArmor.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    DwarvenArmor.EquipmentAttributes.Armor = 20;
    DwarvenArmor.EquipmentAttributes.MeleeSkillBonus = 4;
    DwarvenArmor.EquipmentAttributes.DistanceSkillBonus = 3;
    RegisterItem(DwarvenArmor);

    
    FGameItem DwarvenAxe;
    DwarvenAxe.Name = TEXT("Dwarven Axe");
    DwarvenAxe.Description = TEXT("A mighty axe forged by dwarven craftsmen.");
    DwarvenAxe.Rarity = EItemRarity::Uncommon;
    DwarvenAxe.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dwarven_axe_icon.dwarven_axe_icon")));
    DwarvenAxe.Weight = 69.0f;
    DwarvenAxe.ItemType = EItemType::Equipment;
    DwarvenAxe.bIsStackable = false;
    DwarvenAxe.StackCount = 1;
    DwarvenAxe.UseType = EItemUseType::Equipment;
    DwarvenAxe.UseAction = TEXT("Equip Weapon");
    DwarvenAxe.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DwarvenAxe.EquipmentAttributes.Attack = 37;
    DwarvenAxe.EquipmentAttributes.Defense = 25;
    RegisterItem(DwarvenAxe);

    
    FGameItem EthnoCoat;
    EthnoCoat.Name = TEXT("Ethno Coat");
    EthnoCoat.Description = TEXT("A coat with traditional ethnic patterns.");
    EthnoCoat.Rarity = EItemRarity::Rare;
    EthnoCoat.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/ethno_coat_icon.ethno_coat_icon")));
    EthnoCoat.Weight = 24.0f;
    EthnoCoat.ItemType = EItemType::Equipment;
    EthnoCoat.bIsStackable = false;
    EthnoCoat.StackCount = 1;
    EthnoCoat.UseType = EItemUseType::Equipment;
    EthnoCoat.UseAction = TEXT("Equip Armor");
    EthnoCoat.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    EthnoCoat.EquipmentAttributes.Armor = 7;
    EthnoCoat.EquipmentAttributes.MagicLevelBonus = 5;
    RegisterItem(EthnoCoat);

    
    FGameItem LeatherLegs;
    LeatherLegs.Name = TEXT("Leather Pants");
    LeatherLegs.Description = TEXT("Simple leg protection made of leather.");
    LeatherLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/leather_legs_icon.leather_legs_icon")));
    LeatherLegs.Weight = 18.0f;
    LeatherLegs.ItemType = EItemType::Equipment;
    LeatherLegs.bIsStackable = false;
    LeatherLegs.StackCount = 1;
    LeatherLegs.UseType = EItemUseType::Equipment;
    LeatherLegs.UseAction = TEXT("Equip Legs");
    LeatherLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    LeatherLegs.EquipmentAttributes.Armor = 5;
    RegisterItem(LeatherLegs);

    
    FGameItem MammothFurCape;
    MammothFurCape.Name = TEXT("Mammoth Fur Cape");
    MammothFurCape.Description = TEXT("A warm cape made from mammoth fur.");
    MammothFurCape.Rarity = EItemRarity::Junk;
    MammothFurCape.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/mammoth_fur_cape_icon.mammoth_fur_cape_icon")));
    MammothFurCape.Weight = 32.0f;
    MammothFurCape.ItemType = EItemType::Equipment;
    MammothFurCape.bIsStackable = false;
    MammothFurCape.StackCount = 1;
    MammothFurCape.UseType = EItemUseType::Equipment;
    MammothFurCape.UseAction = TEXT("Equip Armor");
    MammothFurCape.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    MammothFurCape.EquipmentAttributes.Armor = 3;
    MammothFurCape.EquipmentAttributes.MagicLevelBonus = 1;
    RegisterItem(MammothFurCape);

    
    FGameItem MammothFurShorts;
    MammothFurShorts.Name = TEXT("Mammoth Fur Shorts");
    MammothFurShorts.Description = TEXT("Warm shorts made from mammoth fur.");
    MammothFurShorts.Rarity = EItemRarity::Junk;
    MammothFurShorts.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/mammoth_fur_shorts_icon.mammoth_fur_shorts_icon")));
    MammothFurShorts.Weight = 25.0f;
    MammothFurShorts.ItemType = EItemType::Equipment;
    MammothFurShorts.bIsStackable = false;
    MammothFurShorts.StackCount = 1;
    MammothFurShorts.UseType = EItemUseType::Equipment;
    MammothFurShorts.UseAction = TEXT("Equip Legs");
    MammothFurShorts.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    MammothFurShorts.EquipmentAttributes.Armor = 4;
    RegisterItem(MammothFurShorts);

    
    FGameItem MoohtahPlate;
    MoohtahPlate.Name = TEXT("Moohtah Plate");
    MoohtahPlate.Description = TEXT("Armor forged by the mysterious Moohtah warriors.");
    MoohtahPlate.Rarity = EItemRarity::Common;
    MoohtahPlate.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/moohtah_plate_icon.moohtah_plate_icon")));
    MoohtahPlate.Weight = 87.0f;
    MoohtahPlate.ItemType = EItemType::Equipment;
    MoohtahPlate.bIsStackable = false;
    MoohtahPlate.StackCount = 1;
    MoohtahPlate.UseType = EItemUseType::Equipment;
    MoohtahPlate.UseAction = TEXT("Equip Armor");
    MoohtahPlate.EquipmentAttributes.EquipmentType = EEquipmentType::Armor;
    MoohtahPlate.EquipmentAttributes.Armor = 12;
    MoohtahPlate.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(MoohtahPlate);

    
    FGameItem DwarvenLegs;
    DwarvenLegs.Name = TEXT("Dwarven Legs");
    DwarvenLegs.Description = TEXT("Leg protection crafted by dwarven smiths.");
    DwarvenLegs.Rarity = EItemRarity::Uncommon;
    DwarvenLegs.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dwarven_legs_icon.dwarven_legs_icon")));
    DwarvenLegs.Weight = 49.0f;
    DwarvenLegs.ItemType = EItemType::Equipment;
    DwarvenLegs.bIsStackable = false;
    DwarvenLegs.StackCount = 1;
    DwarvenLegs.UseType = EItemUseType::Equipment;
    DwarvenLegs.UseAction = TEXT("Equip Legs");
    DwarvenLegs.EquipmentAttributes.EquipmentType = EEquipmentType::Legs;
    DwarvenLegs.EquipmentAttributes.Armor = 11;
    DwarvenLegs.EquipmentAttributes.MeleeSkillBonus = 1;
    RegisterItem(DwarvenLegs);

    
    FGameItem DeeplingStaff;
    DeeplingStaff.Name = TEXT("Deepling Staff");
    DeeplingStaff.Description = TEXT("A staff used by deepling spellcasters.");
    DeeplingStaff.Rarity = EItemRarity::Epic;
    DeeplingStaff.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/deepling_staff_icon.deepling_staff_icon")));
    DeeplingStaff.Weight = 38.0f;
    DeeplingStaff.ItemType = EItemType::Equipment;
    DeeplingStaff.bIsStackable = false;
    DeeplingStaff.StackCount = 1;
    DeeplingStaff.UseType = EItemUseType::Equipment;
    DeeplingStaff.UseAction = TEXT("Equip Weapon");
    DeeplingStaff.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DeeplingStaff.EquipmentAttributes.Attack = 28;
    DeeplingStaff.EquipmentAttributes.Defense = 25;
    DeeplingStaff.EquipmentAttributes.MagicLevelBonus = 7;
    RegisterItem(DeeplingStaff);

    
    FGameItem ExpeditionBackpack;
    ExpeditionBackpack.Name = TEXT("Expedition Backpack");
    ExpeditionBackpack.Description = TEXT("A durable backpack designed for long expeditions.");
    ExpeditionBackpack.Rarity = EItemRarity::Epic;
    ExpeditionBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/expedition_backpack_icon.expedition_backpack_icon")));
    ExpeditionBackpack.Weight = 11.0f;
    ExpeditionBackpack.ItemType = EItemType::Equipment;
    ExpeditionBackpack.bIsStackable = false;
    ExpeditionBackpack.StackCount = 1;
    ExpeditionBackpack.UseType = EItemUseType::Equipment;
    ExpeditionBackpack.UseAction = TEXT("Equip Backpack");
    ExpeditionBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    ExpeditionBackpack.EquipmentAttributes.CapacityBonus = 220;
    RegisterItem(ExpeditionBackpack);

    
    FGameItem PillowBackpack;
    PillowBackpack.Name = TEXT("Pillow Backpack");
    PillowBackpack.Description = TEXT("A soft, cushioned backpack.");
    PillowBackpack.Rarity = EItemRarity::Rare;
    PillowBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/pillow_backpack_icon.pillow_backpack_icon")));
    PillowBackpack.Weight = 6.0f;
    PillowBackpack.ItemType = EItemType::Equipment;
    PillowBackpack.bIsStackable = false;
    PillowBackpack.StackCount = 1;
    PillowBackpack.UseType = EItemUseType::Equipment;
    PillowBackpack.UseAction = TEXT("Equip Backpack");
    PillowBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    PillowBackpack.EquipmentAttributes.CapacityBonus = 120;
    RegisterItem(PillowBackpack);

    
    FGameItem PirateBag;
    PirateBag.Name = TEXT("Pirate Bag");
    PirateBag.Description = TEXT("A weathered bag used by pirates.");
    PirateBag.Rarity = EItemRarity::Rare;
    PirateBag.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/pirate_bag_icon.pirate_bag_icon")));
    PirateBag.Weight = 8.0f;
    PirateBag.ItemType = EItemType::Equipment;
    PirateBag.bIsStackable = false;
    PirateBag.StackCount = 1;
    PirateBag.UseType = EItemUseType::Equipment;
    PirateBag.UseAction = TEXT("Equip Backpack");
    PirateBag.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    PirateBag.EquipmentAttributes.CapacityBonus = 130;
    RegisterItem(PirateBag);

    
    FGameItem PurpleBackpack;
    PurpleBackpack.Name = TEXT("Purple Backpack");
    PurpleBackpack.Description = TEXT("A simple purple backpack.");
    PurpleBackpack.Rarity = EItemRarity::Uncommon;
    PurpleBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/purple_backpack_icon.purple_backpack_icon")));
    PurpleBackpack.Weight = 7.0f;
    PurpleBackpack.ItemType = EItemType::Equipment;
    PurpleBackpack.bIsStackable = false;
    PurpleBackpack.StackCount = 1;
    PurpleBackpack.UseType = EItemUseType::Equipment;
    PurpleBackpack.UseAction = TEXT("Equip Backpack");
    PurpleBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    PurpleBackpack.EquipmentAttributes.CapacityBonus = 35;
    RegisterItem(PurpleBackpack);

    
    FGameItem YellowBackpack;
    YellowBackpack.Name = TEXT("Yellow Backpack");
    YellowBackpack.Description = TEXT("A simple yellow backpack.");
    YellowBackpack.Rarity = EItemRarity::Uncommon;
    YellowBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/yellow_backpack_icon.yellow_backpack_icon")));
    YellowBackpack.Weight = 7.0f;
    YellowBackpack.ItemType = EItemType::Equipment;
    YellowBackpack.bIsStackable = false;
    YellowBackpack.StackCount = 1;
    YellowBackpack.UseType = EItemUseType::Equipment;
    YellowBackpack.UseAction = TEXT("Equip Backpack");
    YellowBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    YellowBackpack.EquipmentAttributes.CapacityBonus = 40;
    RegisterItem(YellowBackpack);

    
    FGameItem BuggyBackpack;
    BuggyBackpack.Name = TEXT("Buggy Backpack");
    BuggyBackpack.Description = TEXT("A backpack decorated with bug patterns.");
    BuggyBackpack.Rarity = EItemRarity::Rare;
    BuggyBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/buggy_backpack_icon.buggy_backpack_icon")));
    BuggyBackpack.Weight = 7.0f;
    BuggyBackpack.ItemType = EItemType::Equipment;
    BuggyBackpack.bIsStackable = false;
    BuggyBackpack.StackCount = 1;
    BuggyBackpack.UseType = EItemUseType::Equipment;
    BuggyBackpack.UseAction = TEXT("Equip Backpack");
    BuggyBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    BuggyBackpack.EquipmentAttributes.CapacityBonus = 140;
    RegisterItem(BuggyBackpack);

    
    FGameItem DragonSlayer;
    DragonSlayer.Name = TEXT("Dragon Slayer");
    DragonSlayer.Description = TEXT("A legendary sword used to slay dragons.");
    DragonSlayer.Rarity = EItemRarity::Rare;
    DragonSlayer.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/dragon_slayer_icon.dragon_slayer_icon")));
    DragonSlayer.Weight = 84.0f;
    DragonSlayer.ItemType = EItemType::Equipment;
    DragonSlayer.bIsStackable = false;
    DragonSlayer.StackCount = 1;
    DragonSlayer.UseType = EItemUseType::Equipment;
    DragonSlayer.UseAction = TEXT("Equip Weapon");
    DragonSlayer.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    DragonSlayer.EquipmentAttributes.Attack = 49;
    DragonSlayer.EquipmentAttributes.Defense = 30;
    RegisterItem(DragonSlayer);

    
    FGameItem CakeBackpack;
    CakeBackpack.Name = TEXT("Cake Backpack");
    CakeBackpack.Description = TEXT("A backpack shaped like a delicious cake.");
    CakeBackpack.Rarity = EItemRarity::Rare;
    CakeBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/cake_backpack_icon.cake_backpack_icon")));
    CakeBackpack.Weight = 7.0f;
    CakeBackpack.ItemType = EItemType::Equipment;
    CakeBackpack.bIsStackable = false;
    CakeBackpack.StackCount = 1;
    CakeBackpack.UseType = EItemUseType::Equipment;
    CakeBackpack.UseAction = TEXT("Equip Backpack");
    CakeBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    CakeBackpack.EquipmentAttributes.CapacityBonus = 150;
    RegisterItem(CakeBackpack);

    
    FGameItem CamouflageBackpack;
    CamouflageBackpack.Name = TEXT("Camouflage Backpack");
    CamouflageBackpack.Description = TEXT("A backpack with military camouflage pattern.");
    CamouflageBackpack.Rarity = EItemRarity::Rare;
    CamouflageBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/camouflage_backpack_icon.camouflage_backpack_icon")));
    CamouflageBackpack.Weight = 8.0f;
    CamouflageBackpack.ItemType = EItemType::Equipment;
    CamouflageBackpack.bIsStackable = false;
    CamouflageBackpack.StackCount = 1;
    CamouflageBackpack.UseType = EItemUseType::Equipment;
    CamouflageBackpack.UseAction = TEXT("Equip Backpack");
    CamouflageBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    CamouflageBackpack.EquipmentAttributes.CapacityBonus = 160;
    RegisterItem(CamouflageBackpack);

    
    FGameItem CrownBackpack;
    CrownBackpack.Name = TEXT("Crown Backpack");
    CrownBackpack.Description = TEXT("A backpack decorated with a royal crown pattern.");
    CrownBackpack.Rarity = EItemRarity::Legendary;
    CrownBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/crown_backpack_icon.crown_backpack_icon")));
    CrownBackpack.Weight = 8.0f;
    CrownBackpack.ItemType = EItemType::Equipment;
    CrownBackpack.bIsStackable = false;
    CrownBackpack.StackCount = 1;
    CrownBackpack.UseType = EItemUseType::Equipment;
    CrownBackpack.UseAction = TEXT("Equip Backpack");
    CrownBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    CrownBackpack.EquipmentAttributes.CapacityBonus = 700;
    RegisterItem(CrownBackpack);

    
    FGameItem DemonBackpack;
    DemonBackpack.Name = TEXT("Demon Backpack");
    DemonBackpack.Description = TEXT("A backpack with demonic designs.");
    DemonBackpack.Rarity = EItemRarity::Legendary;
    DemonBackpack.Rarity = EItemRarity::Epic;
    DemonBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/demon_backpack_icon.demon_backpack_icon")));
    DemonBackpack.Weight = 10.0f;
    DemonBackpack.ItemType = EItemType::Equipment;
    DemonBackpack.bIsStackable = false;
    DemonBackpack.StackCount = 1;
    DemonBackpack.UseType = EItemUseType::Equipment;
    DemonBackpack.UseAction = TEXT("Equip Backpack");
    DemonBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    DemonBackpack.EquipmentAttributes.CapacityBonus = 750;
    RegisterItem(DemonBackpack);

    
    FGameItem FestiveBackpack;
    FestiveBackpack.Name = TEXT("Festive Backpack");
    FestiveBackpack.Description = TEXT("A backpack decorated with festive patterns.");
    FestiveBackpack.Rarity = EItemRarity::Epic;
    FestiveBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/festive_backpack_icon.festive_backpack_icon")));
    FestiveBackpack.Weight = 7.0f;
    FestiveBackpack.ItemType = EItemType::Equipment;
    FestiveBackpack.bIsStackable = false;
    FestiveBackpack.StackCount = 1;
    FestiveBackpack.UseType = EItemUseType::Equipment;
    FestiveBackpack.UseAction = TEXT("Equip Backpack");
    FestiveBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    FestiveBackpack.EquipmentAttributes.CapacityBonus = 240;
    RegisterItem(FestiveBackpack);

    
    FGameItem LilypadBackpack;
    LilypadBackpack.Name = TEXT("Lilypad Backpack");
    LilypadBackpack.Description = TEXT("A backpack decorated with lilypad patterns.");
    LilypadBackpack.Rarity = EItemRarity::Epic;
    LilypadBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/lilypad_backpack_icon.lilypad_backpack_icon")));
    LilypadBackpack.Weight = 7.0f;
    LilypadBackpack.ItemType = EItemType::Equipment;
    LilypadBackpack.bIsStackable = false;
    LilypadBackpack.StackCount = 1;
    LilypadBackpack.UseType = EItemUseType::Equipment;
    LilypadBackpack.UseAction = TEXT("Equip Backpack");
    LilypadBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    LilypadBackpack.EquipmentAttributes.CapacityBonus = 260;
    RegisterItem(LilypadBackpack);

    
    FGameItem MinotaurBackpack;
    MinotaurBackpack.Name = TEXT("Minotaur Backpack");
    MinotaurBackpack.Description = TEXT("A backpack decorated with minotaur patterns.");
    MinotaurBackpack.Rarity = EItemRarity::Epic;
    MinotaurBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/minotaur_backpack_icon.minotaur_backpack_icon")));
    MinotaurBackpack.Weight = 9.0f;
    MinotaurBackpack.ItemType = EItemType::Equipment;
    MinotaurBackpack.bIsStackable = false;
    MinotaurBackpack.StackCount = 1;
    MinotaurBackpack.UseType = EItemUseType::Equipment;
    MinotaurBackpack.UseAction = TEXT("Equip Backpack");
    MinotaurBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    MinotaurBackpack.EquipmentAttributes.CapacityBonus = 280;
    RegisterItem(MinotaurBackpack);

    
    FGameItem RaccoonBackpack;
    RaccoonBackpack.Name = TEXT("Raccoon Backpack");
    RaccoonBackpack.Description = TEXT("A backpack decorated with raccoon patterns.");
    RaccoonBackpack.Rarity = EItemRarity::Epic;
    RaccoonBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/raccoon_backpack_icon.raccoon_backpack_icon")));
    RaccoonBackpack.Weight = 7.0f;
    RaccoonBackpack.ItemType = EItemType::Equipment;
    RaccoonBackpack.bIsStackable = false;
    RaccoonBackpack.StackCount = 1;
    RaccoonBackpack.UseType = EItemUseType::Equipment;
    RaccoonBackpack.UseAction = TEXT("Equip Backpack");
    RaccoonBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    RaccoonBackpack.EquipmentAttributes.CapacityBonus = 300;
    RegisterItem(RaccoonBackpack);

    
    FGameItem WolfBackpack;
    WolfBackpack.Name = TEXT("Wolf Backpack");
    WolfBackpack.Description = TEXT("A backpack decorated with wolf patterns.");
    WolfBackpack.Rarity = EItemRarity::Epic;
    WolfBackpack.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/wolf_backpack_icon.wolf_backpack_icon")));
    WolfBackpack.Weight = 8.0f;
    WolfBackpack.ItemType = EItemType::Equipment;
    WolfBackpack.bIsStackable = false;
    WolfBackpack.StackCount = 1;
    WolfBackpack.UseType = EItemUseType::Equipment;
    WolfBackpack.UseAction = TEXT("Equip Backpack");
    WolfBackpack.EquipmentAttributes.EquipmentType = EEquipmentType::Bag;
    WolfBackpack.EquipmentAttributes.CapacityBonus = 320;
    RegisterItem(WolfBackpack);
}

void UItemDatabase::RegisterConsumables()
{
    FGameItem ManaFluid;
    ManaFluid.Name = TEXT("Mana Fluid");
    ManaFluid.Description = TEXT("A magical fluid that restores 75-175 mana points when consumed.");
    ManaFluid.Rarity = EItemRarity::Junk;
    ManaFluid.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/manafluid_icon.manafluid_icon")));
    ManaFluid.Weight = 1.8f;
    ManaFluid.ItemType = EItemType::Consumable;
    ManaFluid.bIsStackable = true;
    ManaFluid.StackCount = 1;
    ManaFluid.UseType = EItemUseType::Consumable;
    ManaFluid.UseAction = TEXT("Mana Fluid");
    RegisterItem(ManaFluid);

    FGameItem RewardPresent;
    RewardPresent.Name = TEXT("Reward Present");
    RewardPresent.Description = TEXT("A present containing a reward.");
    RewardPresent.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/UI/Items/sweet_present_icon.sweet_present_icon")));
    RewardPresent.Weight = 1.5f;
    RewardPresent.ItemType = EItemType::Consumable;
    RewardPresent.bIsStackable = false;
    RewardPresent.StackCount = 1;
    RewardPresent.UseType = EItemUseType::Consumable;
    RewardPresent.UseAction = TEXT("Open Present");
    RegisterItem(RewardPresent);
}

void UItemDatabase::RegisterRunes()
{
    FGameItem SuddenDeathRune;
    SuddenDeathRune.Name = TEXT("Sudden Death Rune");
    SuddenDeathRune.Description = TEXT("A powerful rune that deals heavy damage to a single target.");
    SuddenDeathRune.Rarity = EItemRarity::Junk;
    SuddenDeathRune.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/sudden_death_rune_icon.sudden_death_rune_icon")));
    SuddenDeathRune.Weight = 0.7f;
    SuddenDeathRune.ItemType = EItemType::Rune;
    SuddenDeathRune.bIsStackable = true;
    SuddenDeathRune.StackCount = 1;
    SuddenDeathRune.UseType = EItemUseType::RuneSpell;
    SuddenDeathRune.UseAction = TEXT("Sudden Death");
    RegisterItem(SuddenDeathRune);

    FGameItem UltimateHealingRune;
    UltimateHealingRune.Name = TEXT("Ultimate Healing Rune");
    UltimateHealingRune.Description = TEXT("A powerful healing rune that restores a large amount of health.");
    UltimateHealingRune.Rarity = EItemRarity::Junk;
    UltimateHealingRune.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/ultimate_healing_rune_icon.ultimate_healing_rune_icon")));
    UltimateHealingRune.Weight = 2.1f;
    UltimateHealingRune.ItemType = EItemType::Rune;
    UltimateHealingRune.bIsStackable = true;
    UltimateHealingRune.StackCount = 1;
    UltimateHealingRune.UseType = EItemUseType::RuneSpell;
    UltimateHealingRune.UseAction = TEXT("Ultimate Healing");
    RegisterItem(UltimateHealingRune);

    FGameItem GreatFireballRune;
    GreatFireballRune.Name = TEXT("Great Fireball Rune");
    GreatFireballRune.Description = TEXT("A powerful rune that creates a large explosion of fire.");
    GreatFireballRune.Rarity = EItemRarity::Junk;
    GreatFireballRune.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/great_fireball_rune_icon.great_fireball_rune_icon")));
    GreatFireballRune.Weight = 1.2f;
    GreatFireballRune.ItemType = EItemType::Rune;
    GreatFireballRune.bIsStackable = true;
    GreatFireballRune.StackCount = 1;
    GreatFireballRune.UseType = EItemUseType::RuneSpell;
    GreatFireballRune.UseAction = TEXT("Great Fireball Rune");
    RegisterItem(GreatFireballRune);

    FGameItem MagicWallRune;
    MagicWallRune.Name = TEXT("Magic Wall Rune");
    MagicWallRune.Description = TEXT("A magical rune that creates a temporary wall of force.");
    MagicWallRune.Rarity = EItemRarity::Junk;
    MagicWallRune.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/magic_wall_rune_icon.magic_wall_rune_icon")));
    MagicWallRune.Weight = 0.9f;
    MagicWallRune.ItemType = EItemType::Rune;
    MagicWallRune.bIsStackable = true;
    MagicWallRune.StackCount = 1;
    MagicWallRune.UseType = EItemUseType::RuneSpell;
    MagicWallRune.UseAction = TEXT("Magic Wall Rune");
    RegisterItem(MagicWallRune);

    FGameItem ExplosionRune;
    ExplosionRune.Name = TEXT("Explosion Rune");
    ExplosionRune.Description = TEXT("A powerful rune that creates a small explosion.");
    ExplosionRune.Rarity = EItemRarity::Junk;
    ExplosionRune.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/HUDs/Images/explosion_rune_icon.explosion_rune_icon")));
    ExplosionRune.Weight = 0.9f;
    ExplosionRune.ItemType = EItemType::Rune;
    ExplosionRune.bIsStackable = true;
    ExplosionRune.StackCount = 1;
    ExplosionRune.UseType = EItemUseType::RuneSpell;
    ExplosionRune.UseAction = TEXT("Explosion Rune");
    RegisterItem(ExplosionRune);
}

void UItemDatabase::RegisterItem(const FGameItem &Item)
{
    Items.Add(Item.Name, Item);

    
    if (!ItemsByRarity.Contains(Item.Rarity))
    {
        ItemsByRarity.Add(Item.Rarity, TArray<FGameItem>());
    }
    ItemsByRarity[Item.Rarity].Add(Item);
}

const FGameItem *UItemDatabase::GetItem(const FString &ItemName)
{
    return Items.Find(ItemName);
}

const TArray<FGameItem> *UItemDatabase::GetItemsByRarity(EItemRarity Rarity)
{
    return ItemsByRarity.Find(Rarity);
}