

#include "FibulaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "FibulaHUD.h"
#include "GameFormulas.h"
#include "ChatSystem.h"
#include "SpellSystem.h"
#include "FibulaGameState.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "PlayerNameTagWidget.h"
#include "Components/DecalComponent.h"
#include "DamageDisplayUtils.h"
#include "Containers/Map.h"
#include "FloatingNumberWidget.h"
#include "FloatingNumberUtils.h"
#include "FloatingTextWidget.h"
#include "SpellDatabase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerStartingElements.h"
#include "Particles/ParticleSystemComponent.h"
#include "NameTagWidgetComponent.h"
#include "FibulaPlayerState.h"
#include "ProtectionZoneComponent.h"
#include "FibulaGameMode.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "SpellProjectileAnimationActor.h"
#include "GameplayUtils.h"
#include "SpellEffectDisplay.h"
#include "ItemContainer.h"
#include "DeadCharacter.h"
#include "EquipmentTypes.h"
#include "RewardSystem.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "LoginWidget.h"
#include "FibulaTeamBattleGameMode.h"
#include "SoundSettingsManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);




AFibulaCharacter::AFibulaCharacter()
{
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	Vocation = EVocation::None;
	CharacterLevel = UPlayerStartingElements::GetStartingLevel();
	MagicLevel = UPlayerStartingElements::GetStartingMagicLevel(Vocation);
	Experience = GameFormulas::CalculateExperienceForLevel(CharacterLevel);
	Speed = GameFormulas::CalculateSpeed(CharacterLevel, Vocation);
	DistanceSkill = UPlayerStartingElements::GetStartingDistanceSkill(Vocation);
	MeleeSkill = UPlayerStartingElements::GetStartingMeleeSkill(Vocation);
	ShieldingSkill = UPlayerStartingElements::GetStartingShieldingSkill(Vocation);
	UpdateAttributes();

	PrimaryActorTick.bCanEverTick = (GetNetMode() != NM_DedicatedServer);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;		// The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;								// Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	NameTagWidget = CreateDefaultSubobject<UNameTagWidgetComponent>(TEXT("NameTagWidget"));
	NameTagWidget->SetupAttachment(RootComponent);
	NameTagWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	NameTagWidget->PrimaryComponentTick.bCanEverTick = (GetNetMode() != NM_DedicatedServer);

	TargetNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TargetNiagaraComponent"));
	TargetNiagaraComponent->PrimaryComponentTick.bCanEverTick = false;

	HealingTargetNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HealingTargetNiagaraComponent"));
	HealingTargetNiagaraComponent->PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TargetNiagaraFinder(TEXT("/Game/Effects/NS_TargetDecal"));
	if (TargetNiagaraFinder.Succeeded())
	{
		TargetNiagaraSystem = TargetNiagaraFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> HealingTargetNiagaraFinder(TEXT("/Game/Effects/NS_HealingTargetDecal"));
	if (HealingTargetNiagaraFinder.Succeeded())
	{
		HealingTargetNiagaraSystem = HealingTargetNiagaraFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> TargetActionAsset(TEXT("/Game/Input/IA_TargetCharacter"));
	if (TargetActionAsset.Succeeded())
	{
		TargetAction = TargetActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> EscapeActionAsset(TEXT("/Game/Input/IA_Escape"));
	if (EscapeActionAsset.Succeeded())
	{
		EscapeAction = EscapeActionAsset.Object;
	}

	DamageDealers = TMap<AFibulaCharacter *, FDamageDealer>();

	LastCombatTime = -PROTECTION_ZONE_COMBAT_DELAY;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TeamBMaterialFinder(TEXT("/Game/BattleWizardPolyart/Materials/MaskTintPolyartMatInst02.MaskTintPolyartMatInst02"));
	if (TeamBMaterialFinder.Succeeded())
	{
		TeamBMaterial = TeamBMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> BowSoundFinder(TEXT("/Game/SoundFX/bow_and_arrow_Cue"));
	if (BowSoundFinder.Succeeded())
	{
		BowAndArrowSound = BowSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SwordHitSoundFinder(TEXT("/Game/SoundFX/sword_hit_cue"));
	if (SwordHitSoundFinder.Succeeded())
	{
		SwordHitSounds.Add(SwordHitSoundFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SwordStabSoundFinder(TEXT("/Game/SoundFX/sword_stab_Cue"));
	if (SwordStabSoundFinder.Succeeded())
	{
		SwordHitSounds.Add(SwordStabSoundFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SwordSwingSoundFinder(TEXT("/Game/SoundFX/sword_swing_Cue"));
	if (SwordSwingSoundFinder.Succeeded())
	{
		SwordHitSounds.Add(SwordSwingSoundFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SwordStabAltSoundFinder(TEXT("/Game/SoundFX/sword_stab_alt_Cue"));
	if (SwordStabAltSoundFinder.Succeeded())
	{
		SwordHitSounds.Add(SwordStabAltSoundFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SwordHitAltSoundFinder(TEXT("/Game/SoundFX/sword_hit_alt_Cue"));
	if (SwordHitAltSoundFinder.Succeeded())
	{
		SwordHitSounds.Add(SwordHitAltSoundFinder.Object);
	}

	// Create and setup the light component
	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComponent"));
	LightComponent->SetupAttachment(RootComponent);
	LightComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 150.0f));
	LightComponent->SetVisibility(false);
	LightComponent->SetIntensity(1000.0f);
	LightComponent->SetAttenuationRadius(4000.0f); // Changed from 2000.0f to 4000.0f
	LightComponent->SetLightColor(FLinearColor(1.0f, 0.9f, 0.7f));

	// Team A Meshes
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamADruidMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/ImmortalSK"));
	if (TeamADruidMeshFinder.Succeeded())
	{
		TeamAMeshes.Add(EVocation::Druid, TeamADruidMeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamASorcererMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/WizardSM"));
	if (TeamASorcererMeshFinder.Succeeded())
	{
		TeamAMeshes.Add(EVocation::Sorcerer, TeamASorcererMeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamAKnightMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/KnightSK"));
	if (TeamAKnightMeshFinder.Succeeded())
	{
		TeamAMeshes.Add(EVocation::Knight, TeamAKnightMeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamAPaladinMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/TemplarSK"));
	if (TeamAPaladinMeshFinder.Succeeded())
	{
		TeamAMeshes.Add(EVocation::Paladin, TeamAPaladinMeshFinder.Object);
	}

	// Team B Meshes
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamBDruidMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/BattleMageSK"));
	if (TeamBDruidMeshFinder.Succeeded())
	{
		TeamBMeshes.Add(EVocation::Druid, TeamBDruidMeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamBSorcererMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/MageSK"));
	if (TeamBSorcererMeshFinder.Succeeded())
	{
		TeamBMeshes.Add(EVocation::Sorcerer, TeamBSorcererMeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamBPaladinMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/PrinceSK"));
	if (TeamBPaladinMeshFinder.Succeeded())
	{
		TeamBMeshes.Add(EVocation::Paladin, TeamBPaladinMeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TeamBKnightMeshFinder(TEXT("/Game/Characters/ModularCharacterMeshes/Meshes/OneMeshCharacters/SoldierSK"));
	if (TeamBKnightMeshFinder.Succeeded())
	{
		TeamBMeshes.Add(EVocation::Knight, TeamBKnightMeshFinder.Object);
	}
}




void AFibulaCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->bShowMouseCursor = true;
		UpdateAllStatusIcons();
	}
}

void AFibulaCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFibulaCharacter::Move);

		
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFibulaCharacter::Look);

		EnhancedInputComponent->BindAction(ChatAction, ETriggerEvent::Started, this, &AFibulaCharacter::HandleChatInput);
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Triggered, this, &AFibulaCharacter::HandleTargeting);
		EnhancedInputComponent->BindAction(EscapeAction, ETriggerEvent::Started, this, &AFibulaCharacter::HandleEscapeInput);
		PlayerInputComponent->BindKey("I", IE_Pressed, this, &AFibulaCharacter::ToggleInventory);
		PlayerInputComponent->BindKey("P", IE_Pressed, this, &AFibulaCharacter::ToggleScoreboard);
		PlayerInputComponent->BindAxisKey(EKeys::MouseWheelAxis, this, &AFibulaCharacter::OnMouseWheel);
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AFibulaCharacter::OnLeftMousePressed);
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AFibulaCharacter::OnLeftMouseReleased);
		PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AFibulaCharacter::OnRightMousePressed);
		PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AFibulaCharacter::OnRightMouseReleased);

		
		for (int32 i = 1; i <= 10; i++)
		{
			FString KeyName;
			switch (i)
			{
			case 1:
				KeyName = TEXT("One");
				break;
			case 2:
				KeyName = TEXT("Two");
				break;
			case 3:
				KeyName = TEXT("Three");
				break;
			case 4:
				KeyName = TEXT("Four");
				break;
			case 5:
				KeyName = TEXT("Five");
				break;
			case 6:
				KeyName = TEXT("Six");
				break;
			case 7:
				KeyName = TEXT("Seven");
				break;
			case 8:
				KeyName = TEXT("Eight");
				break;
			case 9:
				KeyName = TEXT("Nine");
				break;
			case 10:
				KeyName = TEXT("Zero");
				break;
			}

			FKey Hotkey(*KeyName);
			if (Hotkey.IsValid())
			{
				PlayerInputComponent->BindKey(Hotkey, IE_Pressed, this, &AFibulaCharacter::OnHotkeyPressed);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to create valid hotkey for slot %d"), i);
			}
		}

		
		const FString SecondRowKeyNames[] = {
			TEXT("Q"), TEXT("Z"), TEXT("E"), TEXT("C"), TEXT("R"),
			TEXT("F"), TEXT("V"), TEXT("T"), TEXT("G"), TEXT("B")};

		for (const FString &KeyName : SecondRowKeyNames)
		{
			FKey Hotkey(*KeyName);
			if (Hotkey.IsValid())
			{
				PlayerInputComponent->BindKey(Hotkey, IE_Pressed, this, &AFibulaCharacter::OnHotkeyPressed);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to create valid hotkey for key %s"), *KeyName);
			}
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFibulaCharacter::Move(const FInputActionValue &Value)
{
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFibulaCharacter::Look(const FInputActionValue &Value)
{
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFibulaCharacter::HandleChatInput(const FInputActionValue &Value)
{
	if (APlayerController *PC = Cast<APlayerController>(Controller))
	{
		if (AFibulaHUD *FibulaHUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			if (UStatusWidget *Status = FibulaHUD->GetStatusWidget())
			{
				if (!Status->IsChatFocused())
				{
					Status->FocusChat();
				}
			}
		}
	}
}

void AFibulaCharacter::HandleTargeting()
{
	if (APlayerController *PC = Cast<APlayerController>(Controller))
	{
		FVector WorldLocation, WorldDirection;
		if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			
			FHitResult DirectHit;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;

			
			FCollisionObjectQueryParams ObjectParams;
			ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

			
			const FVector TraceStart = PC->PlayerCameraManager->GetCameraLocation();
			const FVector TraceEnd = TraceStart + (WorldDirection * MAX_TARGET_DISTANCE);

			bool bHitCharacter = GetWorld()->LineTraceSingleByObjectType(
				DirectHit,
				TraceStart,
				TraceEnd,
				ObjectParams,
				QueryParams);

			AFibulaCharacter *DirectTargetCharacter = nullptr;
			if (bHitCharacter)
			{
				DirectTargetCharacter = Cast<AFibulaCharacter>(DirectHit.GetActor());
			}

			
			FHitResult GroundHit;
			bool bHitGround = GetWorld()->LineTraceSingleByChannel(
				GroundHit,
				TraceStart,
				TraceEnd,
				ECC_Visibility,
				QueryParams);

			if (!bHitGround)
				return;

			const float OverlapRadius = 200.0f;

			TArray<AActor *> OverlappingActors;
			UKismetSystemLibrary::SphereOverlapActors(
				GetWorld(),
				GroundHit.Location,
				OverlapRadius,
				TArray<TEnumAsByte<EObjectTypeQuery>>(),
				AActor::StaticClass(),
				TArray<AActor *>{this},
				OverlappingActors);

			AFibulaCharacter *ClosestCharacter = nullptr;
			AItemContainer *ClosestContainer = nullptr;
			float ClosestDistance = MAX_TARGET_DISTANCE;

			for (AActor *Actor : OverlappingActors)
			{
				if (Actor == this)
					continue;

				float Distance = FVector::Distance(GetActorLocation(), Actor->GetActorLocation());

				if (AFibulaCharacter *PotentialCharacter = Cast<AFibulaCharacter>(Actor))
				{
					if (Distance < ClosestDistance)
					{
						ClosestDistance = Distance;
						ClosestCharacter = PotentialCharacter;
					}
				}
				else if (AItemContainer *PotentialContainer = Cast<AItemContainer>(Actor))
				{
					if (Distance < ClosestDistance)
					{
						ClosestDistance = Distance;
						ClosestContainer = PotentialContainer;
					}
				}
			}

			
			if (DirectTargetCharacter)
			{
				float Distance = FVector::Distance(GetActorLocation(), DirectTargetCharacter->GetActorLocation());
				HandleCharacterTargeting(DirectTargetCharacter, Distance);
			}
			
			else if (ClosestCharacter)
			{
				HandleCharacterTargeting(ClosestCharacter, ClosestDistance);
			}

			
			if (ClosestContainer)
			{
				HandleContainerTargeting(ClosestContainer, ClosestDistance);
			}
		}
	}
}

void AFibulaCharacter::HandleCharacterTargeting(AFibulaCharacter *TargetCharacter, float Distance)
{
	if (IsInProtectionZone())
	{
		return;
	}

	if (!TargetCharacter || Distance > MAX_TARGET_DISTANCE)
	{
		ClearTarget();
		return;
	}

	if (TargetCharacter->IsInProtectionZone())
	{
		ClearTarget();
		return;
	}

	if (TargetCharacter == HealingTarget)
	{
		ClearHealingTarget();
		return;
	}

	if (TargetCharacter == CurrentTarget)
	{
		ClearTarget();
		return;
	}

	if (!bIsInTeamBattle)
	{
		SetTarget(TargetCharacter);
		return;
	}

	if (IsOpponentOf(TargetCharacter))
	{
		SetTarget(TargetCharacter);
	}
	else
	{
		SetHealingTarget(TargetCharacter);
	}
}

void AFibulaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFibulaCharacter, Experience);
	DOREPLIFETIME(AFibulaCharacter, CharacterLevel);
	DOREPLIFETIME(AFibulaCharacter, CurrentHealth);
	DOREPLIFETIME(AFibulaCharacter, MaxHealth);
	DOREPLIFETIME(AFibulaCharacter, CurrentMana);
	DOREPLIFETIME(AFibulaCharacter, MaxMana);
	DOREPLIFETIME(AFibulaCharacter, MagicLevel);
	DOREPLIFETIME(AFibulaCharacter, MeleeSkill);
	DOREPLIFETIME(AFibulaCharacter, DistanceSkill);
	DOREPLIFETIME(AFibulaCharacter, ShieldingSkill);
	DOREPLIFETIME(AFibulaCharacter, Vocation);
	DOREPLIFETIME(AFibulaCharacter, CurrentCapacity);
	DOREPLIFETIME(AFibulaCharacter, MaxCapacity);
	DOREPLIFETIME(AFibulaCharacter, Speed);
	DOREPLIFETIME(AFibulaCharacter, CharacterName);
	DOREPLIFETIME(AFibulaCharacter, bIsGenerallyExhausted);
	DOREPLIFETIME(AFibulaCharacter, bIsOffensivelyExhausted);
	DOREPLIFETIME(AFibulaCharacter, CurrentTarget);
	DOREPLIFETIME(AFibulaCharacter, Inventory);
	DOREPLIFETIME(AFibulaCharacter, bHasMagicShield);
	DOREPLIFETIME(AFibulaCharacter, LastCombatTime);
	DOREPLIFETIME(AFibulaCharacter, bIsInProtectionZone);
	DOREPLIFETIME(AFibulaCharacter, TeamId);
	DOREPLIFETIME(AFibulaCharacter, bIsStrengthened);
	DOREPLIFETIME(AFibulaCharacter, Equipment);
	DOREPLIFETIME(AFibulaCharacter, EquippedHelm);
	DOREPLIFETIME(AFibulaCharacter, EquippedArmor);
	DOREPLIFETIME(AFibulaCharacter, EquippedBag);
	DOREPLIFETIME(AFibulaCharacter, EquippedWeapon);
	DOREPLIFETIME(AFibulaCharacter, EquippedShield);
	DOREPLIFETIME(AFibulaCharacter, EquippedLegs);
	DOREPLIFETIME(AFibulaCharacter, EquippedBoots);
	DOREPLIFETIME(AFibulaCharacter, EquippedAmulet);
	DOREPLIFETIME(AFibulaCharacter, EquippedRing);
	DOREPLIFETIME(AFibulaCharacter, EquippedAmmunition);
	DOREPLIFETIME(AFibulaCharacter, HealingTarget);
	DOREPLIFETIME(AFibulaCharacter, bIsInTeamBattle);
	DOREPLIFETIME(AFibulaCharacter, KillCount);
	DOREPLIFETIME(AFibulaCharacter, DeathCount);
	DOREPLIFETIME(AFibulaCharacter, DamageReceivedMultiplier);
	DOREPLIFETIME(AFibulaCharacter, MeleeSkillMultiplier);
	DOREPLIFETIME(AFibulaCharacter, DistanceSkillMultiplier);
	DOREPLIFETIME(AFibulaCharacter, MagicLevelMultiplier);
	DOREPLIFETIME(AFibulaCharacter, bHasLight);
}

void AFibulaCharacter::GainExperience(int64 Amount)
{
	if (HasAuthority())
	{
		
		if (CharacterLevel >= 150)
		{
			float ReductionFactor = 1.0f - FMath::Min(((CharacterLevel - 150) * 0.02f), 0.999f);
			Amount = static_cast<int64>(Amount * ReductionFactor);
		}

		Experience += Amount;
		int32 NewLevel = GameFormulas::CalculateLevel(Experience);

		if (NewLevel != CharacterLevel)
		{
			HandleLevelChange(CharacterLevel, NewLevel);
			CharacterLevel = NewLevel;
			UpdateAttributes(true);
		}

		TArray<AFibulaCharacter *> NearbyPlayers = UGameplayUtils::GetPlayersWithinRadius(this, GetActorLocation(), 2000.0f);
		for (AFibulaCharacter *Player : NearbyPlayers)
		{
			Player->ClientShowExperienceGain(this, Amount);
		}
	}
}

void AFibulaCharacter::ClientShowExperienceGain_Implementation(AFibulaCharacter *Earner, int64 Amount)
{
	APlayerController *LocalPC = GetWorld()->GetFirstPlayerController();
	if (!LocalPC || !LocalPC->IsLocalController())
	{
		return;
	}

	UDamageDisplayUtils::ShowFloatingNumber(LocalPC, Earner, Amount, EDamageDisplayType::Experience);
}

void AFibulaCharacter::UpdateAttributes(bool bIsLevelUp)
{
	if (HasAuthority())
	{
		MaxHealth = GameFormulas::CalculateMaxHealth(Vocation, CharacterLevel);
		MaxMana = GameFormulas::CalculateMaxMana(Vocation, CharacterLevel);
		MaxCapacity = GameFormulas::CalculateMaxCapacity(Vocation, CharacterLevel);
		SetSpeed(GameFormulas::CalculateSpeed(CharacterLevel, Vocation));

		if (!bIsLevelUp)
		{
			CurrentHealth = MaxHealth;
			CurrentMana = MaxMana;
		}
	}
}

void AFibulaCharacter::OnRep_Experience()
{
	
}

void AFibulaCharacter::OnRep_CharacterLevel()
{
	
}

void AFibulaCharacter::OnRep_Health()
{
	UpdateHealthBar();
	UpdateStatusWidget();
}

void AFibulaCharacter::OnRep_Mana()
{
	UpdateStatusWidget();
}

void AFibulaCharacter::OnRep_Vocation()
{
	UpdateAttributes();
	

	if (!IsLocallyControlled())
		return;

	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
			{
				StatusWidget->SetupActionBar();
			}
		}
	}
}

void AFibulaCharacter::ServerBroadcastMessage_Implementation(const FString &SenderName, const FString &Message)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AFibulaGameState *GameState = GetWorld()->GetGameState<AFibulaGameState>())
	{
		if (AChatSystem *ChatSystem = GameState->GetChatSystem())
		{
			ChatSystem->MulticastBroadcastMessage(SenderName, Message);
		}

		if (ASpellSystem *SpellSystem = GameState->GetSpellSystem())
		{
			const FSpellDefinition *Spell = USpellDatabase::GetSpellByWords(Message);
			if (Spell)
			{
				SpellSystem->ServerTryExecuteSpell(this, Spell->Name);
			}
		}
	}
}

void AFibulaCharacter::OnRep_Speed()
{
	if (UCharacterMovementComponent *MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Speed;
		UpdateAllStatusIcons();
	}
}

void AFibulaCharacter::OnRep_CurrentTarget()
{
	if (!IsLocallyControlled())
		return;

	UpdateTargetDecal();
}

void AFibulaCharacter::UpdateHealthBar()
{
	if (NameTagWidget)
	{
		float HealthPercentage = MaxHealth > 0 ? static_cast<float>(CurrentHealth) / MaxHealth : 0.0f;
		NameTagWidget->UpdateHealth(HealthPercentage);
	}
}

void AFibulaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		UpdateCurrentCapacity();
	}
}

void AFibulaCharacter::ServerModifyHealth_Implementation(int32 Amount, AFibulaCharacter *DamageDealer, EDamageDisplayType DisplayType)
{
	if (!HasAuthority())
		return;

	if (IsInProtectionZone())
	{
		return;
	}

	
	if (AFibulaGameMode *GameMode = Cast<AFibulaGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (Amount < 0 && DamageDealer && !GameMode->CanDamage(DamageDealer, this))
			return;
		if (Amount > 0 && DamageDealer && !GameMode->CanHeal(DamageDealer, this))
			return;
	}

	
	if (Amount >= 0)
	{
		int32 NewHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
		CurrentHealth = NewHealth;
		UpdateHealthBar();
		UpdateStatusWidget();
		return;
	}

	
	int32 DamageAmount = FMath::Abs(Amount);
	if (DamageDealer)
	{
		DamageDealer->UpdateLastCombatTime();
		LastDamageDealer = DamageDealer;
	}

	
	if (DamageAmount > 0 && DamageReceivedMultiplier > 1.0f)
	{
		DamageAmount = FMath::FloorToInt(DamageAmount * DamageReceivedMultiplier);
	}

	if (DamageAmount > 0 && !EquippedAmulet.Name.IsEmpty() &&
		(EquippedAmulet.EquipmentAttributes.DamageReductionPercent > 0.0f))
	{
		
		DamageAmount = FMath::FloorToInt(DamageAmount * (1.0f - EquippedAmulet.EquipmentAttributes.DamageReductionPercent));

		
		if (EquippedAmulet.EquipmentAttributes.Charges > 0)
		{
			EquippedAmulet.EquipmentAttributes.Charges--;

			
			if (EquippedAmulet.EquipmentAttributes.Charges <= 0)
			{
				FGameItem EmptyItem;
				EquippedAmulet = EmptyItem;
				UpdateCurrentCapacity();
			}
		}
	}

	if (bHasMagicShield && CurrentMana > 0)
	{
		
		if (CurrentMana >= DamageAmount)
		{
			ModifyMana(-DamageAmount);
			ShowDamageToClosestPlayers(this, DamageAmount, EDamageDisplayType::MagicShield);
			return;
		}
		else
		{
			
			int32 ManaAmount = CurrentMana;
			DamageAmount -= ManaAmount;

			
			ShowDamageToClosestPlayers(this, ManaAmount, EDamageDisplayType::MagicShield);

			
			ModifyMana(-CurrentMana);
		}
	}

	
	int32 NewHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0, MaxHealth);

	
	if (DamageAmount > 0 && DamageDealer && DamageDealer != this)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		FDamageDealer &DealerInfo = DamageDealers.FindOrAdd(DamageDealer);
		DealerInfo.Timestamp = CurrentTime;
		DealerInfo.DamageDealt += DamageAmount;
	}

	CurrentHealth = NewHealth;
	UpdateHealthBar();
	UpdateStatusWidget();

	
	ShowDamageToClosestPlayers(this, DamageAmount, DisplayType);

	
	if (CurrentHealth <= 0)
	{
		HandleDeath();
	}
}

void AFibulaCharacter::ClientShowExhaustedMessage_Implementation()
{
	ShowCenteredMessage(TEXT("You are exhausted."), 1.0f, EFloatingTextType::MinorInfo);
}

void AFibulaCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (UWorld *World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GeneralExhaustTimer);
		World->GetTimerManager().ClearTimer(OffensiveExhaustTimer);
	}
}

void AFibulaCharacter::SetGeneralExhaust(bool bExhausted)
{
	if (HasAuthority())
	{
		bIsGenerallyExhausted = bExhausted;

		if (bExhausted)
		{
			UWorld *World = GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(
					GeneralExhaustTimer,
					[this]()
					{ SetGeneralExhaust(false); },
					1.0f,
					false);
			}
		}
	}
}

void AFibulaCharacter::SetOffensiveExhaust(bool bExhausted)
{
	if (HasAuthority())
	{
		bIsOffensivelyExhausted = bExhausted;

		if (bExhausted)
		{
			UWorld *World = GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(
					OffensiveExhaustTimer,
					[this]()
					{ SetOffensiveExhaust(false); },
					2.0f,
					false);
			}
		}
	}
}

void AFibulaCharacter::ServerSetTarget_Implementation(AFibulaCharacter *NewTarget)
{
	if (CurrentTarget != NewTarget)
	{
		StopAutoAttack();
		CurrentTarget = NewTarget;

		
		if (CurrentTarget && IsValid(CurrentTarget) &&
			(Vocation == EVocation::Knight || Vocation == EVocation::Paladin))
		{
			
			float CurrentTime = GetWorld()->GetTimeSeconds();
			if (CurrentTime - LastAutoAttackTime >= AUTO_ATTACK_INTERVAL)
			{
				StartAutoAttack();
				LastAutoAttackTime = CurrentTime;
			}
			else
			{
				
				GetWorld()->GetTimerManager().SetTimer(
					AutoAttackTimer,
					this,
					&AFibulaCharacter::StartAutoAttack,
					AUTO_ATTACK_INTERVAL - (CurrentTime - LastAutoAttackTime),
					false);
			}
		}
	}
}

void AFibulaCharacter::ServerSetHealingTarget_Implementation(AFibulaCharacter *NewTarget)
{
	if (!HasAuthority())
		return;

	HealingTarget = NewTarget;
}

void AFibulaCharacter::ClientTargetingFailed_Implementation()
{
	CurrentTarget = nullptr;
	UpdateTargetDecal();
}

void AFibulaCharacter::UpdateTargetDecal()
{
	
	if (!IsLocallyControlled())
	{
		return;
	}

	
	if (CurrentTarget && IsValid(CurrentTarget))
	{
		if (!TargetNiagaraComponent)
		{
			TargetNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				TargetNiagaraSystem,
				CurrentTarget->GetRootComponent(),
				NAME_None,
				FVector(0, 0, 0),
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true);
		}
		else
		{
			TargetNiagaraComponent->SetAsset(TargetNiagaraSystem);
			TargetNiagaraComponent->Activate();
			TargetNiagaraComponent->AttachToComponent(
				CurrentTarget->GetRootComponent(),
				FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else if (TargetNiagaraComponent)
	{
		TargetNiagaraComponent->Deactivate();
		TargetNiagaraComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}

	
	if (HealingTarget && IsValid(HealingTarget))
	{
		if (!HealingTargetNiagaraComponent)
		{
			HealingTargetNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				HealingTargetNiagaraSystem,
				HealingTarget->GetRootComponent(),
				NAME_None,
				FVector(0, 0, 0),
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true);
		}
		else
		{
			HealingTargetNiagaraComponent->SetAsset(HealingTargetNiagaraSystem);
			HealingTargetNiagaraComponent->Activate();
			HealingTargetNiagaraComponent->AttachToComponent(
				HealingTarget->GetRootComponent(),
				FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else if (HealingTargetNiagaraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deactivating healing target effect"));
		HealingTargetNiagaraComponent->Deactivate();
		HealingTargetNiagaraComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}

void AFibulaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() && NameTagWidget && NameTagWidget->IsVisible())
	{
		NameTagWidget->RotateTowardsCamera();
	}

	if (!HasAuthority() && bLeftMousePressed && bRightMousePressed && Controller != nullptr)
	{
		WalkTowardsCamera();
	}
}

void AFibulaCharacter::WalkTowardsCamera()
{
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	
	AddMovementInput(ForwardDirection, 1.0f);
}

void AFibulaCharacter::CheckTargetDistance()
{
	if (!HasAuthority() || !CurrentTarget || !IsValid(CurrentTarget))
		return;

	float Distance = FVector::Distance(GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Distance > MAX_TARGET_DISTANCE)
	{
		CurrentTarget = nullptr;
		UpdateTargetDecal();
		ClientTargetingFailed();
	}
}

void AFibulaCharacter::BeginPlay()
{
	Super::BeginPlay();

	LastAutoAttackTime = -AUTO_ATTACK_INTERVAL; 

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			TargetCheckTimer,
			this,
			&AFibulaCharacter::CheckTargetDistance,
			TARGET_CHECK_INTERVAL,
			true);
		SetInProtectionZone(true);
	}
}

void AFibulaCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearAsTargetOnNearbyPlayers();
}

float AFibulaCharacter::GetAttackRange() const
{
	switch (Vocation)
	{
	case EVocation::Knight:
		return KNIGHT_ATTACK_RANGE;
	case EVocation::Paladin:
		return PALADIN_ATTACK_RANGE;
	case EVocation::Sorcerer:
	case EVocation::Druid:
		return MAGE_ATTACK_RANGE;
	default:
		return 0.0f;
	}
}

void AFibulaCharacter::StartAutoAttack()
{
	if (!HasAuthority() || !CurrentTarget || !IsValid(CurrentTarget))
		return;

	
	if (Vocation != EVocation::Knight && Vocation != EVocation::Paladin)
		return;

	LastAutoAttackTime = GetWorld()->GetTimeSeconds();
	GetWorld()->GetTimerManager().SetTimer(
		AutoAttackTimer,
		this,
		&AFibulaCharacter::PerformAutoAttack,
		AUTO_ATTACK_INTERVAL,
		true);

	
	PerformAutoAttack();
}

void AFibulaCharacter::StopAutoAttack()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoAttackTimer);
	}
}

