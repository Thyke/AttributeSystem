// // Copyright (C) Thyke. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AttributeData.h"
#include "AttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, FGameplayTag, AttributeTag, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeAdded, FGameplayTag, AttributeTag, float, InitialValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeRemoved, FGameplayTag, AttributeTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeThresholdReached, FGameplayTag, AttributeTag, float, CurrentValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPreAttributeChanged, FGameplayTag, AttributeTag, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitializeAttributes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentValue, float, NewValue);

UENUM(BlueprintType)
enum class EAttributeOperation : uint8
{
    Add UMETA(DisplayName = "Add"),
    Subtract UMETA(DisplayName = "Subtract"),
    Multiply UMETA(DisplayName = "Multiply"),
    Divide UMETA(DisplayName = "Divide"),
    Override UMETA(DisplayName = "Override")
};

USTRUCT(BlueprintType)
struct FAttributeModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSystem|Attributes")
    FGameplayTag AttributeTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSystem|Attributes")
    float Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSystem|Attributes")
    float Duration; // -1 for permanent, >0 for temporary

    FAttributeModifier()
        : AttributeTag(), Value(0.0f), Duration(-1.0f) {}

    FAttributeModifier(FGameplayTag InAttributeTag, float InValue, float InDuration = -1.0f)
        : AttributeTag(InAttributeTag), Value(InValue), Duration(InDuration) {}
};

UCLASS(ClassGroup = (Custom), DisplayName = "AttributeComponent", meta = (BlueprintSpawnableComponent))
class ATTRIBUTESYSTEM_API UAttributeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttributeComponent(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSystem|Attributes")
    class UAttributeData* AttributeDataAsset;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void LoadAttributesFromDataAsset();

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    float GetAttributeValue(const FGameplayTag& AttributeTag) const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void SetAttributeValue(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation = EAttributeOperation::Override);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void DecreaseAttributeValue(const FGameplayTag& AttributeTag, float Amount);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void IncreaseAttributeValue(const FGameplayTag& AttributeTag, float Amount);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void SetAttributeMinValue(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation = EAttributeOperation::Override);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void SetAttributeMaxValue(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation = EAttributeOperation::Override);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void ResetAllAttributes();

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    FAttribute GetAttribute(const FGameplayTag& AttributeTag) const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    TArray<FAttribute> GetAllAttributes() const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void GetAttributeValues(const FGameplayTag& AttributeTag, float& OutValue, float& OutMin, float& OutMax) const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    float GetAttributeNormalized(const FGameplayTag& AttributeTag) const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    TMap<FGameplayTag, FAttribute> GetAttributeMap() const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void SetUseRegen(const FGameplayTag& AttributeTag, bool bUseRegen);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    bool IsValidAttribute(const FGameplayTag& AttributeTag) const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void AddAttribute(const FAttribute& NewAttribute);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void RemoveAttribute(const FGameplayTag& AttributeTag);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void CheckThresholds(const FGameplayTag& AttributeTag);

    UPROPERTY(BlueprintReadWrite, Category = "AttributeSystem|Attributes")
    TArray<FAttributeModifier> ActiveModifiers;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void ApplyModifier(const FAttributeModifier& Modifier);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void RemoveModifier(const FGameplayTag& AttributeTag, float ModifierValue);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void DecreaseHealth(float Amount);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void IncreaseHealth(float Amount);

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnAttributeChanged OnAttributeChanged;

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnAttributeAdded OnAttributeAdded;

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnAttributeRemoved OnAttributeRemoved;

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnAttributeThresholdReached OnAttributeThresholdReached;

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnPreAttributeChanged OnPreAttributeChanged;

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnInitializeAttributes OnInitializeAttributes;

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnDeath OnDeath;

    UPROPERTY(BlueprintAssignable, Category = "AttributeSystem|Attributes")
    FOnHealthChanged OnHealthChanged;

private:
    TArray<FAttribute> InitialAttributesBackup;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "AttributeSystem|Attributes", meta = (AllowPrivateAccess = "true"))
    TArray<FAttribute> Attributes;

    TMap<FGameplayTag, FAttribute> AttributeMap;

    void InitializeAttributeMap();
    void CheckRegenStatus(const FGameplayTag& AttributeTag);
    void StartRegenTimer();
    void RegenerateAttributes();

    TMap<FGameplayTag, FTimerHandle> RegenTimers;

public:

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void GetHealthAttributeValues(float& OutValue, float& OutMin, float& OutMax) const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    float GetHealthNormalized() const;

private:

    void SetAttributeValueInternal(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation);

public:
    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void SaveAttributes(FString SlotName = "AttributeSaveSlot", int32 Index = 0);

    UFUNCTION(BlueprintCallable, Category = "AttributeSystem|Attributes")
    void LoadAttributes(FString SlotName = "AttributeSaveSlot", int32 Index = 0);

private:
    FString DefaultSaveSlotName;
    uint32 DefaultUserIndex;
};
