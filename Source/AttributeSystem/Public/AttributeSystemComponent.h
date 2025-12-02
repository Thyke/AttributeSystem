// Copyright (C) Thyke. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AttributeDefinition.h"
#include "AttributeRuntimeData.h"
#include "AttributeSystemComponent.generated.h"

// Forward declarations
class UAttributeDefinition;
struct FAttributeRuntimeData;

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, FGameplayTag, AttributeTag, float, OldValue, float, NewValue, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeAdded, FGameplayTag, AttributeTag, const UAttributeDefinition*, Definition, float, InitialValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeRemoved, FGameplayTag, AttributeTag, float, FinalValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeRegenerated, FGameplayTag, AttributeTag, float, RegenAmount, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnModifierAdded, FGameplayTag, AttributeTag, FGameplayTag, ModifierTag, float, ModifierValue, UObject*, Source);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnModifierRemoved, FGameplayTag, AttributeTag, FGameplayTag, ModifierTag, UObject*, Source);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeReachedZero, FGameplayTag, AttributeTag, float, OldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeReachedMax, FGameplayTag, AttributeTag, float, NewValue);

/**
 * Core Attribute System Component
 * Fully modular, production-ready attribute management system
 * - GameplayTag based
 * - Runtime attribute addition/removal
 * - Modifier system with priorities
 * - Automatic regeneration
 * - Thread-safe operations
 * - Blueprint friendly
 */


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ATTRIBUTESYSTEM_API UAttributeSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeSystemComponent(const FObjectInitializer& ObjectInitializer);
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // ============================
    // DELEGATES
    // ============================
    
    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnAttributeChanged OnAttributeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnAttributeAdded OnAttributeAdded;

    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnAttributeRemoved OnAttributeRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnAttributeRegenerated OnAttributeRegenerated;

    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnModifierAdded OnModifierAdded;

    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnModifierRemoved OnModifierRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnAttributeReachedZero OnAttributeReachedZero;

    UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
    FOnAttributeReachedMax OnAttributeReachedMax;

    // ============================
    // INITIALIZATION
    // ============================

    /** Initialize component with a set of attribute definitions */
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void InitializeAttributes(const TArray<UAttributeDefinition*>& AttributeDefinitions);

    /** Initialize from a data table containing attribute definitions */
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void InitializeFromDataTable(UDataTable* DataTable);

    // ============================
    // RUNTIME ATTRIBUTE MANAGEMENT
    // ============================

    /** Add a new attribute at runtime */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Runtime")
    bool AddAttribute(UAttributeDefinition* AttributeDefinition, float InitialCurrentValue = -1.0f);

    /** Remove an attribute at runtime */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Runtime")
    bool RemoveAttribute(FGameplayTag AttributeTag);

    /** Check if attribute exists */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    bool HasAttribute(FGameplayTag AttributeTag) const;

    /** Get all active attribute tags */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    TArray<FGameplayTag> GetAllAttributeTags() const;

    /** Get attribute definition */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    const UAttributeDefinition* GetAttributeDefinition(FGameplayTag AttributeTag) const;

    // ============================
    // VALUE GETTERS
    // ============================

    /** Get current value of an attribute */
    UFUNCTION(BlueprintPure, Category = "Attributes|Values")
    float GetAttributeCurrentValue(FGameplayTag AttributeTag) const;

    /** Get base value of an attribute */
    UFUNCTION(BlueprintPure, Category = "Attributes|Values")
    float GetAttributeBaseValue(FGameplayTag AttributeTag) const;

    /** Get final calculated value (with all modifiers) */
    UFUNCTION(BlueprintPure, Category = "Attributes|Values")
    float GetAttributeFinalValue(FGameplayTag AttributeTag);

    /** Get max value (for resource attributes) */
    UFUNCTION(BlueprintPure, Category = "Attributes|Values")
    float GetAttributeMaxValue(FGameplayTag AttributeTag);

    /** Get attribute as percentage (Current / Max) */
    UFUNCTION(BlueprintPure, Category = "Attributes|Values")
    float GetAttributePercent(FGameplayTag AttributeTag);

    /** Get multiple attribute values at once (performance optimized) */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Values")
    TMap<FGameplayTag, float> GetMultipleAttributeValues(const TArray<FGameplayTag>& AttributeTags);

    // ============================
    // VALUE SETTERS
    // ============================

    /** Set current value directly */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modification")
    bool SetAttributeCurrentValue(FGameplayTag AttributeTag, float NewValue, bool bClamp = true);

    /** Set base value */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modification")
    bool SetAttributeBaseValue(FGameplayTag AttributeTag, float NewBaseValue);

    /** Modify current value by delta */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modification")
    float ModifyAttributeCurrentValue(FGameplayTag AttributeTag, float Delta, bool bClamp = true);

    /** Set attribute to its maximum value */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modification")
    bool SetAttributeToMax(FGameplayTag AttributeTag);

    /** Set attribute to its minimum value */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modification")
    bool SetAttributeToMin(FGameplayTag AttributeTag);

    // ============================
    // MODIFIER SYSTEM
    // ============================

    /** Add a modifier to an attribute */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
    bool AddModifier(FGameplayTag AttributeTag, const FAttributeModifier& Modifier);

    /** Remove a specific modifier */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
    bool RemoveModifier(FGameplayTag AttributeTag, FGameplayTag ModifierTag, UObject* Source = nullptr);

    /** Remove all modifiers from an attribute */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
    bool RemoveAllModifiers(FGameplayTag AttributeTag);

    /** Remove all modifiers with a specific tag */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
    int32 RemoveModifiersByTag(FGameplayTag ModifierTag);

    /** Remove all modifiers from a specific source */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
    int32 RemoveModifiersBySource(UObject* Source);

    /** Get all modifiers for an attribute */
    UFUNCTION(BlueprintPure, Category = "Attributes|Modifiers")
    TArray<FAttributeModifier> GetAttributeModifiers(FGameplayTag AttributeTag) const;

    /** Check if attribute has a specific modifier */
    UFUNCTION(BlueprintPure, Category = "Attributes|Modifiers")
    bool HasModifier(FGameplayTag AttributeTag, FGameplayTag ModifierTag) const;

    /** Get total modifier value for an attribute */
    UFUNCTION(BlueprintPure, Category = "Attributes|Modifiers")
    float GetTotalModifierValue(FGameplayTag AttributeTag, EAttributeModifierOperation Operation);

    // ============================
    // REGENERATION SYSTEM
    // ============================

    /** Enable/disable regeneration for an attribute */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Regeneration")
    void SetAttributeRegenEnabled(FGameplayTag AttributeTag, bool bEnabled);

    /** Check if regeneration is enabled */
    UFUNCTION(BlueprintPure, Category = "Attributes|Regeneration")
    bool IsAttributeRegenEnabled(FGameplayTag AttributeTag) const;

    /** Force regeneration tick for an attribute */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Regeneration")
    void ForceRegenTick(FGameplayTag AttributeTag);

    /** Set custom regen rate at runtime */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Regeneration")
    void SetAttributeRegenRate(FGameplayTag AttributeTag, float NewRegenRate);

    /** Pause regeneration temporarily */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Regeneration")
    void PauseRegeneration(FGameplayTag AttributeTag, float Duration);

    // ============================
    // BATCH OPERATIONS (Performance Optimized)
    // ============================

    /** Modify multiple attributes at once */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Batch")
    void ModifyMultipleAttributes(const TMap<FGameplayTag, float>& AttributeDeltas);

    /** Add multiple modifiers at once */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Batch")
    void AddMultipleModifiers(const TMap<FGameplayTag, FAttributeModifier>& Modifiers);

    // ============================
    // QUERY FUNCTIONS
    // ============================

    /** Check if attribute is at max */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    bool IsAttributeAtMax(FGameplayTag AttributeTag) const;

    /** Check if attribute is at min */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    bool IsAttributeAtMin(FGameplayTag AttributeTag) const;

    /** Check if attribute is at zero */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    bool IsAttributeAtZero(FGameplayTag AttributeTag) const;

    /** Get attributes by tag query */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    TArray<FGameplayTag> GetAttributesByTagQuery(const FGameplayTagQuery& Query) const;

    /** Get all attributes below a threshold */
    UFUNCTION(BlueprintPure, Category = "Attributes|Query")
    TArray<FGameplayTag> GetAttributesBelowPercent(float Threshold) const;

    // ============================
    // DEBUG & UTILITY
    // ============================

    /** Print all attributes to log */
    UFUNCTION(BlueprintCallable, Category = "Attributes|Debug")
    void DebugPrintAllAttributes() const;

    /** Get debug string for an attribute */
    UFUNCTION(BlueprintPure, Category = "Attributes|Debug")
    FString GetAttributeDebugString(FGameplayTag AttributeTag) const;

    /** Enable/disable debug mode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Debug")
    bool bGlobalDebugMode = false;

protected:
    // ============================
    // INTERNAL DATA
    // ============================

    /** Runtime attribute data - using TMap for fast lookup by tag */
    UPROPERTY()
    TMap<FGameplayTag, FAttributeRuntimeData> Attributes;

    /** Critical section for thread-safe operations */
    mutable FCriticalSection AttributeMutex;

    /** Attributes that should be initialized on BeginPlay */
    UPROPERTY(EditAnywhere, Category = "Attributes|Initialization")
    TArray<UAttributeDefinition*> DefaultAttributes;

    /** Should tick be enabled for regeneration? */
    UPROPERTY(EditAnywhere, Category = "Attributes|Performance")
    bool bEnableRegeneration = true;

    /** Tick interval for regeneration (0 = every frame) */
    UPROPERTY(EditAnywhere, Category = "Attributes|Performance", meta = (ClampMin = "0.0"))
    float RegenTickInterval = 0.1f;

    /** Attributes with paused regeneration */
    TMap<FGameplayTag, float> PausedRegenAttributes;

    // ============================
    // INTERNAL FUNCTIONS
    // ============================

    /** Calculate final value with all modifiers */
    float CalculateFinalValue(FAttributeRuntimeData& AttributeData);

    /** Apply clamping to value */
    float ClampAttributeValue(const FAttributeRuntimeData& AttributeData, float Value) const;

    /** Process regeneration for an attribute */
    void ProcessRegeneration(FGameplayTag AttributeTag, FAttributeRuntimeData& AttributeData, float DeltaTime);

    /** Handle modifier expiration */
    void UpdateModifierDurations(float DeltaTime);

    /** Notify about attribute change */
    void BroadcastAttributeChange(FGameplayTag AttributeTag, float OldValue, float NewValue);

    /** Get or create attribute data */
    FAttributeRuntimeData* GetAttributeData(FGameplayTag AttributeTag);
    const FAttributeRuntimeData* GetAttributeData(FGameplayTag AttributeTag) const;

    /** Thread-safe attribute access */
    template<typename Func>
    auto ExecuteAttributeOperation(Func Operation) const -> decltype(Operation())
    {
        FScopeLock Lock(&AttributeMutex);
        return Operation();
    }
    
    /** Helper function to remove modifier via timer (void return for timer delegate) */
    void RemoveModifierViaTimer(FGameplayTag AttributeTag, FGameplayTag ModifierTag, UObject* Source);
};