void AFibulaCharacter::PerformAutoAttack()
{
	if (!HasAuthority() || !CurrentTarget || !IsValid(CurrentTarget))
	{
		StopAutoAttack();
		return;
	}

	if (IsInProtectionZone() || CurrentTarget->IsInProtectionZone())
	{
		StopAutoAttack();
		ClearTarget();
		return;
	}

	
	if (Vocation == EVocation::Paladin)
	{
		
		if (EquippedWeapon.Name.IsEmpty() || !EquippedWeapon.EquipmentAttributes.bCanFireProjectiles)
		{
			ShowCenteredMessage(TEXT("You need a ranged weapon to perform this action."), 1.0f, EFloatingTextType::MinorInfo);
			return;
		}

		
		if (EquippedAmmunition.Name.IsEmpty() || EquippedAmmunition.StackCount <= 0)
		{
			ShowCenteredMessage(TEXT("You need ammunition to perform this action."), 1.0f, EFloatingTextType::MinorInfo);
			return;
		}
	}

	float Distance = FVector::Distance(GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Distance <= GetAttackRange())
	{
		ServerPerformAutoAttack(CurrentTarget);
	}
}

void AFibulaCharacter::ServerPerformAutoAttack_Implementation(AFibulaCharacter *Target)
{
	if (!Target || !IsValid(Target))
		return;

	int32 Damage = 0;
	switch (Vocation)
	{
	case EVocation::Knight:
		Damage = GameFormulas::CalculateAutoAttackDamage(this, Target);
		
		{
			TArray<AFibulaCharacter *> NearbyPlayers = UGameplayUtils::GetPlayersWithinRadius(this, Target->GetActorLocation(), 2500.0f);
			for (AFibulaCharacter *Player : NearbyPlayers)
			{
				Player->ClientSpawnAutoAttackParticle(this, Target);
			}
		}
		break;
	case EVocation::Paladin:
	{
		
		if (EquippedAmmunition.Name.IsEmpty() || EquippedAmmunition.StackCount <= 0)
		{
			StopAutoAttack();
			return;
		}

		
		EquippedAmmunition.StackCount--;
		if (EquippedAmmunition.StackCount <= 0)
		{
			EquippedAmmunition = FGameItem();
		}

		
		{
			TArray<AFibulaCharacter *> NearbyPlayers = UGameplayUtils::GetPlayersWithinRadius(this, Target->GetActorLocation(), 2500.0f);
			for (AFibulaCharacter *Player : NearbyPlayers)
			{
				Player->ClientSpawnAutoAttackParticle(this, Target);
			}
		}

		Damage = GameFormulas::CalculateAutoAttackDamage(this, Target);
		break;
	}
	case EVocation::Sorcerer:
	case EVocation::Druid:
		return;
	}

	Target->ServerModifyHealth(-Damage, this);
}

