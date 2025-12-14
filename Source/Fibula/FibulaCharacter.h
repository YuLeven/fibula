

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CharacterEnums.h"
#include "PlayerNameTagWidget.h"
#include "Components/WidgetComponent.h"
#include "Containers/Map.h"
#include "FloatingTextWidget.h"
#include "GameItem.h"
#include "ItemDatabase.h"
#include "Net/UnrealNetwork.h"
#include "ItemContainer.h"
#include "NameTagWidgetComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/PointLightComponent.h"
#include "DamageTypes.h"
#include "FibulaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AFibulaCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *LookAction;

	/** Chat Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *ChatAction;

	/** Target Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *TargetAction;

	/** Escape Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *EscapeAction;

	/** Auto Attack Effect */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	UNiagaraSystem *AutoAttackEffect;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Experience)
	int64 Experience;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterLevel)
	int32 CharacterLevel;
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	int32 CurrentHealth;

	UPROPERTY(Replicated)
	int32 MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	int32 CurrentMana;

	UPROPERTY(Replicated)
	int32 MaxMana;

	UPROPERTY(Replicated)
	int32 MagicLevel;

	UPROPERTY(Replicated)
	int32 MeleeSkill;

	UPROPERTY(Replicated)
	int32 DistanceSkill;

	UPROPERTY(Replicated)
	int32 ShieldingSkill;

	UPROPERTY(ReplicatedUsing = OnRep_Vocation)
	EVocation Vocation;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentCapacity)
	float CurrentCapacity;

	UPROPERTY(Replicated)
	float MaxCapacity;

	UPROPERTY(ReplicatedUsing = OnRep_Speed)
	int32 Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI)
	UNameTagWidgetComponent *NameTagWidget;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterName)
	FString CharacterName;

	FTimerHandle GeneralExhaustTimer;
	FTimerHandle OffensiveExhaustTimer;
	UPROPERTY(Replicated)
	bool bIsGenerallyExhausted;
	UPROPERTY(Replicated)
	bool bIsOffensivelyExhausted;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	AFibulaCharacter *CurrentTarget;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	AFibulaCharacter *HealingTarget;

	UPROPERTY(Replicated)
	bool bIsInTeamBattle;

	UPROPERTY()
	UNiagaraComponent *TargetNiagaraComponent;

	UPROPERTY()
	UNiagaraComponent *HealingTargetNiagaraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem *TargetNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem *HealingTargetNiagaraSystem;

	FTimerHandle TargetCheckTimer;
	const float TARGET_CHECK_INTERVAL = 0.5f;
	const float MAX_TARGET_DISTANCE = 10000.0f;

	UPROPERTY()
	UMaterialInstanceDynamic *DynamicDecalMaterial;

	const float AUTO_ATTACK_INTERVAL = 2.0f;
	const float KNIGHT_ATTACK_RANGE = 210.0f;
	const float MAGE_ATTACK_RANGE = 400.0f;
	const float PALADIN_ATTACK_RANGE = 2500.0f;

	FTimerHandle AutoAttackTimer;

	UPROPERTY(ReplicatedUsing = OnRep_KillCount)
	int32 KillCount;

	UPROPERTY(ReplicatedUsing = OnRep_DeathCount)
	int32 DeathCount;

