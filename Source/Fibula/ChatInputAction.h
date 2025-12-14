#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "ChatInputAction.generated.h"

UCLASS(Config = Input)
class FIBULA_API UChatInputAction : public UInputAction
{
    GENERATED_BODY()

public:
    UChatInputAction()
    {
        ValueType = EInputActionValueType::Boolean;
        bConsumeInput = true;
    }
};