void AFibulaCharacter::ClientSpawnSpellEffects_Implementation(const FVector &Center, AFibulaCharacter *Caster, const FString &SpellWords)
{
	if (HasAuthority())
		return;

	const FSpellDefinition *Spell = USpellDatabase::GetSpell(SpellWords);
	if (Spell)
	{
		const FRotator Rotation = Caster->GetActorRotation();
		
		USpellEffectDisplay::SpawnSpellEffects(GetWorld(), Caster, Center, Rotation, Spell);

		
		if (Spell->SpellSound)
		{
			USoundSettingsManager *Settings = USoundSettingsManager::LoadSettings();
			if (!Settings)
				return;

			float FinalVolume = Settings->MasterVolume * Settings->SoundEffectsVolume;
			UGameplayStatics::PlaySoundAtLocation(
				this,
				Spell->SpellSound,
				Center,
				FinalVolume);
		}
	}
}

void AFibulaCharacter::PlayAutoAttackSound(AFibulaCharacter *Caster, AFibulaCharacter *Target)
{
	
	USoundSettingsManager *Settings = USoundSettingsManager::LoadSettings();
	if (!Settings)
		return;

	float FinalVolume = Settings->MasterVolume * Settings->SoundEffectsVolume;

	
	if (Caster->GetVocation() == EVocation::Paladin && BowAndArrowSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BowAndArrowSound,
			Caster->GetActorLocation(),
			FinalVolume);
	}
	else if (Caster->GetVocation() == EVocation::Knight && SwordHitSounds.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, SwordHitSounds.Num() - 1);
		UGameplayStatics::PlaySoundAtLocation(
			this,
			SwordHitSounds[RandomIndex],
			Target->GetActorLocation(),
			FinalVolume);
	}
}

