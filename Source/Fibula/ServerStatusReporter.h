#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "JsonObjectConverter.h"
#include "ServerStatusReporter.generated.h"

USTRUCT()
struct FPlayerInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Vocation;

    UPROPERTY()
    int32 Level = 1;
};

USTRUCT()
struct FServerStatus
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Port = 7777;

    UPROPERTY()
    int32 MaxPlayers = 100;

    UPROPERTY()
    int32 OnlinePlayers = 0;

    UPROPERTY()
    TArray<FPlayerInfo> Players;
};

UCLASS()
class FIBULA_API UServerStatusReporter : public UObject
{
    GENERATED_BODY()

public:
    UServerStatusReporter();

    void Initialize(int32 InPort, int32 InMaxPlayers);
    void StartReporting();
    void StopReporting();

private:
    FServerStatus GatherServerStatus();
    void ReportStatus();
    void ReportCharacterStats();

private:
    int32 ServerPort;
    int32 MaxPlayers;
    bool bIsReporting;
    FTimerHandle ReportingTimerHandle;
    FTimerHandle StatsTimerHandle;
};