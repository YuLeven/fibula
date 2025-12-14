#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "BackgroundMusicManager.generated.h"

UCLASS()
class FIBULA_API ABackgroundMusicManager : public AActor
{
    GENERATED_BODY()

public:
    ABackgroundMusicManager();

    void StartBattleMusic();
    void StopBattleMusic();
    void UpdateCurrentMusicVolume(float MasterVolume, float MusicVolume);

protected:
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;

private:
    UPROPERTY()
    TArray<USoundCue *> BattleMusicCues;

    UPROPERTY()
    UAudioComponent *CurrentMusicComponent;

    FTimerHandle MusicTimerHandle;

    void PlayRandomBattleMusic();
    void LoadBattleMusicCues();

    UFUNCTION()
    void OnMusicFinished();
};