void AFibulaCharacter::ClientSpawnAutoAttackParticle_Implementation(AFibulaCharacter *Caster, AFibulaCharacter *Target)
{
	if (HasAuthority())
		return;

	PlayAutoAttackSound(Caster, Target);
	if (Caster->GetVocation() != EVocation::Paladin)
		return;

	
	UNiagaraSystem *ProjectileEffect = nullptr;
	if (!Caster->GetEquippedAmmunition().Name.IsEmpty())
	{
		ProjectileEffect = Caster->GetEquippedAmmunition().EquipmentAttributes.ProjectileEffect;
	}

	
	if (!ProjectileEffect)
	{
		ProjectileEffect = AutoAttackEffect;
	}

	if (!ProjectileEffect || !Target)
		return;

	
	FVector StartLocation = Caster->GetActorLocation() + FVector(0, 0, 60.0f);
	FVector EndLocation = Target->GetActorLocation() + FVector(0, 0, 60.0f);

	FVector Direction = EndLocation - StartLocation;
	Direction.Normalize();

	FRotator ProjectileRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASpellProjectileAnimationActor *Projectile = GetWorld()->SpawnActor<ASpellProjectileAnimationActor>(
		ASpellProjectileAnimationActor::StaticClass(),
		StartLocation,
		ProjectileRotation,
		SpawnParams);

	if (Projectile)
	{
		Projectile->VisualEffect->SetAsset(ProjectileEffect);
		const float ProjectileSpeed = 4000.0f;
		Projectile->SetHomingTarget(Target);
		Projectile->InitializeProjectile(StartLocation, EndLocation, ProjectileSpeed);
	}
}

void AFibulaCharacter::ClientShowDamage_Implementation(AFibulaCharacter *Target, int32 Damage, EDamageDisplayType DisplayType)
{
	APlayerController *LocalPC = GetWorld()->GetFirstPlayerController();
	if (!LocalPC || !LocalPC->IsLocalController())
	{
		return;
	}

	UDamageDisplayUtils::ShowFloatingNumber(LocalPC, Target, Damage, DisplayType);
}

void AFibulaCharacter::SetTarget(AFibulaCharacter *NewTarget)
{
	
	if (IsLocallyControlled())
	{
		CurrentTarget = NewTarget;
		UpdateTargetDecal();
	}
	ServerSetTarget(CurrentTarget);
}

void AFibulaCharacter::SetHealingTarget(AFibulaCharacter *NewTarget)
{
	if (IsLocallyControlled())
	{
		HealingTarget = NewTarget;
		UpdateTargetDecal();
	}
	ServerSetHealingTarget(NewTarget);
}

void AFibulaCharacter::ClearTarget()
{
	
	if (IsLocallyControlled())
	{
		CurrentTarget = nullptr;
		UpdateTargetDecal();
	}
	ServerSetTarget(nullptr);
}

void AFibulaCharacter::ClearHealingTarget()
{
	if (IsLocallyControlled())
	{
		HealingTarget = nullptr;
		UpdateTargetDecal();
	}

	ServerSetHealingTarget(nullptr);
}

void AFibulaCharacter::HandleDeath()
{
	if (!HasAuthority())
	{
		return;
	}

	
	IncrementDeaths();

	
	FVector DeathLocation = GetActorLocation();
	FRotator DeathRotation = FRotator(-90.0f, GetActorRotation().Yaw, 0.0f);

	
	if (CharacterLevel > 90)
	{
		int64 ExperienceLoss = Experience * 0.05;
		Experience = FMath::Max(0ll, Experience - ExperienceLoss);

		
		float CurrentTime = GetWorld()->GetTimeSeconds();
		int64 TotalExperienceGain = ExperienceLoss * 2;
		int32 TotalDamage = 0;

		
		TArray<AFibulaCharacter *> ExpiredDealers;
		for (auto &Pair : DamageDealers)
		{
			if (CurrentTime - Pair.Value.Timestamp > DAMAGE_TRACKING_DURATION)
			{
				ExpiredDealers.Add(Pair.Key);
			}
			else
			{
				TotalDamage += Pair.Value.DamageDealt;
			}
		}

		
		for (AFibulaCharacter *ExpiredDealer : ExpiredDealers)
		{
			DamageDealers.Remove(ExpiredDealer);
		}

		
		for (const auto &Pair : DamageDealers)
		{
			AFibulaCharacter *Dealer = Pair.Key;
			if (IsValid(Dealer) && Dealer != this)
			{
				
				float LevelRatio = static_cast<float>(CharacterLevel) / Dealer->CharacterLevel;
				if (LevelRatio >= 0.75f)
				{
					
					float DamageRatio = static_cast<float>(Pair.Value.DamageDealt) / TotalDamage;
					int64 ExperienceShare = static_cast<int64>(TotalExperienceGain * DamageRatio);
					Dealer->GainExperience(ExperienceShare);
				}
			}
		}

		
		DamageDealers.Empty();

		
		int32 NewLevel = GameFormulas::CalculateLevel(Experience);
		if (NewLevel != CharacterLevel)
		{
			CharacterLevel = NewLevel;
			UpdateAttributes(true);
		}
	}
	
	if (AFibulaGameMode *GameMode = GetWorld()->GetAuthGameMode<AFibulaGameMode>())
	{
		if (GameMode->GetCurrentGameMode() == EGameModeType::TeamBattle)
		{
			AFibulaTeamBattleGameMode *TeamBattleMode = Cast<AFibulaTeamBattleGameMode>(GameMode);
			if (TeamBattleMode)
			{
				TeamBattleMode->OnPlayerDeath(this, LastDamageDealer);
			}
		}
		else
		{
			GameMode->OnPlayerDeath(this, LastDamageDealer);
		}
	}

	
	ClearTarget();
	ClearHealingTarget();
	StopAutoAttack();
	HandleDeathItems(DeathLocation, DeathRotation);

	
	ShowCenteredMessage(TEXT("You are dead."), 5.0f, EFloatingTextType::MajorInfo);
	
	ClientPinCameraAtDeathLocation(DeathLocation, LastDamageDealer, 5.0f);

	RespawnCharacter();
}

void AFibulaCharacter::RespawnCharacter()
{
	if (!HasAuthority())
	{
		return;
	}

	ClearAsTargetOnNearbyPlayers();

	
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;

	
	if (AFibulaGameMode *GameMode = Cast<AFibulaGameMode>(GetWorld()->GetAuthGameMode()))
	{
		AActor *StartSpot = nullptr;

		
		if (bIsInTeamBattle)
		{
			StartSpot = GameMode->FindTeamPlayerStart(TeamId);
		}

		
		if (!StartSpot)
		{
			StartSpot = GameMode->FindPlayerStart(nullptr);
		}

		if (StartSpot)
		{
			
			FVector SpawnLocation = StartSpot->GetActorLocation();
			SpawnLocation.Z += 50.0f; 

			
			SetActorLocationAndRotation(SpawnLocation, StartSpot->GetActorRotation(), false, nullptr, ETeleportType::TeleportPhysics);
			SetInProtectionZone(true);

			
			if (UCharacterMovementComponent *MovementComp = GetCharacterMovement())
			{
				MovementComp->Velocity = FVector::ZeroVector;
				MovementComp->StopMovementImmediately();
			}
		}
		LastCombatTime = -PROTECTION_ZONE_COMBAT_DELAY; 
	}

	
	InitializeCharacterStats(false);
	UpdateHealthBar();
}

void AFibulaCharacter::HandleEscapeInput(const FInputActionValue &Value)
{
	if (CurrentTarget)
	{
		ClearTarget();
	}
	if (HealingTarget)
	{
		ClearHealingTarget();
	}
}

void AFibulaCharacter::ShowCenteredMessage_Implementation(const FString &Message, float Duration, EFloatingTextType TextType)
{
	if (APlayerController *PC = Cast<APlayerController>(Controller))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			if (TSubclassOf<UFloatingTextWidget> FloatingTextClass = HUD->GetFloatingTextWidgetClass())
			{
				
				EFloatingTextChannel Channel =
					(TextType == EFloatingTextType::MinorInfo) ? EFloatingTextChannel::MinorInfo : EFloatingTextChannel::Alert;

				
				UFloatingTextWidget *FloatingText = UFloatingTextWidget::GetOrCreateWidgetForChannel(
					PC, FloatingTextClass, Channel);

				if (FloatingText)
				{
					FloatingText->SetSystemText(Message, TextType, Duration);
				}
			}
		}
	}
}

void AFibulaCharacter::HandleLevelChange_Implementation(int32 OldLevel, int32 NewLevel)
{
	if (NewLevel > OldLevel)
	{
		ShowCenteredMessage(FString::Printf(TEXT("You advanced from level %d to level %d"), OldLevel, NewLevel), 4.0f, EFloatingTextType::MajorInfo);
	}
	else if (NewLevel < OldLevel)
	{
		const FString DeathMessage = TEXT("You are dead.");
		const FString LevelDownMessage = FString::Printf(TEXT("You were downgraded from level %d to level %d"), OldLevel, NewLevel);

		ShowCenteredMessage(DeathMessage, 3.0f, EFloatingTextType::MajorInfo);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, LevelDownMessage]()
											   { ShowCenteredMessage(LevelDownMessage, 3.0f, EFloatingTextType::MajorInfo); }, 3.0f, false);
	}
}

