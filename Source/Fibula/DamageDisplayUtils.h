#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DamageTypes.h"
#include "DamageDisplayUtils.generated.h"

UCLASS()
class FIBULA_API UDamageDisplayUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    
    static void ShowFloatingNumber(APlayerController *LocalPC, AActor *Target, int32 Value, EDamageDisplayType DisplayType, FVector Offset = FVector(0, 0, 150));

    
    static void ShowFloatingNumberWithColor(APlayerController *LocalPC, AActor *Target, int32 Value, const FLinearColor &Color, FVector Offset = FVector(0, 0, 150));
};