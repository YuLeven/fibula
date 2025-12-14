#include "ChatSystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "FibulaHUD.h"
#include "StatusWidget.h"

AChatSystem::AChatSystem()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bAlwaysRelevant = true;
    SetReplicateMovement(false);
}

void AChatSystem::BeginPlay()
{
    Super::BeginPlay();
}

void AChatSystem::MulticastBroadcastMessage_Implementation(const FString &SenderName, const FString &Message)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController *PC = It->Get())
        {
            if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
            {
                if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
                {
                    StatusWidget->AppendNewMessage(SenderName, Message);
                }
            }
        }
    }
}