void AFibulaCharacter::HandleSkillGain_Implementation(const FString &SkillName, int32 NewLevel)
{
	ShowCenteredMessage(FString::Printf(TEXT("You advanced to %s %d"), *SkillName, NewLevel), 3.0f, EFloatingTextType::MajorInfo);
}

void AFibulaCharacter::ModifyMana(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	int32 NewMana = FMath::Clamp(CurrentMana + Amount, 0, MaxMana);
	if (NewMana != CurrentMana)
	{
		CurrentMana = NewMana;
	}
}

void AFibulaCharacter::UpdateStatusWidget()
{
	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
			{
				StatusWidget->UpdateHealth(CurrentHealth, MaxHealth);
				StatusWidget->UpdateMana(CurrentMana, MaxMana);
				StatusWidget->UpdateLevel(CharacterLevel);
				StatusWidget->UpdateCapacity(CurrentCapacity, GetMaxCapacity());

				
				int64 CurrentLevelExp = GameFormulas::CalculateExperienceForLevel(CharacterLevel);
				int64 NextLevelExp = GameFormulas::CalculateExperienceForLevel(CharacterLevel + 1);
				int64 ExpForCurrentLevel = Experience - CurrentLevelExp;
				int64 ExpNeededForNextLevel = NextLevelExp - CurrentLevelExp;
				StatusWidget->UpdateExperience(static_cast<float>(ExpForCurrentLevel), static_cast<float>(ExpNeededForNextLevel));
			}
		}
	}
}

void AFibulaCharacter::InitializeCharacterStats(bool bResetLevelAndExperience)
{
	if (!HasAuthority())
		return;

	
	MagicLevel = UPlayerStartingElements::GetStartingMagicLevel(Vocation);
	MeleeSkill = UPlayerStartingElements::GetStartingMeleeSkill(Vocation);
	DistanceSkill = UPlayerStartingElements::GetStartingDistanceSkill(Vocation);
	ShieldingSkill = UPlayerStartingElements::GetStartingShieldingSkill(Vocation);

	
	if (bResetLevelAndExperience)
	{
		CharacterLevel = UPlayerStartingElements::GetStartingLevel();
		Experience = GameFormulas::CalculateExperienceForLevel(CharacterLevel);
	}

	
	MaxHealth = GameFormulas::CalculateMaxHealth(Vocation, CharacterLevel);
	MaxMana = GameFormulas::CalculateMaxMana(Vocation, CharacterLevel);
	MaxCapacity = GameFormulas::CalculateMaxCapacity(Vocation, CharacterLevel);
	Speed = GameFormulas::CalculateSpeed(CharacterLevel, Vocation);

	
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	CurrentCapacity = 0; 

	
	if (UCharacterMovementComponent *MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Speed;
	}

	
	TArray<FGameItem> StartingItems = UPlayerStartingElements::GetStartingItems(Vocation);
	for (const FGameItem &DesiredItem : StartingItems)
	{
		
		int32 CurrentCount = GetItemCount(DesiredItem.Name);
		int32 DesiredCount = DesiredItem.StackCount;

		
		if (CurrentCount < DesiredCount)
		{
			FGameItem TopUpItem = DesiredItem;
			TopUpItem.StackCount = DesiredCount - CurrentCount;

			
			AddItem(TopUpItem);
		}
	}

	
	if (Vocation == EVocation::Paladin)
	{
		
		bool hasElvishBow = false;
		if (!EquippedWeapon.Name.IsEmpty() && EquippedWeapon.Name == TEXT("Elvish Bow"))
		{
			hasElvishBow = true;
		}
		else
		{
			
			for (const FGameItem &Item : Inventory)
			{
				if (Item.Name == TEXT("Elvish Bow"))
				{
					hasElvishBow = true;
					break;
				}
			}
		}

		
		if (!hasElvishBow)
		{
			if (const FGameItem *ElvishBow = UItemDatabase::GetItem(TEXT("Elvish Bow")))
			{
				AddItem(*ElvishBow);
			}
		}

		
		int32 CurrentArrows = 0;
		if (!EquippedAmmunition.Name.IsEmpty() && EquippedAmmunition.Name == TEXT("Arrow"))
		{
			CurrentArrows += EquippedAmmunition.StackCount;
		}

		for (const FGameItem &Item : Inventory)
		{
			if (Item.Name == TEXT("Arrow"))
			{
				CurrentArrows += Item.StackCount;
			}
		}

		if (CurrentArrows < 800)
		{
			if (const FGameItem *Arrow = UItemDatabase::GetItem(TEXT("Arrow")))
			{
				FGameItem NewArrows = *Arrow;
				NewArrows.StackCount = 800 - CurrentArrows;
				AddItem(NewArrows);
			}
		}
	}
	else if (Vocation == EVocation::Knight)
	{
		
		bool hasStrongMeleeWeapon = false;
		if (!EquippedWeapon.Name.IsEmpty())
		{
			
			hasStrongMeleeWeapon = !EquippedWeapon.EquipmentAttributes.bCanFireProjectiles &&
								   EquippedWeapon.EquipmentAttributes.Attack >= 35;
		}

		if (!hasStrongMeleeWeapon)
		{
			
			for (const FGameItem &Item : Inventory)
			{
				if (Item.EquipmentAttributes.EquipmentType == EEquipmentType::Weapon &&
					!Item.EquipmentAttributes.bCanFireProjectiles &&
					Item.EquipmentAttributes.Attack >= 35)
				{
					hasStrongMeleeWeapon = true;
					break;
				}
			}

			
			if (!hasStrongMeleeWeapon)
			{
				if (const FGameItem *GiantSword = UItemDatabase::GetItem(TEXT("Giant Sword")))
				{
					AddItem(*GiantSword);
				}
			}
		}
	}
	UpdateCurrentCapacity();
}

bool AFibulaCharacter::AddItem(const FGameItem &Item)
{
	if (!HasAuthority())
		return false;

	
	float NewWeight = Item.Weight * Item.StackCount;
	float CurrentWeight = 0.0f;

	
	for (const FGameItem &ExistingItem : Inventory)
	{
		CurrentWeight += ExistingItem.Weight * ExistingItem.StackCount;
	}

	
	if (CurrentWeight + NewWeight > GetMaxCapacity())
	{
		ShowCenteredMessage(TEXT("You cannot carry any more items."), 3.0f, EFloatingTextType::MinorInfo);
		return false;
	}

	
	if (Item.bIsStackable)
	{
		for (FGameItem &ExistingItem : Inventory)
		{
			if (ExistingItem.Name == Item.Name)
			{
				ExistingItem.StackCount += Item.StackCount;
				UpdateCurrentCapacity();
				return true;
			}
		}
	}

	
	Inventory.Add(Item);
	UpdateCurrentCapacity();
	return true;
}

void AFibulaCharacter::UpdateCurrentCapacity()
{
	if (!HasAuthority())
		return;

	float TotalWeight = 0.0f;

	
	for (const FGameItem &Item : Inventory)
	{
		TotalWeight += Item.Weight * Item.StackCount;
	}

	
	if (!EquippedHelm.Name.IsEmpty())
		TotalWeight += EquippedHelm.Weight;
	if (!EquippedArmor.Name.IsEmpty())
		TotalWeight += EquippedArmor.Weight;
	if (!EquippedBag.Name.IsEmpty())
		TotalWeight += EquippedBag.Weight;
	if (!EquippedWeapon.Name.IsEmpty())
		TotalWeight += EquippedWeapon.Weight;
	if (!EquippedShield.Name.IsEmpty())
		TotalWeight += EquippedShield.Weight;
	if (!EquippedLegs.Name.IsEmpty())
		TotalWeight += EquippedLegs.Weight;
	if (!EquippedBoots.Name.IsEmpty())
		TotalWeight += EquippedBoots.Weight;
	if (!EquippedAmulet.Name.IsEmpty())
		TotalWeight += EquippedAmulet.Weight;
	if (!EquippedRing.Name.IsEmpty())
		TotalWeight += EquippedRing.Weight;
	if (!EquippedAmmunition.Name.IsEmpty())
		TotalWeight += EquippedAmmunition.Weight * EquippedAmmunition.StackCount;

	CurrentCapacity = TotalWeight;
}

bool AFibulaCharacter::RemoveItem(const FString &ItemName, int32 Count)
{
	if (!HasAuthority())
		return false;

	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].Name == ItemName)
		{
			if (Inventory[i].bIsStackable)
			{
				if (Inventory[i].StackCount > Count)
				{
					Inventory[i].StackCount -= Count;
					UpdateCurrentCapacity();
					return true;
				}
				else if (Inventory[i].StackCount == Count)
				{
					Inventory.RemoveAt(i);
					UpdateCurrentCapacity();
					return true;
				}
			}
			else if (Count == 1)
			{
				Inventory.RemoveAt(i);
				UpdateCurrentCapacity();
				return true;
			}
		}
	}
	return false;
}

int32 AFibulaCharacter::GetItemCount(const FString &ItemName) const
{
	for (const FGameItem &Item : Inventory)
	{
		if (Item.Name == ItemName)
		{
			return Item.StackCount;
		}
	}
	return 0;
}

void AFibulaCharacter::OnRep_Inventory()
{
	
	if (IsLocallyControlled())
	{
		if (APlayerController *PC = Cast<APlayerController>(GetController()))
		{
			if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
			{
				if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
				{
					StatusWidget->UpdateInventoryDisplay(Inventory);
				}
			}
		}
	}
}

void AFibulaCharacter::ServerUseItem_Implementation(const FGameItem &Item, const FVector &TargetLocation)
{
	if (!HasAuthority())
		return;

	const FSpellDefinition *Spell = USpellDatabase::GetSpell(Item.Name);
	if (!Spell)
		return;

	if (Item.UseType == EItemUseType::RuneSpell || Spell->SpellType == ESpellType::Potion)
	{
		if (AFibulaGameState *GameState = GetWorld()->GetGameState<AFibulaGameState>())
		{
			if (ASpellSystem *SpellSystem = GameState->GetSpellSystem())
			{
				UE_LOG(LogTemp, Warning, TEXT("Server: Using item spell: %s"), *Item.Name);
				SpellSystem->ServerTryExecuteSpell(this, Item.Name, TargetLocation);
			}
		}
	}
}

void AFibulaCharacter::ServerCastSpell_Implementation(const FString &SpellName)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AFibulaGameState *GameState = GetWorld()->GetGameState<AFibulaGameState>())
	{
		if (ASpellSystem *SpellSystem = GameState->GetSpellSystem())
		{
			SpellSystem->ServerTryExecuteSpell(this, SpellName);
		}
	}
}

