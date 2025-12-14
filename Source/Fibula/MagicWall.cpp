#include "MagicWall.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "FibulaCharacter.h"
#include "TimerManager.h"

AMagicWall::AMagicWall()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    
    WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
    RootComponent = WallMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/SM_Stone_Big_Var03.SM_Stone_Big_Var03"));
    if (MeshAsset.Succeeded())
    {
        WallMesh->SetStaticMesh(MeshAsset.Object);
        
        WallMesh->SetCollisionProfileName(TEXT("MagicWall"));
        WallMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        WallMesh->SetGenerateOverlapEvents(true);
        WallMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    }

    
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(RootComponent);
    CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 100.0f));
    CollisionBox->SetCollisionProfileName(TEXT("MagicWall"));
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionBox->SetGenerateOverlapEvents(true);
    CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

void AMagicWall::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        
        GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this, &AMagicWall::DestroyWall, 20.0f, false);
    }

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMagicWall::OnOverlapBegin);
}

void AMagicWall::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                                UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (!HasAuthority())
        return;

    if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(OtherActor))
    {
        
        if (OverlappedComponent == WallMesh)
        {
            DestroyWall();
        }
    }
}

void AMagicWall::DestroyWall()
{
    Destroy();
}