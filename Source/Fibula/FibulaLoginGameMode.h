#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FibulaLoginGameMode.generated.h"

UCLASS()
class FIBULA_API AFibulaLoginGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AFibulaLoginGameMode();

    virtual void InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class ULoginWidget> LoginWidgetClass;
};