void AFibulaCharacter::HandleContainerTargeting(AItemContainer *TargetContainer, float Distance)
{
	const float LOOT_DISTANCE = 300.0f;

	if (!TargetContainer || Distance > LOOT_DISTANCE)
	{
		return;
	}

	if (APlayerController *PC = Cast<APlayerController>(Controller))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			
			if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
			{
				StatusWidget->ShowInventory();
			}

			
			HUD->ShowLootWindow(TargetContainer);
		}
	}
}

void AFibulaCharacter::ServerTransferItemFromContainer_Implementation(AItemContainer *Container, const FGameItem &Item)
{
	if (!HasAuthority() || !IsValid(Container))
		return;

	
	float Distance = FVector::Distance(GetActorLocation(), Container->GetActorLocation());
	if (Distance > 300.0f)
		return;

	
	float ItemWeight = Item.Weight;
	float RemainingCapacity = GetMaxCapacity() - GetCurrentCapacity();
	int32 MaxItemsCanCarry = FMath::FloorToInt(RemainingCapacity / ItemWeight);

	int32 ItemsToTransfer = FMath::Min(Item.StackCount, MaxItemsCanCarry);
	int32 ItemsPerTransfer = (Item.EquipmentAttributes.EquipmentType == EEquipmentType::Ammunition) ? 100 : 50;
	ItemsToTransfer = FMath::Min(ItemsToTransfer, ItemsPerTransfer);

	if (ItemsToTransfer <= 0)
		return;

	
	FGameItem TransferItem = Item;
	TransferItem.StackCount = ItemsToTransfer;

	
	if (!Container->RemoveItem(Item.Name, ItemsToTransfer))
		return;

	
	if (!AddItem(TransferItem))
	{
		Container->AddItem(TransferItem);
	}
}

void AFibulaCharacter::ServerSwapInventorySlots_Implementation(const FGameItem &SourceItem, const FGameItem &TargetItem)
{
	if (!HasAuthority())
		return;

	int32 SourceIndex = -1;
	int32 TargetIndex = -1;

	
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].Name == SourceItem.Name &&
			Inventory[i].StackCount == SourceItem.StackCount)
		{
			SourceIndex = i;
		}
		if (!TargetItem.Name.IsEmpty() &&
			Inventory[i].Name == TargetItem.Name &&
			Inventory[i].StackCount == TargetItem.StackCount)
		{
			TargetIndex = i;
		}
	}

	
	if (SourceIndex == -1)
		return;

	if (TargetIndex != -1)
	{
		
		FGameItem TempItem = Inventory[SourceIndex];
		Inventory[SourceIndex] = Inventory[TargetIndex];
		Inventory[TargetIndex] = TempItem;
	}
	else if (TargetItem.Name.IsEmpty())
	{
		
		FGameItem MovedItem = Inventory[SourceIndex];
		Inventory.RemoveAt(SourceIndex);
		Inventory.Add(MovedItem);
	}

	
	OnRep_Inventory();
}

void AFibulaCharacter::ServerTransferItemToContainer_Implementation(AItemContainer *Container, const FGameItem &Item)
{
	if (!HasAuthority())
		return;

	
	float Distance = FVector::Distance(GetActorLocation(), Container->GetActorLocation());
	if (Distance > 300.0f)
		return;

	
	int32 InventoryIndex = -1;
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].Name == Item.Name &&
			Inventory[i].StackCount == Item.StackCount)
		{
			InventoryIndex = i;
			break;
		}
	}

	if (InventoryIndex == -1)
		return;

	
	Container->AddItem(Item);

	
	Inventory.RemoveAt(InventoryIndex);

	UpdateCurrentCapacity();
}

void AFibulaCharacter::OnHotkeyPressed(FKey Key)
{
	UE_LOG(LogTemp, Warning, TEXT("Hotkey pressed: %s"), *Key.ToString());

	if (APlayerController *PC = Cast<APlayerController>(Controller))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			FString KeyString = Key.ToString();
			FString NumericKey;

			if (KeyString == "One")
				NumericKey = "1";
			else if (KeyString == "Two")
				NumericKey = "2";
			else if (KeyString == "Three")
				NumericKey = "3";
			else if (KeyString == "Four")
				NumericKey = "4";
			else if (KeyString == "Five")
				NumericKey = "5";
			else if (KeyString == "Six")
				NumericKey = "6";
			else if (KeyString == "Seven")
				NumericKey = "7";
			else if (KeyString == "Eight")
				NumericKey = "8";
			else if (KeyString == "Nine")
				NumericKey = "9";
			else if (KeyString == "Zero")
				NumericKey = "0";
			else if (KeyString == "Q")
				NumericKey = "Q";
			else if (KeyString == "Z")
				NumericKey = "Z";
			else if (KeyString == "E")
				NumericKey = "E";
			else if (KeyString == "C")
				NumericKey = "C";
			else if (KeyString == "R")
				NumericKey = "R";
			else if (KeyString == "F")
				NumericKey = "F";
			else if (KeyString == "V")
				NumericKey = "V";
			else if (KeyString == "T")
				NumericKey = "T";
			else if (KeyString == "G")
				NumericKey = "G";
			else if (KeyString == "B")
				NumericKey = "B";

			if (!NumericKey.IsEmpty())
			{
				HUD->HandleHotkeyPressed(FKey(*NumericKey));
			}
		}
	}
}

void AFibulaCharacter::EnableMagicShield(bool bEnable)
{
	if (HasAuthority())
	{
		bHasMagicShield = bEnable;
	}
}

void AFibulaCharacter::SetSpeed(int32 NewSpeed)
{
	if (HasAuthority())
	{
		Speed = NewSpeed;

		
		if (UCharacterMovementComponent *MovementComponent = GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = NewSpeed;
		}
	}
}

bool AFibulaCharacter::IsInCombat() const
{
	if (IsInProtectionZone())
		return false;

	return (GetWorld()->GetTimeSeconds() - LastCombatTime) < PROTECTION_ZONE_COMBAT_DELAY;
}

void AFibulaCharacter::UpdateLastCombatTime()
{
	LastCombatTime = GetWorld()->GetTimeSeconds();
}

void AFibulaCharacter::OnRep_CharacterName()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_CharacterName called"));
	UE_LOG(LogTemp, Warning, TEXT("CharacterName: %s"), *CharacterName);

	if (!HasAuthority())
	{
		if (NameTagWidget)
		{
			NameTagWidget->SetCharacterName(CharacterName);
		}
	}
}

void AFibulaCharacter::SetInProtectionZone(bool bNewValue)
{
	if (HasAuthority())
	{
		bIsInProtectionZone = bNewValue;

		
		if (bNewValue)
		{
			ClearAsTargetOnNearbyPlayers();
		}
	}
}

void AFibulaCharacter::OnRep_ProtectionZone()
{
	UpdateAllStatusIcons();
}

void AFibulaCharacter::OnRep_Combat()
{
	UpdateAllStatusIcons();
}

void AFibulaCharacter::OnRep_MagicShield()
{
	UpdateAllStatusIcons();
}

void AFibulaCharacter::UpdateAllStatusIcons()
{
	
	if (!IsLocallyControlled())
		return;

	APlayerController *PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD());
	if (!HUD)
	{
		return;
	}

	UStatusWidget *StatusWidget = HUD->GetStatusWidget();
	if (!StatusWidget)
	{
		return;
	}

	
	StatusWidget->ClearAllStatusIcons();

	
	if (bIsInProtectionZone)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Protection Zone icon"));
		StatusWidget->AddStatusIcon(EStatusIconType::InProtectionZone);
	}

	
	if (IsInCombat())
	{
		StatusWidget->AddStatusIcon(EStatusIconType::InBattle);
	}

	
	if (bHasMagicShield)
	{
		StatusWidget->AddStatusIcon(EStatusIconType::MagicShield);
	}

	if (Speed > GetBaseSpeed())
	{
		StatusWidget->AddStatusIcon(EStatusIconType::Haste);
	}

	if (Speed < GetBaseSpeed())
	{
		StatusWidget->AddStatusIcon(EStatusIconType::Paralysed);
	}

	
	if (bIsStrengthened)
	{
		StatusWidget->AddStatusIcon(EStatusIconType::Strengthened);
	}
}

void AFibulaCharacter::IncrementKills()
{
	if (HasAuthority())
	{
		KillCount++;
	}
}

void AFibulaCharacter::IncrementDeaths()
{
	if (HasAuthority())
	{
		DeathCount++;
	}
}

void AFibulaCharacter::OnRep_KillCount()
{
	UpdateSkullStatus();
}

void AFibulaCharacter::OnRep_DeathCount()
{
	UpdateSkullStatus();
}

void AFibulaCharacter::UpdateSkullStatus()
{
	if (HasAuthority() || !NameTagWidget)
		return;

	int32 KillBalance = KillCount - DeathCount;

	if (KillBalance <= 10)
	{
		NameTagWidget->UpdateSkullStatus(ESkullType::None);
	}
	else if (KillBalance > 10)
	{
		NameTagWidget->UpdateSkullStatus(ESkullType::White);
	}
	else if (KillBalance > 50)
	{
		NameTagWidget->UpdateSkullStatus(ESkullType::Red);
	}
	else if (KillBalance > 100)
	{
		NameTagWidget->UpdateSkullStatus(ESkullType::Black);
	}
}

void AFibulaCharacter::SetTeamId(int32 NewTeamId)
{
	if (HasAuthority())
	{
		TeamId = NewTeamId;
	}
}

void AFibulaCharacter::SetStrengthened(bool bNewValue)
{
	if (HasAuthority())
	{
		bIsStrengthened = bNewValue;
	}
}

void AFibulaCharacter::OnRep_Strengthened()
{
	UpdateAllStatusIcons();
}

void AFibulaCharacter::ShowDamageToClosestPlayers(AFibulaCharacter *Target, int32 Damage, EDamageDisplayType DisplayType)
{
	TArray<AFibulaCharacter *> NearbyPlayers = UGameplayUtils::GetPlayersWithinRadius(this, Target->GetActorLocation(), 2500.0f);
	for (AFibulaCharacter *Player : NearbyPlayers)
	{
		Player->ClientShowDamage(Target, Damage, DisplayType);
	}
}

void AFibulaCharacter::SetTeamMaterial(int32 NewTeamId)
{
	
}

void AFibulaCharacter::OnRep_TeamId()
{
}

void AFibulaCharacter::EnableLight(bool bEnable)
{
	if (LightComponent)
	{
		LightComponent->SetVisibility(bEnable);
	}
}

void AFibulaCharacter::OnRep_Light()
{
	EnableLight(bHasLight);
}

void AFibulaCharacter::CancelCombatLogout()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(CombatLogoutTimer);
	}
}

void AFibulaCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		CancelCombatLogout();
	}
}

