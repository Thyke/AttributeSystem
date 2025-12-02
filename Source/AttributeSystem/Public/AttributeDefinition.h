// Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeDefinition.generated.h"

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
    Flat UMETA(DisplayName = "Flat Value"),
    Percentage UMETA(DisplayName = "Percentage"),
    Resource UMETA(DisplayName = "Resource (Current/Max)")
};

UENUM(BlueprintType)
enum class EAttributeRegenType : uint8
{
    None UMETA(DisplayName = "No Regeneration"),
    PerSecond UMETA(DisplayName = "Per Second"),
    PerTick UMETA(DisplayName = "Per Tick"),
    Delayed UMETA(DisplayName = "Delayed (After Damage)")
};

USTRUCT(BlueprintType)
struct FAttributeRegenConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regeneration")
    EAttributeRegenType RegenType = EAttributeRegenType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regeneration", meta = (EditCondition = "RegenType != EAttributeRegenType::None"))
    float RegenRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regeneration", meta = (EditCondition = "RegenType == EAttributeRegenType::Delayed"))
    float RegenDelay = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regeneration")
    bool bRegenToMax = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regeneration", meta = (EditCondition = "!bRegenToMax"))
    float RegenTargetValue = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regeneration")
    bool bPauseRegenOnDamage = false;
};

USTRUCT(BlueprintType)
struct FAttributeClampConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clamping")
    bool bUseMinClamp = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clamping", meta = (EditCondition = "bUseMinClamp"))
    float MinValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clamping")
    bool bUseMaxClamp = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clamping", meta = (EditCondition = "bUseMaxClamp"))
    float MaxValue = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clamping")
    bool bClampToAnotherAttribute = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clamping", meta = (EditCondition = "bClampToAnotherAttribute"))
    FGameplayTag ClampTargetAttributeTag;
};

/**
 * Primary Data Asset defining an attribute's properties and behavior
 * Fully modular and reusable across any project
 */
UCLASS()
class ATTRIBUTESYSTEM_API UAttributeDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
    UAttributeDefinition();

    // Unique identifier for this attribute
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
    FGameplayTag AttributeTag;

    // Display name for UI
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
    FText DisplayName;

    // Description for tooltips
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute", meta = (MultiLine = true))
    FText Description;

    // Icon for UI representation
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
    UTexture2D* Icon;

    // Type of attribute
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
    EAttributeType AttributeType = EAttributeType::Resource;

    // Default base value
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
    float DefaultBaseValue = 100.0f;

    // Default current value (for resource types)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute", meta = (EditCondition = "AttributeType == EAttributeType::Resource"))
    float DefaultCurrentValue = 100.0f;

    // Clamping configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
    FAttributeClampConfig ClampConfig;

    // Regeneration configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration")
    FAttributeRegenConfig RegenConfig;

    // Tags that can modify this attribute (for query optimization)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Tags")
    FGameplayTagContainer ModifierTags;

    // Tags this attribute grants to the owner
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Tags")
    FGameplayTagContainer GrantedTags;
    
    // Should changes be logged for debugging?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Debug")
    bool bDebugMode = false;

    // Custom color for debug visualization
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Debug")
    FLinearColor DebugColor = FLinearColor::White;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("AttributeDefinition", GetFName());
    }
};