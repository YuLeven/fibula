#include "GameItem.h"

bool FGameItem::NetSerialize(FArchive &Ar, UPackageMap *Map, bool &bOutSuccess)
{
    bOutSuccess = true;

    
    Ar << Name;
    Ar << Description;
    Ar << Weight;
    Ar << bIsStackable;
    Ar << StackCount;

    
    uint8 RawItemType = static_cast<uint8>(ItemType);
    Ar << RawItemType;
    ItemType = static_cast<EItemType>(RawItemType);

    uint8 RawUseType = static_cast<uint8>(UseType);
    Ar << RawUseType;
    UseType = static_cast<EItemUseType>(RawUseType);

    Ar << UseAction;

    
    FString IconPath = Icon.ToSoftObjectPath().ToString();
    Ar << IconPath;
    if (Ar.IsLoading())
    {
        Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(IconPath));
    }

    return true;
}