void AFibulaCharacter::EquipItem(const FGameItem &Item)
{
	if (!HasAuthority() || Item.ItemType != EItemType::Equipment)
		return;

	
	int32 InventoryIndex = -1;
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].Name == Item.Name &&
			Inventory[i].StackCount == Item.StackCount)
		{
			InventoryIndex = i;
			break;
		}
	}

	if (InventoryIndex == -1)
		return;

	
	if (Item.EquipmentAttributes.EquipmentType == EEquipmentType::Weapon)
	{
		bool bIsTwoHanded = Item.EquipmentAttributes.bIsTwoHanded;
		bool bHasShieldEquipped = !EquippedShield.Name.IsEmpty();

		if (bIsTwoHanded && bHasShieldEquipped)
		{
			
			return;
		}
	}
	else if (Item.EquipmentAttributes.EquipmentType == EEquipmentType::Shield)
	{
		
		if (!EquippedWeapon.Name.IsEmpty() && EquippedWeapon.EquipmentAttributes.bIsTwoHanded)
		{
			
			return;
		}
	}

	
	FGameItem *EquipmentSlot = nullptr;
	switch (Item.EquipmentAttributes.EquipmentType)
	{
	case EEquipmentType::Ammunition:
		EquipmentSlot = &EquippedAmmunition;
		break;
	case EEquipmentType::Helm:
		EquipmentSlot = &EquippedHelm;
		break;
	case EEquipmentType::Armor:
		EquipmentSlot = &EquippedArmor;
		break;
	case EEquipmentType::Bag:
		EquipmentSlot = &EquippedBag;
		break;
	case EEquipmentType::Weapon:
		EquipmentSlot = &EquippedWeapon;
		break;
	case EEquipmentType::Shield:
		EquipmentSlot = &EquippedShield;
		break;
	case EEquipmentType::Legs:
		EquipmentSlot = &EquippedLegs;
		break;
	case EEquipmentType::Boots:
		EquipmentSlot = &EquippedBoots;
		break;
	case EEquipmentType::Amulet:
		EquipmentSlot = &EquippedAmulet;
		break;
	case EEquipmentType::Ring:
		EquipmentSlot = &EquippedRing;
		break;
	default:
		return;
	}

	
	FGameItem ItemToRemove = Inventory[InventoryIndex];

	
	if (!EquipmentSlot->Name.IsEmpty())
	{
		
		if (EquipmentSlot->Name == ItemToRemove.Name && ItemToRemove.bIsStackable)
		{
			
			EquipmentSlot->StackCount += ItemToRemove.StackCount;
			Inventory.RemoveAt(InventoryIndex);
		}
		else
		{
			
			FGameItem PreviouslyEquipped = *EquipmentSlot;
			*EquipmentSlot = ItemToRemove;
			Inventory[InventoryIndex] = PreviouslyEquipped;
		}
	}
	else
	{
		
		*EquipmentSlot = ItemToRemove;
		Inventory.RemoveAt(InventoryIndex);
	}
}

void AFibulaCharacter::ServerEquipItem_Implementation(const FGameItem &Item)
{
	EquipItem(Item);
}

void AFibulaCharacter::ServerUnequipItem_Implementation(const FGameItem &Item)
{
	if (!HasAuthority() || Item.ItemType != EItemType::Equipment)
		return;

	
	FGameItem *EquipmentSlot = nullptr;
	if (EquippedHelm.Name == Item.Name)
		EquipmentSlot = &EquippedHelm;
	else if (EquippedArmor.Name == Item.Name)
		EquipmentSlot = &EquippedArmor;
	else if (EquippedBag.Name == Item.Name)
		EquipmentSlot = &EquippedBag;
	else if (EquippedWeapon.Name == Item.Name)
		EquipmentSlot = &EquippedWeapon;
	else if (EquippedShield.Name == Item.Name)
		EquipmentSlot = &EquippedShield;
	else if (EquippedLegs.Name == Item.Name)
		EquipmentSlot = &EquippedLegs;
	else if (EquippedBoots.Name == Item.Name)
		EquipmentSlot = &EquippedBoots;
	else if (EquippedAmulet.Name == Item.Name)
		EquipmentSlot = &EquippedAmulet;
	else if (EquippedRing.Name == Item.Name)
		EquipmentSlot = &EquippedRing;
	else if (EquippedAmmunition.Name == Item.Name)
		EquipmentSlot = &EquippedAmmunition;

	if (!EquipmentSlot)
		return;

	
	FGameItem UnequippedItem = *EquipmentSlot;

	
	*EquipmentSlot = FGameItem();

	
	if (UnequippedItem.bIsStackable)
	{
		for (FGameItem &ExistingItem : Inventory)
		{
			if (ExistingItem.Name == UnequippedItem.Name)
			{
				ExistingItem.StackCount += UnequippedItem.StackCount;
				UpdateCurrentCapacity();
				return;
			}
		}
	}

	
	Inventory.Add(UnequippedItem);
	UpdateCurrentCapacity();
}

void AFibulaCharacter::OnRep_Equipment()
{
	if (!IsLocallyControlled())
		return;

	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
			{
				
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Helm, EquippedHelm);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Armor, EquippedArmor);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Bag, EquippedBag);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Weapon, EquippedWeapon);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Shield, EquippedShield);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Legs, EquippedLegs);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Boots, EquippedBoots);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Amulet, EquippedAmulet);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Ring, EquippedRing);
				StatusWidget->UpdateEquipmentSlot(EEquipmentType::Ammunition, EquippedAmmunition);
				UpdateStatusWidget();
			}
		}
	}
}

void AFibulaCharacter::OnRep_CurrentCapacity()
{
	if (!IsLocallyControlled())
		return;

	UpdateStatusWidget();
}

int32 AFibulaCharacter::GetAttack() const
{
	if (Vocation == EVocation::Knight)
	{
		return EquippedWeapon.EquipmentAttributes.Attack;
	}
	else if (Vocation == EVocation::Paladin)
	{
		return EquippedAmmunition.EquipmentAttributes.Attack + EquippedWeapon.EquipmentAttributes.Attack;
	}
	return 0;
}

int32 AFibulaCharacter::GetDefense() const
{
	return EquippedShield.EquipmentAttributes.Defense + EquippedWeapon.EquipmentAttributes.Defense;
}

int32 AFibulaCharacter::GetArmor() const
{
	return EquippedHelm.EquipmentAttributes.Armor +
		   EquippedArmor.EquipmentAttributes.Armor +
		   EquippedLegs.EquipmentAttributes.Armor +
		   EquippedBoots.EquipmentAttributes.Armor +
		   EquippedAmulet.EquipmentAttributes.Armor;
}

int32 AFibulaCharacter::GetBaseSpeed() const
{
	int32 BaseSpeed = GameFormulas::CalculateSpeed(CharacterLevel, Vocation);
	return BaseSpeed * (1.0f + EquippedBoots.EquipmentAttributes.SpeedBonus);
}

int32 AFibulaCharacter::GetMagicLevel() const
{
	if (Vocation == EVocation::Sorcerer || Vocation == EVocation::Druid)
	{
		return (MagicLevel + EquippedShield.EquipmentAttributes.MagicLevelBonus +
				EquippedBoots.EquipmentAttributes.MagicLevelBonus +
				EquippedArmor.EquipmentAttributes.MagicLevelBonus +
				EquippedHelm.EquipmentAttributes.MagicLevelBonus +
				EquippedLegs.EquipmentAttributes.MagicLevelBonus +
				EquippedWeapon.EquipmentAttributes.MagicLevelBonus +
				EquippedAmulet.EquipmentAttributes.MagicLevelBonus +
				EquippedRing.EquipmentAttributes.MagicLevelBonus) *
			   MagicLevelMultiplier;
	}

	return MagicLevel;
}

int32 AFibulaCharacter::GetMeleeSkill() const
{
	int32 ItemBonus = 0;
	if (Vocation == EVocation::Knight)
	{
		ItemBonus = EquippedWeapon.EquipmentAttributes.MeleeSkillBonus +
					EquippedHelm.EquipmentAttributes.MeleeSkillBonus +
					EquippedArmor.EquipmentAttributes.MeleeSkillBonus +
					EquippedLegs.EquipmentAttributes.MeleeSkillBonus +
					EquippedShield.EquipmentAttributes.MeleeSkillBonus +
					EquippedAmulet.EquipmentAttributes.MeleeSkillBonus +
					EquippedRing.EquipmentAttributes.MeleeSkillBonus +
					EquippedBoots.EquipmentAttributes.MeleeSkillBonus;
	}
	return (MeleeSkill + ItemBonus) * MeleeSkillMultiplier;
}

int32 AFibulaCharacter::GetDistanceSkill() const
{
	int32 ItemBonus = 0;
	if (Vocation == EVocation::Paladin)
	{
		ItemBonus = EquippedWeapon.EquipmentAttributes.DistanceSkillBonus +
					EquippedAmmunition.EquipmentAttributes.DistanceSkillBonus +
					EquippedHelm.EquipmentAttributes.DistanceSkillBonus +
					EquippedArmor.EquipmentAttributes.DistanceSkillBonus +
					EquippedLegs.EquipmentAttributes.DistanceSkillBonus +
					EquippedAmulet.EquipmentAttributes.DistanceSkillBonus +
					EquippedRing.EquipmentAttributes.DistanceSkillBonus +
					EquippedBoots.EquipmentAttributes.DistanceSkillBonus;
	}
	return (DistanceSkill + ItemBonus) * DistanceSkillMultiplier;
}

void AFibulaCharacter::HandleDeathItems(const FVector &DeathLocation, const FRotator &DeathRotation)
{
	
	bool bPreventItemLoss = !EquippedAmulet.Name.IsEmpty() &&
							EquippedAmulet.EquipmentAttributes.bPreventItemLoss;

	
	TArray<FGameItem> LostItems;

	if (bPreventItemLoss)
	{
		
		EquippedAmulet = FGameItem();
	}
	else
	{
		
		LostItems = Inventory;
		const float LossChance = 0.01f;

		auto ProcessEquipmentLoss = [&](FGameItem &EquipmentSlot)
		{
			if (!EquipmentSlot.Name.IsEmpty() && FMath::FRand() < LossChance)
			{
				LostItems.Add(EquipmentSlot);
				EquipmentSlot = FGameItem();
				return true;
			}
			return false;
		};

		
		ProcessEquipmentLoss(EquippedHelm);
		ProcessEquipmentLoss(EquippedArmor);
		ProcessEquipmentLoss(EquippedBag);
		ProcessEquipmentLoss(EquippedWeapon);
		ProcessEquipmentLoss(EquippedShield);
		ProcessEquipmentLoss(EquippedLegs);
		ProcessEquipmentLoss(EquippedBoots);
		ProcessEquipmentLoss(EquippedAmulet);
		ProcessEquipmentLoss(EquippedRing);
		ProcessEquipmentLoss(EquippedAmmunition);

		
		Inventory.Empty();
	}

	
	bool bShouldDropReward = false;
	if (FMath::FRand() < 0.35f) 
	{
		
		TArray<AFibulaCharacter *> ValidDealers;
		float CurrentTime = GetWorld()->GetTimeSeconds();
		bool bTargetDamagedSomeone = false;

		for (auto &Pair : DamageDealers)
		{
			if (CurrentTime - Pair.Value.Timestamp <= DAMAGE_TRACKING_DURATION)
			{
				ValidDealers.Add(Pair.Key);
			}
		}

		
		for (AFibulaCharacter *Dealer : ValidDealers)
		{
			if (Dealer && Dealer->DamageDealers.Contains(this))
			{
				bTargetDamagedSomeone = true;
				break;
			}
		}

		
		if (ValidDealers.Num() >= 2 && bTargetDamagedSomeone)
		{
			if (const FGameItem *RewardPresent = UItemDatabase::GetItem(TEXT("Reward Present")))
			{
				LostItems.Add(*RewardPresent);
			}
		}
	}

	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADeadCharacter *Corpse = GetWorld()->SpawnActor<ADeadCharacter>(
		ADeadCharacter::StaticClass(),
		DeathLocation,
		DeathRotation,
		SpawnParams);

	if (Corpse)
	{
		
		Corpse->SetCorpseMesh(CurrentMesh);

		
		Corpse->SetInventory(LostItems);
	}

	OnRep_Equipment();
	OnRep_Inventory();
}

