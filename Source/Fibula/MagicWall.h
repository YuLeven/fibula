#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicWall.generated.h"

UCLASS()
class FIBULA_API AMagicWall : public AActor
{
    GENERATED_BODY()

public:
    AMagicWall();

    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent *WallMesh;

    UPROPERTY(VisibleAnywhere)
    class UBoxComponent *CollisionBox;

    FTimerHandle DestructionTimer;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                        UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    void DestroyWall();
};