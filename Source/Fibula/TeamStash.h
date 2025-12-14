#pragma once

#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "TeamStash.generated.h"

UCLASS()
class FIBULA_API ATeamStash : public AItemContainer
{
    GENERATED_BODY()

public:
    ATeamStash();

protected:
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent *ChestMesh;
};