#include "GameplayUtils.h"
#include "Kismet/GameplayStatics.h"

TArray<AFibulaCharacter *> UGameplayUtils::GetPlayersWithinRadius(const UObject *WorldContextObject, const FVector &Center, float Radius)
{
    TArray<AFibulaCharacter *> NearbyPlayers;

    if (!WorldContextObject)
        return NearbyPlayers;

    UWorld *World = WorldContextObject->GetWorld();
    if (!World)
        return NearbyPlayers;

    TArray<AActor *> AllPlayers;
    UGameplayStatics::GetAllActorsOfClass(World, AFibulaCharacter::StaticClass(), AllPlayers);

    for (AActor *Actor : AllPlayers)
    {
        if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(Actor))
        {
            float Distance = FVector::Distance(Center, Character->GetActorLocation());
            if (Distance <= Radius)
            {
                NearbyPlayers.Add(Character);
            }
        }
    }

    return NearbyPlayers;
}