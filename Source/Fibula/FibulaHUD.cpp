#include "FibulaHUD.h"
#include "Blueprint/UserWidget.h"
#include "FibulaGameState.h"
#include "UObject/ConstructorHelpers.h"
#include "FloatingTextWidget.h"
#include "StatusWidget.h"
#include "FibulaCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AFibulaHUD::AFibulaHUD()
{
    static ConstructorHelpers::FClassFinder<UFloatingTextWidget> FloatingTextBPClass(TEXT("/Game/HUDs/Widgets/WBP_FloatingTextWidget"));
    if (FloatingTextBPClass.Class != nullptr)
    {
        FloatingTextWidgetClass = FloatingTextBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<UFloatingNumberWidget> FloatingNumberBPClass(TEXT("/Game/HUDs/Widgets/WBP_FloatingRaisingNumberWidget"));
    if (FloatingNumberBPClass.Class != nullptr)
    {
        FloatingNumberWidgetClass = FloatingNumberBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<UStatusWidget> StatusWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_StatusWidget"));
    if (StatusWidgetBPClass.Class != nullptr)
    {
        StatusWidgetClass = StatusWidgetBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<ULootWidget> LootWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_LootWindow"));
    if (LootWidgetBPClass.Class != nullptr)
    {
        LootWidgetClass = LootWidgetBPClass.Class;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TargetingCircleSystem(TEXT("/Game/Effects/NS_Targeting_Circle_Simple"));
    if (TargetingCircleSystem.Succeeded())
    {
        TargetingCircleTemplate = TargetingCircleSystem.Object;
    }
}

void AFibulaHUD::BeginPlay()
{
    Super::BeginPlay();

    if (!StatusWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("StatusWidgetClass not found"));
        return;
    }

    StatusWidget = CreateWidget<UStatusWidget>(GetOwningPlayerController(), StatusWidgetClass);
    if (!StatusWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("StatusWidget not created in HUD"));
        return;
    }

    StatusWidget->AddToViewport();
}

void AFibulaHUD::StartItemTargeting(UGameSlot *SourceSlot, const FGameItem &Item)
{
    bIsTargetingItem = true;
    TargetingSourceSlot = SourceSlot;
    TargetingItem = Item;

    if (APlayerController *PC = GetOwningPlayerController())
    {
        PC->bShowMouseCursor = true;
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(InputMode);

        
        if (TargetingCircleTemplate && !ActiveTargetingCircle)
        {
            ActiveTargetingCircle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                TargetingCircleTemplate,
                FVector::ZeroVector,
                FRotator::ZeroRotator);
        }
    }
}

void AFibulaHUD::EndItemTargeting()
{
    bIsTargetingItem = false;
    TargetingSourceSlot = nullptr;

    if (ActiveTargetingCircle)
    {
        ActiveTargetingCircle->DestroyComponent();
        ActiveTargetingCircle = nullptr;
    }

    if (APlayerController *PC = GetOwningPlayerController())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(InputMode);
        PC->SetShowMouseCursor(true);
    }
}

void AFibulaHUD::DrawHUD()
{
    Super::DrawHUD();

    if (bIsTargetingItem)
    {
        
        if (ActiveTargetingCircle)
        {
            APlayerController *PC = GetOwningPlayerController();
            if (PC)
            {
                FVector WorldLocation, WorldDirection;
                if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
                {
                    FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
                    FVector TraceEnd = CameraLocation + WorldDirection * 10000.0f;

                    FHitResult GroundHit;
                    FCollisionQueryParams QueryParams;
                    QueryParams.bTraceComplex = true;

                    if (GetWorld()->LineTraceSingleByChannel(
                            GroundHit,
                            CameraLocation,
                            TraceEnd,
                            ECC_GameTraceChannel2,
                            QueryParams))
                    {
                        
                        ActiveTargetingCircle->SetWorldLocation(GroundHit.Location + FVector(0, 0, 1));
                    }
                }
            }

            
            if (PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
            {
                HandleItemTargetingClick();
            }
            else if (PC->WasInputKeyJustPressed(EKeys::RightMouseButton))
            {
                EndItemTargeting();
            }
        }
    }

    if (APlayerController *PC = GetOwningPlayerController())
    {
        if (PC->WasInputKeyJustPressed(EKeys::Tab))
        {
            if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
            {
                bool bHealingTarget = PC->IsInputKeyDown(EKeys::LeftControl) ||
                                      PC->IsInputKeyDown(EKeys::RightControl);
                Character->HandleTabTargeting(bHealingTarget);
            }
        }
    }
}

void AFibulaHUD::HandleItemTargetingClick()
{
    if (!bIsTargetingItem || !TargetingSourceSlot)
        return;

    APlayerController *PC = GetOwningPlayerController();
    AFibulaCharacter *Character = PC ? Cast<AFibulaCharacter>(PC->GetPawn()) : nullptr;
    if (!PC || !Character)
        return;

    
    FVector WorldLocation, WorldDirection;
    if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        
        FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
        FVector TraceEnd = CameraLocation + WorldDirection * 10000.0f;

        FHitResult GroundHit;
        FCollisionQueryParams QueryParams;
        QueryParams.bTraceComplex = true;

        bool bHitGround = GetWorld()->LineTraceSingleByChannel(
            GroundHit,
            CameraLocation,
            TraceEnd,
            ECC_GameTraceChannel2,
            QueryParams);

        FVector IntersectionPoint;
        if (bHitGround)
        {
            IntersectionPoint = GroundHit.Location;
        }
        else
        {
            
            IntersectionPoint = CameraLocation + WorldDirection * 1000.0f;
        }

        
        Character->ServerUseItem(TargetingItem, IntersectionPoint);
    }

    EndItemTargeting();
}

void AFibulaHUD::ShowLootWindow(AItemContainer *Container)
{
    UE_LOG(LogTemp, Warning, TEXT("ShowLootWindow: Attempting to show loot window"));
    if (!LootWidget && LootWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowLootWindow: Creating loot widget"));
        LootWidget = CreateWidget<ULootWidget>(GetOwningPlayerController(), LootWidgetClass);
    }

    if (LootWidget)
    {
        LootWidget->SetSourceContainer(Container);
        LootWidget->AddToViewport();
    }
}

void AFibulaHUD::HideLootWindow()
{
    if (LootWidget && LootWidget->IsInViewport())
    {
        LootWidget->RemoveFromViewport();
    }
}

void AFibulaHUD::HandleHotkeyPressed(const FKey &PressedKey)
{
    if (UGameSlot *Slot = FindSlotByHotkey(PressedKey))
    {
        Slot->ExecuteActionBarAction();
    }
}

UGameSlot *AFibulaHUD::FindSlotByHotkey(const FKey &Hotkey) const
{
    if (!StatusWidget || !StatusWidget->GetActionBarPanel())
        return nullptr;

    UUniformGridPanel *ActionBarPanel = Cast<UUniformGridPanel>(StatusWidget->GetActionBarPanel());
    if (!ActionBarPanel)
        return nullptr;

    
    for (UWidget *Child : ActionBarPanel->GetAllChildren())
    {
        if (UGameSlot *Slot = Cast<UGameSlot>(Child))
        {
            if (Slot->GetAssignedHotkey() == Hotkey)
            {
                return Slot;
            }
        }
    }

    return nullptr;
}