#include "ProtectionZoneVolume.h"

AProtectionZoneVolume::AProtectionZoneVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    ProtectionZone = CreateDefaultSubobject<UProtectionZoneComponent>(TEXT("ProtectionZone"));
    RootComponent = ProtectionZone;
}