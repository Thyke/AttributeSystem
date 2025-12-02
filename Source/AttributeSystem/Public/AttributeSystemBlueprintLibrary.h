// Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AttributeSystemBlueprintLibrary.generated.h"

/**
 * Blueprint Function Library for Attribute System
 * Provides convenient helper functions for Blueprint usage
 */
UCLASS()
class ATTRIBUTESYSTEM_API UAttributeSystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    // ============================
    // COMPONENT ACCESS
    // ============================

    /** Get attribute system component from actor */
    UFUNCTION(BlueprintPure, Category = "Attributes|Utility", meta = (DefaultToSelf = "Actor"))
    static UAttributeSystemComponent* GetAttributeSystemComponent(AActor* Actor);

    /** Get or add attribute system component to actor */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Utility", meta = (DefaultToSelf = "Actor"))
    static UAttributeSystemComponent* GetOrCreateAttributeSystemComponent(AActor* Actor);

    // ============================
    // QUICK ACCESS FUNCTIONS
    // ============================

    /** Get attribute current value from actor */
    UFUNCTION(BlueprintPure, Category = "Attributes|Quick Access", meta = (DefaultToSelf = "Actor"))
    static float GetActorAttributeValue(AActor* Actor, FGameplayTag AttributeTag);

    /** Set attribute current value on actor */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Quick Access", meta = (DefaultToSelf = "Actor"))
    static bool SetActorAttributeValue(AActor* Actor, FGameplayTag AttributeTag, float NewValue, bool bClamp = true);

    /** Modify attribute on actor */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Quick Access", meta = (DefaultToSelf = "Actor"))
    static float ModifyActorAttribute(AActor* Actor, FGameplayTag AttributeTag, float Delta, bool bClamp = true);

    /** Get attribute percent from actor */
    UFUNCTION(BlueprintPure, Category = "Attributes|Quick Access", meta = (DefaultToSelf = "Actor"))
    static float GetActorAttributePercent(AActor* Actor, FGameplayTag AttributeTag);

    // ============================
    // MODIFIER HELPERS
    // ============================

    /** Create a simple additive modifier */
    UFUNCTION(BlueprintPure, Category = "Attributes|Modifiers")
    static FAttributeModifier MakeAdditiveModifier(FGameplayTag ModifierTag, float Value, int32 Priority = 0);

    /** Create a simple multiplicative modifier */
    UFUNCTION(BlueprintPure, Category = "Attributes|Modifiers")
    static FAttributeModifier MakeMultiplicativeModifier(FGameplayTag ModifierTag, float Multiplier, int32 Priority = 0);

    /** Create a temporary modifier with duration */
    UFUNCTION(BlueprintPure, Category = "Attributes|Modifiers")
    static FAttributeModifier MakeTemporaryModifier(FGameplayTag ModifierTag, EAttributeModifierOperation Operation, 
        float Value, float Duration, int32 Priority = 0);

    /** Create a percentage-based modifier (converts to multiplier) */
    UFUNCTION(BlueprintPure, Category = "Attributes|Modifiers")
    static FAttributeModifier MakePercentageModifier(FGameplayTag ModifierTag, float Percentage, int32 Priority = 0);

    // ============================
    // BATCH OPERATIONS
    // ============================

    /** Apply damage to multiple attributes at once */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Batch", meta = (DefaultToSelf = "Actor"))
    static void ApplyDamageToAttributes(AActor* Actor, const TMap<FGameplayTag, float>& DamageMap);

    /** Heal multiple attributes at once */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Batch", meta = (DefaultToSelf = "Actor"))
    static void HealMultipleAttributes(AActor* Actor, const TMap<FGameplayTag, float>& HealMap);

    /** Set multiple attributes to max */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Batch", meta = (DefaultToSelf = "Actor"))
    static void SetMultipleAttributesToMax(AActor* Actor, const TArray<FGameplayTag>& AttributeTags);

    // ============================
    // COMPARISON FUNCTIONS
    // ============================

    /** Compare attribute values between two actors */
    UFUNCTION(BlueprintPure, Category = "Attributes|Comparison")
    static float CompareAttributeValues(AActor* ActorA, AActor* ActorB, FGameplayTag AttributeTag);

    /** Get actor with highest attribute value */
    UFUNCTION(BlueprintPure, Category = "Attributes|Comparison")
    static AActor* GetActorWithHighestAttribute(const TArray<AActor*>& Actors, FGameplayTag AttributeTag);

    /** Get actor with lowest attribute value */
    UFUNCTION(BlueprintPure, Category = "Attributes|Comparison")
    static AActor* GetActorWithLowestAttribute(const TArray<AActor*>& Actors, FGameplayTag AttributeTag);

    // ============================
    // UTILITY FUNCTIONS
    // ============================

    /** Interpolate attribute value towards target */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Utility", meta = (DefaultToSelf = "Actor"))
    static float InterpAttributeToTarget(AActor* Actor, FGameplayTag AttributeTag, float TargetValue, 
        float DeltaTime, float InterpSpeed);

    /** Transfer attribute value from one actor to another */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Utility")
    static float TransferAttributeValue(AActor* FromActor, AActor* ToActor, FGameplayTag AttributeTag, 
        float Amount, bool bClamp = true);

    /** Copy all attributes from one actor to another */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Utility")
    static void CopyAttributesToActor(AActor* SourceActor, AActor* TargetActor);

    // ============================
    // VALIDATION & CHECKS
    // ============================

    /** Check if actor can afford attribute cost */
    UFUNCTION(BlueprintPure, Category = "Attributes|Validation", meta = (DefaultToSelf = "Actor"))
    static bool CanAffordAttributeCost(AActor* Actor, const TMap<FGameplayTag, float>& Costs);

    /** Check if attribute is within range */
    UFUNCTION(BlueprintPure, Category = "Attributes|Validation")
    static bool IsAttributeInRange(float AttributeValue, float MinValue, float MaxValue, bool bInclusive = true);

    /** Get missing amount to reach target value */
    UFUNCTION(BlueprintPure, Category = "Attributes|Validation", meta = (DefaultToSelf = "Actor"))
    static float GetMissingAttributeAmount(AActor* Actor, FGameplayTag AttributeTag, float TargetValue);

    // ============================
    // FORMATTING & DISPLAY
    // ============================

    /** Format attribute value for display */
    UFUNCTION(BlueprintPure, Category = "Attributes|Display")
    static FText FormatAttributeValue(float Value, int32 DecimalPlaces = 0);

    /** Format attribute as percentage */
    UFUNCTION(BlueprintPure, Category = "Attributes|Display")
    static FText FormatAttributePercent(float Percent, int32 DecimalPlaces = 0);

    /** Get attribute display text with current/max format */
    UFUNCTION(BlueprintPure, Category = "Attributes|Display", meta = (DefaultToSelf = "Actor"))
    static FText GetAttributeDisplayText(AActor* Actor, FGameplayTag AttributeTag);

    // ============================
    // COLOR HELPERS (for UI)
    // ============================

    /** Get color based on attribute percentage (red -> yellow -> green) */
    UFUNCTION(BlueprintPure, Category = "Attributes|Display")
    static FLinearColor GetAttributeColorByPercent(float Percent);

    /** Get custom color gradient based on percentage */
    UFUNCTION(BlueprintPure, Category = "Attributes|Display")
    static FLinearColor GetColorGradient(float Percent, FLinearColor LowColor, FLinearColor MidColor, FLinearColor HighColor);

    // ============================
    // SAVE/LOAD HELPERS
    // ============================

    /** Serialize attributes to string (for save games) */
    UFUNCTION(BlueprintCallable, Category = "Attributes|SaveLoad", meta = (DefaultToSelf = "Actor"))
    static FString SerializeAttributes(AActor* Actor);

    /** Deserialize attributes from string */
    UFUNCTION(BlueprintCallable, Category = "Attributes|SaveLoad", meta = (DefaultToSelf = "Actor"))
    static bool DeserializeAttributes(AActor* Actor, const FString& SerializedData);

    // ============================
    // GAMEPLAY TAG HELPERS
    // ============================

    /** Make attribute tag from string */
    UFUNCTION(BlueprintPure, Category = "Attributes|Tags")
    static FGameplayTag MakeAttributeTag(const FString& TagString);

    /** Check if tag is valid attribute tag */
    UFUNCTION(BlueprintPure, Category = "Attributes|Tags")
    static bool IsValidAttributeTag(FGameplayTag Tag);
};