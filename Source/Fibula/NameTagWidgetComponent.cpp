#include "NameTagWidgetComponent.h"
#include "PlayerNameTagWidget.h"

UNameTagWidgetComponent::UNameTagWidgetComponent()
{
    SetWidgetSpace(EWidgetSpace::World);
    SetDrawAtDesiredSize(true);
    SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
    SetComponentTickEnabled(true);
    SetTickMode(ETickMode::Automatic);
    PrimaryComponentTick.bCanEverTick = (GetNetMode() != NM_DedicatedServer);
    SetVisibility(true);

    static ConstructorHelpers::FClassFinder<UUserWidget> NameTagWidgetClass(TEXT("/Game/HUDs/Widgets/WBP_PlayerNameTag"));
    if (NameTagWidgetClass.Class)
    {
        SetWidgetClass(NameTagWidgetClass.Class);
    }
}

void UNameTagWidgetComponent::BeginPlay()
{
    Super::BeginPlay();
    InitWidget();
}

void UNameTagWidgetComponent::InitWidget()
{
    Super::InitWidget();

    if (UPlayerNameTagWidget *NameTagWidget = Cast<UPlayerNameTagWidget>(GetWidget()))
    {
        NameTagWidget->SetVisibility(ESlateVisibility::Visible);
        SetVisibility(true);
    }
}

void UNameTagWidgetComponent::SetCharacterName(const FString &Name)
{
    if (!IsWidgetReady())
    {
        
        InitWidget();
    }

    if (UPlayerNameTagWidget *NameTagWidget = Cast<UPlayerNameTagWidget>(GetWidget()))
    {
        NameTagWidget->SetCharacterName(Name);
    }
    else
    {
        
        PendingCharacterName = Name;

        
        GetWorld()->GetTimerManager().SetTimer(
            RetrySetNameHandle,
            FTimerDelegate::CreateUObject(this, &UNameTagWidgetComponent::RetrySetName),
            0.1f,
            true);
    }
}

void UNameTagWidgetComponent::RetrySetName()
{
    if (IsWidgetReady())
    {
        if (UPlayerNameTagWidget *NameTagWidget = Cast<UPlayerNameTagWidget>(GetWidget()))
        {
            NameTagWidget->SetCharacterName(PendingCharacterName);
            GetWorld()->GetTimerManager().ClearTimer(RetrySetNameHandle);
        }
    }
}

void UNameTagWidgetComponent::UpdateHealth(float HealthPercentage)
{
    if (UPlayerNameTagWidget *NameTagWidget = Cast<UPlayerNameTagWidget>(GetWidget()))
    {
        NameTagWidget->UpdateHealth(HealthPercentage);
    }
}

void UNameTagWidgetComponent::RotateTowardsCamera()
{
    if (APlayerController *PC = GetWorld()->GetFirstPlayerController())
    {
        if (APlayerCameraManager *CameraManager = PC->PlayerCameraManager)
        {
            FVector CameraLocation = CameraManager->GetCameraLocation();
            FRotator NewRotation = (CameraLocation - GetComponentLocation()).Rotation();
            SetWorldRotation(NewRotation);
        }
    }
}

void UNameTagWidgetComponent::UpdateSkullStatus(ESkullType SkullType)
{
    if (UPlayerNameTagWidget *NameTagWidget = Cast<UPlayerNameTagWidget>(GetWidget()))
    {
        NameTagWidget->UpdateSkullStatus(SkullType);
    }
}

bool UNameTagWidgetComponent::IsWidgetReady() const
{
    return GetWidget() != nullptr && Cast<UPlayerNameTagWidget>(GetWidget()) != nullptr;
}