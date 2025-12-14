#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "ProtectionZoneComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIBULA_API UProtectionZoneComponent : public UBoxComponent
{
    GENERATED_BODY()

public:
    UProtectionZoneComponent();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                        UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                      UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
};