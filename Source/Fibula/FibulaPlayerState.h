#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CharacterEnums.h"
#include "FibulaPlayerState.generated.h"

UCLASS()
class FIBULA_API AFibulaPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AFibulaPlayerState();

    void SetPlayerVocation(const FString &InVocation);
    FString GetPlayerVocation() const { return PlayerVocation; }

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated)
    FString PlayerVocation;
};