public:
	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FGameItem> Inventory;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FGameItem &Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(const FString &ItemName, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(const FString &ItemName) const;

	FString GetCharacterName() const { return CharacterName; }

	void SetCharacterName(const FString &Name) { CharacterName = Name; }
	void SetVocation(EVocation SelectedVocation);
	void InitializeCharacterStats(bool bResetLevelAndExperience = true);
	void SetSpeed(int32 NewSpeed);

	UFUNCTION(Server, Reliable)
	void ServerOpenRewardPresent();

	UFUNCTION(Server, Reliable)
	void ServerRemoveItem(const FString &ItemName, int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Character")
	FString GetVocationAsString() const;

	virtual void Destroyed() override;
	void HandleCombatLogout();

	void CancelCombatLogout();
	virtual void PossessedBy(AController *NewController) override;

private:
	struct FDamageDealer
	{
		float Timestamp;
		int32 DamageDealt;

		FDamageDealer() : Timestamp(0.0f), DamageDealt(0) {}
		FDamageDealer(float InTimestamp, int32 InDamage)
			: Timestamp(InTimestamp), DamageDealt(InDamage) {}
	};

	TMap<AFibulaCharacter *, FDamageDealer> DamageDealers;
	const float DAMAGE_TRACKING_DURATION = 300.0f; 

	UPROPERTY()
	AFibulaCharacter *LastDamageDealer;
	void HandleDeathItems(const FVector &DeathLocation, const FRotator &DeathRotation);
	void ToggleInventory();
	void ToggleScoreboard();
	void Zoom(const FInputActionValue &Value);
	void OnMouseWheel(float Value);

public:
	AFibulaCharacter();
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void StartAutoAttack();
	void StopAutoAttack();
	void PerformAutoAttack();
	float GetAttackRange() const;

	UFUNCTION(Server, Reliable)
	void ServerPerformAutoAttack(AFibulaCharacter *Target);

	UFUNCTION(Client, Reliable)
	void ShowCenteredMessage(const FString &Message, float Duration = 3.0f, EFloatingTextType TextType = EFloatingTextType::MinorInfo);

	UFUNCTION(Client, Reliable)
	void HandleLevelChange(int32 OldLevel, int32 NewLevel);

	UFUNCTION(Client, Reliable)
	void HandleSkillGain(const FString &SkillName, int32 NewLevel);
	void ModifyMana(int32 Amount);

	UFUNCTION(Server, Reliable)
	void ServerCastSpell(const FString &SpellName);

	void SetIsTeamBattle(bool isTeamBattle) { bIsInTeamBattle = isTeamBattle; }

protected:
	/** Called for movement input */
	void Move(const FInputActionValue &Value);

	/** Called for looking input */
	void Look(const FInputActionValue &Value);

	/** Called for chat input */
	void HandleChatInput(const FInputActionValue &Value);

	/** Called for targeting input */
	void HandleTargeting();

	void HandleEscapeInput(const FInputActionValue &Value);

protected:
	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE int32 GetCharacterLevel() const { return CharacterLevel; }
	FORCEINLINE int64 GetExperience() const { return Experience; }
	FORCEINLINE int32 GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE int32 GetCurrentMana() const { return CurrentMana; }
	FORCEINLINE int32 GetMaxMana() const { return MaxMana; }
	int32 GetMagicLevel() const;
	int32 GetMeleeSkill() const;
	int32 GetDistanceSkill() const;
	FORCEINLINE int32 GetShieldingSkill() const { return ShieldingSkill; }
	FORCEINLINE EVocation GetVocation() const { return Vocation; }
	FORCEINLINE int32 GetCurrentCapacity() const { return CurrentCapacity; }
	int32 GetMaxCapacity() const;
	int32 GetBaseSpeed() const;

	UFUNCTION(Server, Reliable)
	void ServerBroadcastMessage(const FString &SenderName, const FString &Message);

	void GainExperience(int64 Amount);
	void UpdateAttributes(bool bIsLevelUp = false);

	UFUNCTION(Server, Reliable)
	void ServerModifyHealth(int32 Amount, AFibulaCharacter *DamageDealer = nullptr, EDamageDisplayType DisplayType = EDamageDisplayType::Physical);

	bool IsGenerallyExhausted() const { return bIsGenerallyExhausted; }
	bool IsOffensivelyExhausted() const { return bIsOffensivelyExhausted; }
	void SetGeneralExhaust(bool bExhausted);
	void SetOffensiveExhaust(bool bExhausted);

	UFUNCTION(Client, Reliable)
	void ClientShowExhaustedMessage();

	UFUNCTION(Server, Reliable)
	void ServerSetTarget(AFibulaCharacter *NewTarget);

	UFUNCTION(Server, Reliable)
	void ServerSetHealingTarget(AFibulaCharacter *NewTarget);

	UFUNCTION()
	void CheckTargetDistance();

	AFibulaCharacter *GetCurrentTarget() const { return CurrentTarget; }
	AFibulaCharacter *GetHealingTarget() const { return HealingTarget; }

	UFUNCTION(Client, Reliable)
	void ClientShowDamage(AFibulaCharacter *Target, int32 Damage, EDamageDisplayType DisplayType);

	bool HasMagicShield() const { return bHasMagicShield; }

protected:
	UFUNCTION()
	void OnRep_Experience();

	UFUNCTION()
	void OnRep_CharacterLevel();

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_Mana();

	UFUNCTION()
	void OnRep_Vocation();

	UFUNCTION()
	void OnRep_CurrentCapacity();

	UFUNCTION()
	void OnRep_Speed();

	UFUNCTION()
	void OnRep_CurrentTarget();

	UFUNCTION()
	void OnRep_CharacterName();

	UFUNCTION()
	void OnRep_ProtectionZone();

	UFUNCTION()
	void OnRep_Combat();

	UFUNCTION()
	void OnRep_MagicShield();

	UFUNCTION()
	void OnRep_KillCount();

	UFUNCTION()
	void OnRep_DeathCount();

	UFUNCTION()
	void OnRep_TeamId();

	UFUNCTION()
	void OnRep_Light();

	void UpdateHealthBar();

	UFUNCTION(Client, Reliable)
	void ClientTargetingFailed();

	UFUNCTION(Client, Reliable)
	void ClientShowExperienceGain(AFibulaCharacter *Earner, int64 Amount);

private:
	float LastAutoAttackTime;
	void HandleDeath();
	void RespawnCharacter();
	void ClearTarget();
	void ClearHealingTarget();
	void UpdateTargetDecal();
	void SetTarget(AFibulaCharacter *NewTarget);
	void SetHealingTarget(AFibulaCharacter *NewTarget);
	void UpdateStatusWidget();
	void UpdateCurrentCapacity();
	void HandleCharacterTargeting(AFibulaCharacter *TargetCharacter, float Distance);
	void HandleContainerTargeting(AItemContainer *TargetContainer, float Distance);
	void ShowDamageToClosestPlayers(AFibulaCharacter *Target, int32 Damage, EDamageDisplayType DisplayType);
	void ClearAsTargetOnNearbyPlayers();

public:
	const TArray<FGameItem> &GetInventory() const { return Inventory; }

	UFUNCTION()
	void OnRep_Inventory();

	void EquipItem(const FGameItem &Item);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FGameItem &Item, const FVector &TargetLocation);

	UFUNCTION(Server, Reliable)
	void ServerTransferItemFromContainer(AItemContainer *Container, const FGameItem &Item);

	UFUNCTION(Server, Reliable)
	void ServerTransferItemToContainer(AItemContainer *Container, const FGameItem &Item);

	UFUNCTION(Server, Reliable)
	void ServerSwapInventorySlots(const FGameItem &SourceItem, const FGameItem &TargetItem);

	UFUNCTION(BlueprintCallable, Category = "Character|Combat")
	void EnableMagicShield(bool bEnable);

	UFUNCTION(Client, Reliable)
	void ClientSpawnSpellEffects(const FVector &Center, AFibulaCharacter *Caster, const FString &SpellWords);

protected:
	void OnHotkeyPressed(FKey Key);

	UFUNCTION(Client, Reliable)
	void ClientSpawnAutoAttackParticle(AFibulaCharacter *Caster, AFibulaCharacter *Target);

	
	UPROPERTY(ReplicatedUsing = OnRep_MagicShield)
	bool bHasMagicShield;
	void UpdateAllStatusIcons();

private:
	UPROPERTY(ReplicatedUsing = OnRep_ProtectionZone)
	bool bIsInProtectionZone;

	UPROPERTY(ReplicatedUsing = OnRep_Combat)
	float LastCombatTime;
	static constexpr float PROTECTION_ZONE_COMBAT_DELAY = 120.0f; 
	void UpdateLastCombatTime();

	
	UPROPERTY()
	UMaterialInterface *TeamBMaterial;

public:
	bool IsInProtectionZone() const { return bIsInProtectionZone; }
	void SetInProtectionZone(bool bNewValue);
	bool IsInCombat() const;

	void IncrementKills();
	void IncrementDeaths();
	void UpdateSkullStatus();

	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	int32 TeamId;

	int32 GetTeamId() const { return TeamId; }
	void SetTeamId(int32 NewTeamId);

	UPROPERTY(Replicated)
	float DamageReceivedMultiplier = 1.0f;

	UPROPERTY(Replicated)
	float MeleeSkillMultiplier = 1.0f;

	UPROPERTY(Replicated)
	float DistanceSkillMultiplier = 1.0f;

	UPROPERTY(Replicated)
	float MagicLevelMultiplier = 1.0f;

	UPROPERTY(Replicated)
	bool bCantHeal = false;

	bool IsOpponentOf(AFibulaCharacter *Other) const;
	bool IsAllyOf(AFibulaCharacter *Other) const;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Strengthened)
	bool bIsStrengthened;

	UFUNCTION()
	void OnRep_Strengthened();

