#include "BackgroundMusicManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "SoundSettingsManager.h"

ABackgroundMusicManager::ABackgroundMusicManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bAlwaysRelevant = true;

    
    LoadBattleMusicCues();
}

void ABackgroundMusicManager::LoadBattleMusicCues()
{
    static ConstructorHelpers::FObjectFinder<USoundCue> BGM1(TEXT("/Game/SoundFX/bgm_1_Cue"));
    static ConstructorHelpers::FObjectFinder<USoundCue> BGM2(TEXT("/Game/SoundFX/bgm_2_Cue"));
    static ConstructorHelpers::FObjectFinder<USoundCue> BGM3(TEXT("/Game/SoundFX/bgm_3_Cue"));
    static ConstructorHelpers::FObjectFinder<USoundCue> BGM4(TEXT("/Game/SoundFX/bgm_4_Cue"));
    static ConstructorHelpers::FObjectFinder<USoundCue> BGM5(TEXT("/Game/SoundFX/bgm_5_Cue"));
    static ConstructorHelpers::FObjectFinder<USoundCue> BGM6(TEXT("/Game/SoundFX/bgm_6_Cue"));
    static ConstructorHelpers::FObjectFinder<USoundCue> BGM7(TEXT("/Game/SoundFX/bgm_7_Cue"));

    if (BGM1.Succeeded())
        BattleMusicCues.Add(BGM1.Object);
    if (BGM2.Succeeded())
        BattleMusicCues.Add(BGM2.Object);
    if (BGM3.Succeeded())
        BattleMusicCues.Add(BGM3.Object);
    if (BGM4.Succeeded())
        BattleMusicCues.Add(BGM4.Object);
    if (BGM5.Succeeded())
        BattleMusicCues.Add(BGM5.Object);
    if (BGM6.Succeeded())
        BattleMusicCues.Add(BGM6.Object);
    if (BGM7.Succeeded())
        BattleMusicCues.Add(BGM7.Object);
}

void ABackgroundMusicManager::BeginPlay()
{
    Super::BeginPlay();
}

void ABackgroundMusicManager::BeginDestroy()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(MusicTimerHandle);
    }

    if (CurrentMusicComponent)
    {
        CurrentMusicComponent->Stop();
    }

    Super::BeginDestroy();
}

void ABackgroundMusicManager::StartBattleMusic()
{
    if (BattleMusicCues.Num() == 0)
        return;

    if (!CurrentMusicComponent)
    {
        PlayRandomBattleMusic();
    }
}

void ABackgroundMusicManager::StopBattleMusic()
{
    if (CurrentMusicComponent)
    {
        CurrentMusicComponent->FadeOut(2.0f, 0.0f);
        CurrentMusicComponent = nullptr;
    }

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(MusicTimerHandle);
    }
}

void ABackgroundMusicManager::PlayRandomBattleMusic()
{
    if (BattleMusicCues.Num() == 0)
        return;

    
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(MusicTimerHandle);
    }

    if (CurrentMusicComponent)
    {
        CurrentMusicComponent->Stop();
        CurrentMusicComponent = nullptr;
    }

    
    USoundSettingsManager *Settings = USoundSettingsManager::LoadSettings();
    if (!Settings)
        return;

    
    int32 RandomIndex = FMath::RandRange(0, BattleMusicCues.Num() - 1);
    USoundCue *SelectedMusic = BattleMusicCues[RandomIndex];

    if (!SelectedMusic)
        return;

    
    CurrentMusicComponent = UGameplayStatics::SpawnSound2D(this, SelectedMusic);
    if (CurrentMusicComponent)
    {
        float FinalVolume = Settings->MasterVolume * Settings->BackgroundMusicVolume * 0.5f;
        CurrentMusicComponent->SetVolumeMultiplier(FinalVolume);
        CurrentMusicComponent->OnAudioFinished.AddDynamic(this, &ABackgroundMusicManager::OnMusicFinished);
    }
}

void ABackgroundMusicManager::OnMusicFinished()
{
    
    CurrentMusicComponent = nullptr;

    
    PlayRandomBattleMusic();
}

void ABackgroundMusicManager::UpdateCurrentMusicVolume(float MasterVolume, float MusicVolume)
{
    if (CurrentMusicComponent)
    {
        float FinalVolume = MasterVolume * MusicVolume * 0.5f;
        CurrentMusicComponent->SetVolumeMultiplier(FinalVolume);

        if (!CurrentMusicComponent->IsPlaying())
        {
            PlayRandomBattleMusic();
        }
    }
}