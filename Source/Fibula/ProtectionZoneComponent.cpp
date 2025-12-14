#include "ProtectionZoneComponent.h"
#include "FibulaCharacter.h"

UProtectionZoneComponent::UProtectionZoneComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetGenerateOverlapEvents(true);

    
    FCollisionResponseContainer ResponseContainer;
    ResponseContainer.SetAllChannels(ECR_Overlap);

    
    SetCollisionResponseToChannels(ResponseContainer);
}

void UProtectionZoneComponent::BeginPlay()
{
    Super::BeginPlay();

    OnComponentBeginOverlap.AddDynamic(this, &UProtectionZoneComponent::OnOverlapBegin);
    OnComponentEndOverlap.AddDynamic(this, &UProtectionZoneComponent::OnOverlapEnd);
}

void UProtectionZoneComponent::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent,
                                              AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
                                              bool bFromSweep, const FHitResult &SweepResult)
{
    if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(OtherActor))
    {
        if (Character->IsInCombat())
        {
            
            FVector PushDirection = Character->GetActorLocation() - GetComponentLocation();
            PushDirection.Normalize();
            Character->AddActorWorldOffset(PushDirection * 100.0f);
            Character->ShowCenteredMessage(TEXT("You cannot enter a protection zone while in combat."), 3.0f, EFloatingTextType::MinorInfo);

            return;
        }

        Character->SetInProtectionZone(true);
    }
}

void UProtectionZoneComponent::OnOverlapEnd(UPrimitiveComponent *OverlappedComponent,
                                            AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(OtherActor))
    {
        Character->SetInProtectionZone(false);
    }
}