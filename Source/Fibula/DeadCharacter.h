#pragma once

#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "DeadCharacter.generated.h"

UCLASS()
class FIBULA_API ADeadCharacter : public AItemContainer
{
    GENERATED_BODY()

public:
    ADeadCharacter();
    virtual void BeginPlay() override;
    void SetCorpseMesh(USkeletalMesh *Mesh);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CorpseMesh)
    USkeletalMesh *ReplicatedMesh;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent *CorpseMesh;

    UFUNCTION()
    void OnRep_CorpseMesh();

    FTimerHandle DestructionTimer;
    void DestroyCorpse();
};