#include "TeamStash.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"

ATeamStash::ATeamStash()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    
    ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
    RootComponent = ChestMesh; 

    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Fantastic_Village_Pack/meshes/props/container/SM_PROP_box_01.SM_PROP_box_01"));
    if (MeshAsset.Succeeded())
    {
        ChestMesh->SetStaticMesh(MeshAsset.Object);

        
        ChestMesh->SetMobility(EComponentMobility::Static);
        ChestMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        ChestMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

        
        FCollisionResponseContainer ResponseContainer;
        ResponseContainer.SetAllChannels(ECR_Block);
        ResponseContainer.SetResponse(ECC_Camera, ECR_Ignore);
        ResponseContainer.SetResponse(ECC_Pawn, ECR_Block);
        ResponseContainer.SetResponse(ECC_Visibility, ECR_Block);
        ChestMesh->SetCollisionResponseToChannels(ResponseContainer);
    }
}