#include "DeadCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

ADeadCharacter::ADeadCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);

    CorpseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CorpseMesh"));
    RootComponent = CorpseMesh;

    
    CorpseMesh->SetSimulatePhysics(true);
    CorpseMesh->SetEnableGravity(true);
    CorpseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CorpseMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

    
    CorpseMesh->SetMassScale(NAME_None, 1000.0f);

    
    CorpseMesh->SetAngularDamping(10.0f);
    CorpseMesh->SetLinearDamping(10.0f);

    if (FBodyInstance *BodyInstance = CorpseMesh->GetBodyInstance())
    {
        
        BodyInstance->bLockXRotation = true;
        BodyInstance->bLockYRotation = true;
        BodyInstance->bLockZRotation = true;

        
        BodyInstance->SetMassOverride(2000.0f); 
        BodyInstance->LinearDamping = 10.0f;
        BodyInstance->AngularDamping = 10.0f;

        
        BodyInstance->SetMaxDepenetrationVelocity(0.0f);

        
        BodyInstance->bUseCCD = true;
    }

    
    FCollisionResponseContainer ResponseContainer;
    ResponseContainer.SetAllChannels(ECR_Block);
    ResponseContainer.SetResponse(ECC_Camera, ECR_Ignore);
    ResponseContainer.SetResponse(ECC_Pawn, ECR_Ignore);
    ResponseContainer.SetResponse(ECC_Visibility, ECR_Block);
    CorpseMesh->SetCollisionResponseToChannels(ResponseContainer);
    CorpseMesh->SetIsReplicated(true);
}

void ADeadCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADeadCharacter, ReplicatedMesh);
}

void ADeadCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this, &ADeadCharacter::DestroyCorpse, 180.0f, false);
    }
}

void ADeadCharacter::DestroyCorpse()
{
    Destroy();
}

void ADeadCharacter::SetCorpseMesh(USkeletalMesh *Mesh)
{
    if (HasAuthority() && Mesh)
    {
        ReplicatedMesh = Mesh;
        OnRep_CorpseMesh();
    }
}

void ADeadCharacter::OnRep_CorpseMesh()
{
    if (CorpseMesh && ReplicatedMesh)
    {
        CorpseMesh->SetSkeletalMesh(ReplicatedMesh);
        CorpseMesh->SetRelativeScale3D(FVector(0.6f));
    }
}