public:
	void SetStrengthened(bool bNewValue);

	
	void SetTeamMaterial(int32 NewTeamId);

	int32 GetAttack() const;
	int32 GetDefense() const;
	int32 GetArmor() const;
	int32 GetKills() const { return KillCount; }
	int32 GetDeaths() const { return DeathCount; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Light)
	bool bHasLight;
	UPointLightComponent *LightComponent;
	void EnableLight(bool bEnable);

public:
	void SetLight(bool bEnable) { bHasLight = bEnable; }
	UFUNCTION(Server, Reliable)
	void ServerEquipItem(const FGameItem &Item);

	UFUNCTION(Server, Reliable)
	void ServerUnequipItem(const FGameItem &Item);
	const FGameItem &GetEquippedWeapon() const { return EquippedWeapon; }
	const FGameItem &GetEquippedShield() const { return EquippedShield; }
	const FGameItem &GetEquippedLegs() const { return EquippedLegs; }
	const FGameItem &GetEquippedBoots() const { return EquippedBoots; }
	const FGameItem &GetEquippedAmulet() const { return EquippedAmulet; }
	const FGameItem &GetEquippedRing() const { return EquippedRing; }
	const FGameItem &GetEquippedHelm() const { return EquippedHelm; }
	const FGameItem &GetEquippedAmmunition() const { return EquippedAmmunition; }
	const FGameItem &GetEquippedArmor() const { return EquippedArmor; }
	const FGameItem &GetEquippedBag() const { return EquippedBag; }
	bool AddItemDirect(const FGameItem &Item);
	/** Handle tab targeting logic */
	void HandleTabTargeting(bool bHealingTarget = false);

