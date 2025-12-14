#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProtectionZoneComponent.h"
#include "ProtectionZoneVolume.generated.h"

UCLASS(HideCategories = (Input, Collision, Replication, Actor))
class FIBULA_API AProtectionZoneVolume : public AActor
{
    GENERATED_BODY()

public:
    AProtectionZoneVolume();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Protection Zone")
    UProtectionZoneComponent *ProtectionZone;

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    class UBillboardComponent *SpriteComponent;
#endif
};