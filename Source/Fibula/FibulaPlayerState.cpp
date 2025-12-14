#include "FibulaPlayerState.h"
#include "Net/UnrealNetwork.h"

AFibulaPlayerState::AFibulaPlayerState()
{
    bReplicates = true;
}

void AFibulaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFibulaPlayerState, PlayerVocation);
}

void AFibulaPlayerState::SetPlayerVocation(const FString &InVocation)
{
    PlayerVocation = InVocation;
}