#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PlayerNameTagWidget.h"
#include "NameTagWidgetComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class FIBULA_API UNameTagWidgetComponent : public UWidgetComponent
{
    GENERATED_BODY()

public:
    UNameTagWidgetComponent();

    virtual void BeginPlay() override;

    void SetCharacterName(const FString &Name);
    void UpdateHealth(float HealthPercentage);
    void UpdateSkullStatus(ESkullType SkullType);
    void RotateTowardsCamera();

protected:
    virtual void InitWidget() override;

private:
    FString PendingCharacterName;
    FTimerHandle RetrySetNameHandle;
    bool IsWidgetReady() const;
    void RetrySetName();
};