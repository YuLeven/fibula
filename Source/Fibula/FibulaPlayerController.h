#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FibulaPlayerController.generated.h"

UCLASS()
class FIBULA_API AFibulaPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AFibulaPlayerController();
    virtual void BeginPlay() override;

    virtual void PawnLeavingGame() override;
};