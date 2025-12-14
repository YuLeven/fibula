#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingTextWidget.generated.h"

UENUM(BlueprintType)
enum class EFloatingTextType : uint8
{
    Alert,     // Red, center screen
    MinorInfo, // White, bottom screen
    MajorInfo  // White, center screen
};

UENUM(BlueprintType)
enum class EFloatingTextChannel : uint8
{
    Alert,     // Shared by Alert and MajorInfo messages
    MinorInfo, // System messages
    Chat       // Player chat messages
};

USTRUCT()
struct FQueuedMessage
{
    GENERATED_BODY()

    FString Message;
    EFloatingTextType TextType;
    float Duration;
    float CurrentLifetime;
    float FadeStartTime;

    FQueuedMessage()
        : TextType(EFloatingTextType::MinorInfo), Duration(3.0f), CurrentLifetime(0.0f),
          FadeStartTime(2.0f)
    {
    }

    FQueuedMessage(const FString &InMessage, EFloatingTextType InTextType, float InDuration)
        : Message(InMessage), TextType(InTextType), Duration(InDuration), CurrentLifetime(0.0f),
          FadeStartTime(InDuration - 1.0f)
    {
    }
};

UCLASS()
class FIBULA_API UFloatingTextWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    static UFloatingTextWidget *GetOrCreateWidgetForChannel(
        APlayerController *PC,
        TSubclassOf<UFloatingTextWidget> WidgetClass,
        EFloatingTextChannel Channel,
        const FString &ChatPlayerName = TEXT(""));

    void InitializeFloatingText(EFloatingTextChannel InChannel, const FString &InChatPlayerName = TEXT(""));
    bool CanHandleMessage(EFloatingTextType TextType, const FString &ChatPlayerName = TEXT("")) const;

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

    void SetText(const FString &PlayerName, const FString &Message);
    void SetSystemText(const FString &Message, EFloatingTextType TextType = EFloatingTextType::MinorInfo, float Duration = 3.0f);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock *TextBlock;

    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor AlertColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);

    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor InfoColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor ChatColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);

private:
    EFloatingTextChannel Channel;
    FString ChatPlayerName;
    TArray<FQueuedMessage> MessageQueue;

    float CurrentLifetime;
    float MaxLifetime;
    float FadeStartTime;
    bool bIsShowingMessage;
    EFloatingTextType CurrentTextType;

    void ShowNextMessage();
};