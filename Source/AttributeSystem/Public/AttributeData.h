// // Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeData.generated.h"


USTRUCT(BlueprintType)
struct FAttribute
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute", meta = (GameplayTagFilter = "Attributes"))
    FGameplayTag AttributeTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
    float Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
    float MinValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
    float MaxValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
    bool bUseRegen;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
    float RegenRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
    float RegenValue;

    FAttribute()
        : AttributeTag(), Value(0.0f), MinValue(0.0f), MaxValue(100.0f), bUseRegen(false), RegenRate(0.0f), RegenValue(0.0f)
    {}

    FAttribute(FGameplayTag InAttributeTag, float InValue, float InMinValue, float InMaxValue, bool InUseRegen = false, float InRegenRate = 0.0f, float InRegenValue = 0.0f)
        : AttributeTag(InAttributeTag), Value(InValue), MinValue(InMinValue), MaxValue(InMaxValue), bUseRegen(InUseRegen), RegenRate(InRegenRate), RegenValue(InRegenValue)
    {}
};


UCLASS(BlueprintType, Const, DisplayName = "AttributeSystem Attribute Set")
class ATTRIBUTESYSTEM_API UAttributeData : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSystem|Attributes")
    TArray<FAttribute> Attributes;
};
