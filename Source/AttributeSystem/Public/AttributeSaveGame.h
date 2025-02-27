// Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "AttributeSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ATTRIBUTESYSTEM_API UAttributeSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
    TMap<FGameplayTag, float> SavedAttributes;
};
