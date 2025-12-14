#include "FloatingTextWidget.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UFloatingTextWidget::NativeConstruct()
{
    Super::NativeConstruct();
    CurrentLifetime = 0.0f;
    MaxLifetime = 3.0f;
    FadeStartTime = 2.0f;
    CurrentTextType = EFloatingTextType::MinorInfo;
    bIsShowingMessage = false;

    if (TextBlock)
    {
        TextBlock->SetJustification(ETextJustify::Center);
        TextBlock->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UFloatingTextWidget::SetText(const FString &PlayerName, const FString &Message)
{
    if (TextBlock)
    {
        FString DisplayText = FString::Printf(TEXT("%s says:\n  %s"), *PlayerName, *Message);
        TextBlock->SetText(FText::FromString(DisplayText));
        TextBlock->SetColorAndOpacity(FSlateColor(ChatColor));
        TextBlock->SetVisibility(ESlateVisibility::Visible);

        
        CurrentLifetime = 0.0f;
        MaxLifetime = 3.0f;
        FadeStartTime = 2.0f;
        CurrentTextType = EFloatingTextType::MinorInfo;
        bIsShowingMessage = true;
    }
}

void UFloatingTextWidget::SetSystemText(const FString &Message, EFloatingTextType TextType, float Duration)
{
    
    if (TextType == EFloatingTextType::MinorInfo)
    {
        if (TextBlock)
        {
            TextBlock->SetText(FText::FromString(Message));
            TextBlock->SetColorAndOpacity(FSlateColor(InfoColor));
            TextBlock->SetVisibility(ESlateVisibility::Visible);

            
            CurrentLifetime = 0.0f;
            MaxLifetime = Duration;
            FadeStartTime = Duration - 1.0f;
            CurrentTextType = TextType;
            bIsShowingMessage = true;

            
            FVector2D ViewportSize;
            if (GEngine && GEngine->GameViewport)
            {
                GEngine->GameViewport->GetViewportSize(ViewportSize);
                SetPositionInViewport(FVector2D(ViewportSize.X * 0.5f, ViewportSize.Y * 0.8f));
            }
        }
        return;
    }

    
    MessageQueue.Add(FQueuedMessage(Message, TextType, Duration));

    
    if (!bIsShowingMessage)
    {
        ShowNextMessage();
    }
}

void UFloatingTextWidget::ShowNextMessage()
{
    if (MessageQueue.Num() == 0)
    {
        bIsShowingMessage = false;
        if (TextBlock)
        {
            TextBlock->SetVisibility(ESlateVisibility::Hidden);
        }
        return;
    }

    if (TextBlock)
    {
        FQueuedMessage Message = MessageQueue[0];
        MessageQueue.RemoveAt(0);

        TextBlock->SetText(FText::FromString(Message.Message));
        TextBlock->SetVisibility(ESlateVisibility::Visible);

        
        switch (Message.TextType)
        {
        case EFloatingTextType::Alert:
            TextBlock->SetColorAndOpacity(FSlateColor(AlertColor));
            break;
        case EFloatingTextType::MajorInfo:
            TextBlock->SetColorAndOpacity(FSlateColor(InfoColor));
            break;
        }

        MaxLifetime = Message.Duration;
        FadeStartTime = Message.Duration - 1.0f;
        CurrentLifetime = 0.0f;
        CurrentTextType = Message.TextType;
        bIsShowingMessage = true;

        
        FVector2D ViewportSize;
        if (GEngine && GEngine->GameViewport)
        {
            GEngine->GameViewport->GetViewportSize(ViewportSize);
            SetPositionInViewport(FVector2D(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f));
        }
    }
}

void UFloatingTextWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bIsShowingMessage)
        return;

    CurrentLifetime += InDeltaTime;

    if (CurrentLifetime >= MaxLifetime)
    {
        ShowNextMessage();
    }
    else if (CurrentLifetime >= FadeStartTime)
    {
        float Alpha = 1.0f - ((CurrentLifetime - FadeStartTime) / (MaxLifetime - FadeStartTime));
        FLinearColor Color;

        switch (CurrentTextType)
        {
        case EFloatingTextType::Alert:
            Color = AlertColor;
            break;
        case EFloatingTextType::MinorInfo:
        case EFloatingTextType::MajorInfo:
            Color = InfoColor;
            break;
        }

        TextBlock->SetColorAndOpacity(FSlateColor(Color * Alpha));
    }
}

UFloatingTextWidget *UFloatingTextWidget::GetOrCreateWidgetForChannel(
    APlayerController *PC,
    TSubclassOf<UFloatingTextWidget> WidgetClass,
    EFloatingTextChannel Channel,
    const FString &ChatPlayerName)
{
    if (!PC || !WidgetClass)
        return nullptr;

    
    TArray<UUserWidget *> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(PC->GetWorld(), FoundWidgets, WidgetClass);

    
    for (UUserWidget *Widget : FoundWidgets)
    {
        UFloatingTextWidget *FloatingText = Cast<UFloatingTextWidget>(Widget);
        if (FloatingText &&
            ((Channel == EFloatingTextChannel::Chat && FloatingText->CanHandleMessage(EFloatingTextType::MinorInfo, ChatPlayerName)) ||
             (Channel == EFloatingTextChannel::Alert && FloatingText->CanHandleMessage(EFloatingTextType::Alert, ChatPlayerName)) ||
             (Channel == EFloatingTextChannel::MinorInfo && FloatingText->CanHandleMessage(EFloatingTextType::MinorInfo, ChatPlayerName))))
        {
            return FloatingText;
        }
    }

    
    UFloatingTextWidget *NewWidget = CreateWidget<UFloatingTextWidget>(PC, WidgetClass);
    if (NewWidget)
    {
        NewWidget->InitializeFloatingText(Channel, ChatPlayerName);
        NewWidget->AddToViewport();
    }
    return NewWidget;
}

void UFloatingTextWidget::InitializeFloatingText(EFloatingTextChannel InChannel, const FString &InChatPlayerName)
{
    Channel = InChannel;
    ChatPlayerName = InChatPlayerName;
}

bool UFloatingTextWidget::CanHandleMessage(EFloatingTextType TextType, const FString &InChatPlayerName) const
{
    switch (Channel)
    {
    case EFloatingTextChannel::Alert:
        return TextType == EFloatingTextType::Alert || TextType == EFloatingTextType::MajorInfo;

    case EFloatingTextChannel::MinorInfo:
        return TextType == EFloatingTextType::MinorInfo;

    case EFloatingTextChannel::Chat:
        return !ChatPlayerName.IsEmpty() && ChatPlayerName == InChatPlayerName;

    default:
        return false;
    }
}