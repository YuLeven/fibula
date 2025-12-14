#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "FloatingNumberWidget.generated.h"

UCLASS()
class FIBULA_API UFloatingNumberWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetNumber(int32 Number, const FLinearColor &Color);
    void StartFloat();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *NumberText;

private:
    FVector2D InitialPosition;
    float CurrentLifetime = 0.0f;
    const float MaxLifetime = 2.0f;
    const float FloatSpeed = 100.0f; 
};