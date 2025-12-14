#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FloatingNumberUtils.generated.h"

UCLASS()
class FIBULA_API UFloatingNumberUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static void ShowFloatingNumber(APlayerController *LocalPC, AActor *Target, int32 Number, const FLinearColor &Color, FVector Offset = FVector(0, 0, 150));
};