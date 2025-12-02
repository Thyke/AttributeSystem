// Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AttributeDefinition.h"
#include "AttributeRuntimeData.generated.h"

UENUM(BlueprintType)
enum class EAttributeModifierOperation : uint8
{
    Add UMETA(DisplayName = "Add"),
    Multiply UMETA(DisplayName = "Multiply"),
    Divide UMETA(DisplayName = "Divide"),
    Override UMETA(DisplayName = "Override")
};

USTRUCT(BlueprintType)
struct FAttributeModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
    FGameplayTag ModifierTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
    EAttributeModifierOperation Operation = EAttributeModifierOperation::Add;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
    float Value = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
    int32 Priority = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
    bool bIsTemporary = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier", meta = (EditCondition = "bIsTemporary"))
    float Duration = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Modifier")
    UObject* Source = nullptr;

    float RemainingDuration = 0.0f;
    mutable FTimerHandle TimerHandle;

    FAttributeModifier()
        : Operation(EAttributeModifierOperation::Add)
        , Value(0.0f)
        , Priority(0)
        , bIsTemporary(false)
        , Duration(0.0f)
        , Source(nullptr)
        , RemainingDuration(0.0f)
    {}

    bool operator<(const FAttributeModifier& Other) const
    {
        return Priority > Other.Priority; // Higher priority first
    }
};

USTRUCT(BlueprintType)
struct FAttributeRuntimeData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Attribute")
    const UAttributeDefinition* Definition = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Attribute")
    float BaseValue = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Attribute")
    float CurrentValue = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Attribute")
    float CachedFinalValue = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Attribute")
    TArray<FAttributeModifier> Modifiers;

    // Regen tracking
    float LastRegenTime = 0.0f;
    float LastDamageTime = 0.0f;
    bool bIsDirty = true;

    FAttributeRuntimeData()
        : Definition(nullptr)
        , BaseValue(0.0f)
        , CurrentValue(0.0f)
        , CachedFinalValue(0.0f)
        , LastRegenTime(0.0f)
        , LastDamageTime(0.0f)
        , bIsDirty(true)
    {}

    void MarkDirty()
    {
        bIsDirty = true;
    }

    bool NeedsRecalculation() const
    {
        return bIsDirty;
    }
};