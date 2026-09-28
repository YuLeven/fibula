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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if !UE_BUILD_SHIPPING
	void RunBotTestDriverStep();
	FTimerHandle BotTestDriverTimerHandle;
	float BotTestDriverStartTime = 0.0f;
	float BotTestNextTargetTime = 0.0f;
	int32 BotTestMoveInputs = 0;
	int32 BotTestTargetCommands = 0;
	int32 BotTestObservedMovement = 0;
	int32 BotTestReplicatedDamage = 0;
	int32 BotTestLastHealth = -1;
	FVector BotTestLastLocation = FVector::ZeroVector;
#endif
};