void AFibulaCharacter::ClearAsTargetOnNearbyPlayers()
{
	TArray<AFibulaCharacter *> NearbyPlayers = UGameplayUtils::GetPlayersWithinRadius(this, GetActorLocation(), 5000.0f);
	for (AFibulaCharacter *Player : NearbyPlayers)
	{
		if (Player && Player->GetCurrentTarget() == this)
		{
			Player->ClearTarget();
		}
		if (Player && Player->GetHealingTarget() == this)
		{
			Player->ClearHealingTarget();
		}
	}
}

void AFibulaCharacter::ServerOpenRewardPresent_Implementation()
{
	if (!HasAuthority())
		return;

	int32 RewardPresentIndex = Inventory.IndexOfByPredicate([](const FGameItem &Item)
															{ return Item.Name == TEXT("Reward Present"); });

	if (RewardPresentIndex == INDEX_NONE)
	{
		return;
	}

	URewardSystem::ProcessRewardPresent(this);
}

void AFibulaCharacter::ServerRemoveItem_Implementation(const FString &ItemName, int32 Count)
{
	
	if (!HasAuthority())
		return;

	
	APlayerController *PC = Cast<APlayerController>(GetController());
	if (!PC || PC->Player == nullptr || PC->Player->GetPlayerController(nullptr) != PC)
		return;

	
	bool bFoundItem = false;
	for (const FGameItem &InventoryItem : Inventory)
	{
		if (InventoryItem.Name == ItemName &&
			InventoryItem.StackCount >= Count)
		{
			bFoundItem = true;
			break;
		}
	}

	if (!bFoundItem)
		return;

	
	RemoveItem(ItemName, Count);
}

int32 AFibulaCharacter::GetMaxCapacity() const
{
	return MaxCapacity + EquippedBag.EquipmentAttributes.CapacityBonus;
}

FString AFibulaCharacter::GetVocationAsString() const
{
	switch (Vocation)
	{
	case EVocation::Knight:
		return TEXT("Knight");
	case EVocation::Paladin:
		return TEXT("Paladin");
	case EVocation::Sorcerer:
		return TEXT("Sorcerer");
	case EVocation::Druid:
		return TEXT("Druid");
	default:
		return TEXT("Unknown");
	}
}

void AFibulaCharacter::Destroyed()
{
	if (HasAuthority())
	{
		
		if (AFibulaGameMode *GameMode = Cast<AFibulaGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->PersistCharacterItems(this);
		}
	}

	Super::Destroyed();
}

bool AFibulaCharacter::AddItemDirect(const FGameItem &Item)
{
	if (!HasAuthority())
		return false;

	
	if (Item.bIsStackable)
	{
		for (FGameItem &ExistingItem : Inventory)
		{
			if (ExistingItem.Name == Item.Name)
			{
				ExistingItem.StackCount += Item.StackCount;
				UpdateCurrentCapacity();
				return true;
			}
		}
	}

	
	Inventory.Add(Item);
	UpdateCurrentCapacity();
	return true;
}

void AFibulaCharacter::HandleCombatLogout()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			CombatLogoutTimer,
			[this]()
			{ Destroy(); },
			COMBAT_LOGOUT_DELAY,
			false);
	}
}

AFibulaCharacter *AFibulaCharacter::FindNextTarget(bool bHealingTarget)
{
	TArray<AFibulaCharacter *> ValidTargets;
	TArray<AActor *> NearbyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFibulaCharacter::StaticClass(), NearbyActors);

	
	for (AActor *Actor : NearbyActors)
	{
		AFibulaCharacter *Character = Cast<AFibulaCharacter>(Actor);
		if (!Character || Character == this || Character->IsInProtectionZone())
			continue;

		float Distance = FVector::Distance(GetActorLocation(), Character->GetActorLocation());
		if (Distance > MAX_TARGET_DISTANCE)
			continue;

		
		if (bHealingTarget)
		{
			if (bIsInTeamBattle && !IsAllyOf(Character))
			{
				ValidTargets.Add(Character);
			}
		}
		
		else
		{
			if (!bIsInTeamBattle || IsOpponentOf(Character))
			{
				ValidTargets.Add(Character);
			}
		}
	}

	if (ValidTargets.Num() == 0)
		return nullptr;

	
	AFibulaCharacter *CurrentlyTargeted = bHealingTarget ? HealingTarget : CurrentTarget;
	if (!CurrentlyTargeted)
		return ValidTargets[0];

	
	int32 CurrentIndex = ValidTargets.Find(CurrentlyTargeted);
	int32 NextIndex = (CurrentIndex + 1) % ValidTargets.Num();
	return ValidTargets[NextIndex];
}

void AFibulaCharacter::HandleTabTargeting(bool bHealingTarget)
{
	AFibulaCharacter *NextTarget = FindNextTarget(bHealingTarget);
	if (NextTarget)
	{
		float Distance = FVector::Distance(GetActorLocation(), NextTarget->GetActorLocation());
		HandleCharacterTargeting(NextTarget, Distance);
	}
}

void AFibulaCharacter::ToggleInventory()
{
	if (APlayerController *PC = Cast<APlayerController>(Controller))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
			{
				StatusWidget->ToggleInventory();
			}
		}
	}
}

void AFibulaCharacter::ToggleScoreboard()
{
	if (APlayerController *PC = Cast<APlayerController>(Controller))
	{
		if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
		{
			if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
			{
				StatusWidget->ToggleScoreboard();
			}
		}
	}
}

bool AFibulaCharacter::IsOpponentOf(AFibulaCharacter *Other) const
{
	return TeamId != Other->TeamId;
}

bool AFibulaCharacter::IsAllyOf(AFibulaCharacter *Other) const
{
	return !IsOpponentOf(Other);
}

void AFibulaCharacter::Zoom(const FInputActionValue &Value)
{
	if (CameraBoom)
	{
		const float ZoomValue = Value.Get<float>();
		const float NewArmLength = FMath::Clamp(CameraBoom->TargetArmLength - (ZoomValue * ZoomSpeed), MinZoomDistance, MaxZoomDistance);
		CameraBoom->TargetArmLength = NewArmLength;
	}
}

void AFibulaCharacter::OnMouseWheel(float Value)
{
	if (CameraBoom)
	{
		const float NewArmLength = FMath::Clamp(CameraBoom->TargetArmLength - (Value * ZoomSpeed), MinZoomDistance, MaxZoomDistance);
		CameraBoom->TargetArmLength = NewArmLength;
	}
}

void AFibulaCharacter::OnLeftMousePressed()
{
	bLeftMousePressed = true;
}

void AFibulaCharacter::OnLeftMouseReleased()
{
	bLeftMousePressed = false;
}

void AFibulaCharacter::OnRightMousePressed()
{
	bRightMousePressed = true;
}

void AFibulaCharacter::OnRightMouseReleased()
{
	bRightMousePressed = false;
}

void AFibulaCharacter::SetVocation(EVocation InVocation)
{
	if (!HasAuthority())
		return;

	Vocation = InVocation;
	if (TeamId == 0) 
	{
		
		if (FMath::RandBool())
		{
			CurrentMesh = TeamAMeshes.FindRef(Vocation);
		}
		else
		{
			CurrentMesh = TeamBMeshes.FindRef(Vocation);
		}
	}
	else if (TeamId == 1)
	{
		CurrentMesh = TeamAMeshes.FindRef(Vocation);
	}
	else if (TeamId == 2)
	{
		CurrentMesh = TeamBMeshes.FindRef(Vocation);
	}

	if (CurrentMesh && GetMesh())
	{
		GetMesh()->SetSkeletalMesh(CurrentMesh);
	}
}

void AFibulaCharacter::OnRep_CurrentMesh()
{
	if (GetMesh() && CurrentMesh)
	{
		GetMesh()->SetSkeletalMesh(CurrentMesh);

		
		FVector MeshRelativeLocation = GetMesh()->GetRelativeLocation();
		if (CurrentMesh && CurrentMesh->GetName().Contains("WizardSM")) 
		{
			MeshRelativeLocation.Z = -90.0f;
		}
		else
		{
			MeshRelativeLocation.Z = -108.0f;
		}
		GetMesh()->SetRelativeLocation(MeshRelativeLocation);
	}
}

void AFibulaCharacter::ClientPinCameraAtDeathLocation_Implementation(const FVector &Location, AFibulaCharacter *Killer, float Duration)
{
	if (!IsLocallyControlled())
		return;

	SavedCameraLocation = Location;
	bIsCameraPinned = true;

	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		PC->DisableInput(PC);

		if (Killer && IsValid(Killer))
		{
			
			SavedCameraBoomTargetArmLength = CameraBoom->TargetArmLength;
			SavedCameraBoomRotation = CameraBoom->GetRelativeRotation();
			SavedCameraBoomLag = CameraBoom->bEnableCameraLag;
			SavedCameraBoomLagSpeed = CameraBoom->CameraLagSpeed;

			
			CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			CameraBoom->AttachToComponent(Killer->GetRootComponent(),
										  FAttachmentTransformRules::SnapToTargetNotIncludingScale);

			
			CameraBoom->TargetArmLength = 400.0f;
			CameraBoom->bUsePawnControlRotation = true;
			CameraBoom->bEnableCameraLag = true;
			CameraBoom->CameraLagSpeed = 10.0f;
			CameraBoom->CameraLagMaxDistance = 100.0f;

			
			CameraBoom->SetWorldRotation(Killer->GetActorRotation());
		}
		else
		{
			
			CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			CameraBoom->SetWorldLocation(Location + FVector(0, 0, 50.0f));
			CameraBoom->SetWorldRotation(FRotator(-45.0f, 0.0f, 0.0f));
			CameraBoom->TargetArmLength = 400.0f;
		}
	}

	if (Duration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			CameraRestoreTimer,
			this,
			&AFibulaCharacter::ClientRestoreCamera,
			Duration,
			false);
	}
}

void AFibulaCharacter::ClientRestoreCamera()
{
	if (!IsLocallyControlled() || !bIsCameraPinned)
		return;

	bIsCameraPinned = false;

	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		PC->EnableInput(PC);

		
		CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		CameraBoom->AttachToComponent(GetRootComponent(),
									  FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		
		CameraBoom->TargetArmLength = SavedCameraBoomTargetArmLength;
		CameraBoom->SetRelativeRotation(SavedCameraBoomRotation);
		CameraBoom->bEnableCameraLag = SavedCameraBoomLag;
		CameraBoom->CameraLagSpeed = SavedCameraBoomLagSpeed;
	}
}