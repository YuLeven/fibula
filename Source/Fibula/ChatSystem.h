#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChatSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChatMessageReceived, const FString &, SenderName, const FString &, Message);

UCLASS()
class FIBULA_API AChatSystem : public AActor
{
    GENERATED_BODY()

public:
    AChatSystem();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastBroadcastMessage(const FString &SenderName, const FString &Message);

protected:
    virtual void BeginPlay() override;
};