protected:
	UPROPERTY(Replicated)
	TArray<FGameItem> Equipment;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedHelm;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedAmmunition;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedArmor;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedBag;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedShield;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedLegs;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedBoots;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedAmulet;

	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FGameItem EquippedRing;

	UFUNCTION()
	void OnRep_Equipment();

private:
	FTimerHandle CombatLogoutTimer;
	static constexpr float COMBAT_LOGOUT_DELAY = 31.0f;

	/** Find the next valid target for tab targeting */
	AFibulaCharacter *FindNextTarget(bool bHealingTarget = false);

	
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinZoomDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxZoomDistance = 1300.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomSpeed = 50.0f;

	bool bLeftMousePressed;
	bool bRightMousePressed;

	void OnLeftMousePressed();
	void OnLeftMouseReleased();
	void OnRightMousePressed();
	void OnRightMouseReleased();
	void WalkTowardsCamera();
	void PlayAutoAttackSound(AFibulaCharacter *Caster, AFibulaCharacter *Target);
	UPROPERTY()
	USoundBase *BowAndArrowSound;

	UPROPERTY()
	TArray<USoundBase *> SwordHitSounds;

	
	TMap<EVocation, USkeletalMesh *> TeamAMeshes;
	TMap<EVocation, USkeletalMesh *> TeamBMeshes;

	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentMesh)
	USkeletalMesh *CurrentMesh;

	UFUNCTION()
	void OnRep_CurrentMesh();

	UFUNCTION(Client, Reliable)
	void ClientPinCameraAtDeathLocation(const FVector &Location, AFibulaCharacter *Killer, float Duration);
	void ClientRestoreCamera();

	UPROPERTY()
	FVector SavedCameraLocation;

	UPROPERTY()
	bool bIsCameraPinned;

	FTimerHandle CameraRestoreTimer;

	UPROPERTY()
	AActor *DeathCameraActor;

	UPROPERTY()
	float SavedCameraBoomTargetArmLength;

	UPROPERTY()
	FRotator SavedCameraBoomRotation;

	UPROPERTY()
	bool SavedCameraBoomLag;

	UPROPERTY()
	float SavedCameraBoomLagSpeed;
};
