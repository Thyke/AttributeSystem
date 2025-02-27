// // Copyright (C) Thyke. All Rights Reserved.


#include "AttributeComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "AttributeData.h"
#include "AttributeSaveGame.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttributeComponent)

UAttributeComponent::UAttributeComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer), DefaultSaveSlotName(TEXT("AttributeSaveSlot")), DefaultUserIndex(0)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::LoadAttributesFromDataAsset()
{
    if (AttributeDataAsset)
    {
        InitialAttributesBackup = AttributeDataAsset->Attributes;
        Attributes = AttributeDataAsset->Attributes;

        InitializeAttributeMap();

        for (const FAttribute& Attribute : Attributes)
        {
            OnAttributeChanged.Broadcast(Attribute.AttributeTag, Attribute.Value);
            CheckRegenStatus(Attribute.AttributeTag);
            UE_LOG(LogTemp, Warning, TEXT("Loaded Attribute from DataAsset - Tag: %s, Value: %f, Min: %f, Max: %f"),
                *Attribute.AttributeTag.ToString(), Attribute.Value, Attribute.MinValue, Attribute.MaxValue);
        }

        OnInitializeAttributes.Broadcast();

        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::LoadAttributesFromDataAsset - Attributes loaded from DataAsset"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::LoadAttributesFromDataAsset - No DataAsset assigned"));
    }
}

void UAttributeComponent::InitializeAttributeMap()
{
    AttributeMap.Empty();
    for (FAttribute& Attribute : Attributes)
    {
        AttributeMap.Add(Attribute.AttributeTag, Attribute);
    }
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::InitializeAttributeMap"));
}


void UAttributeComponent::BeginPlay()
{
    Super::BeginPlay();
    LoadAttributesFromDataAsset();
    StartRegenTimer();
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::BeginPlay"));
}

float UAttributeComponent::GetAttributeValue(const FGameplayTag& AttributeTag) const
{
    if (const FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttributeValue - Attribute: %s, Value: %f"), *AttributeTag.ToString(), FoundAttribute->Value);
        return FoundAttribute->Value;
    }
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttributeValue - Attribute not found: %s"), *AttributeTag.ToString());
    return 0.0f;
}

void UAttributeComponent::SetAttributeValue(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation)
{
    SetAttributeValueInternal(AttributeTag, Value, Operation);
}

void UAttributeComponent::DecreaseAttributeValue(const FGameplayTag& AttributeTag, float Amount)
{
    SetAttributeValueInternal(AttributeTag, Amount, EAttributeOperation::Subtract);
}

void UAttributeComponent::IncreaseAttributeValue(const FGameplayTag& AttributeTag, float Amount)
{
    SetAttributeValueInternal(AttributeTag, Amount, EAttributeOperation::Add);
}

void UAttributeComponent::SetAttributeMinValue(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation)
{
    if (FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        float OldValue = FoundAttribute->MinValue;
        float NewValue = OldValue;

        switch (Operation)
        {
        case EAttributeOperation::Add:
            NewValue += Value;
            break;
        case EAttributeOperation::Subtract:
            NewValue -= Value;
            break;
        case EAttributeOperation::Multiply:
            NewValue *= Value;
            break;
        case EAttributeOperation::Divide:
            if (Value != 0)
            {
                NewValue /= Value;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMinValue - Divide by zero error for Attribute: %s"), *AttributeTag.ToString());
            }
            break;
        case EAttributeOperation::Override:
            NewValue = Value;
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMinValue - Invalid operation for Attribute: %s"), *AttributeTag.ToString());
            break;
        }

        FoundAttribute->MinValue = FMath::Clamp(NewValue, 0.0f, FoundAttribute->MaxValue);

        if (OldValue != FoundAttribute->MinValue)
        {
            OnAttributeChanged.Broadcast(AttributeTag, FoundAttribute->MinValue);
            CheckRegenStatus(AttributeTag);
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMinValue - Attribute: %s, Old Min Value: %f, New Min Value: %f, Operation: %d"), *AttributeTag.ToString(), OldValue, FoundAttribute->MinValue, (int32)Operation);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMinValue - Attribute not found: %s"), *AttributeTag.ToString());
    }
}

void UAttributeComponent::SetAttributeMaxValue(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation)
{
    if (FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        float OldValue = FoundAttribute->MaxValue;
        float NewValue = OldValue;

        switch (Operation)
        {
        case EAttributeOperation::Add:
            NewValue += Value;
            break;
        case EAttributeOperation::Subtract:
            NewValue -= Value;
            break;
        case EAttributeOperation::Multiply:
            NewValue *= Value;
            break;
        case EAttributeOperation::Divide:
            if (Value != 0)
            {
                NewValue /= Value;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMaxValue - Divide by zero error for Attribute: %s"), *AttributeTag.ToString());
            }
            break;
        case EAttributeOperation::Override:
            NewValue = Value;
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMaxValue - Invalid operation for Attribute: %s"), *AttributeTag.ToString());
            break;
        }

        FoundAttribute->MaxValue = FMath::Clamp(NewValue, FoundAttribute->MinValue, FLT_MAX);

        if (OldValue != FoundAttribute->MaxValue)
        {
            OnAttributeChanged.Broadcast(AttributeTag, FoundAttribute->MaxValue);
            CheckRegenStatus(AttributeTag);
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMaxValue - Attribute: %s, Old Max Value: %f, New Max Value: %f, Operation: %d"), *AttributeTag.ToString(), OldValue, FoundAttribute->MaxValue, (int32)Operation);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeMaxValue - Attribute not found: %s"), *AttributeTag.ToString());
    }
}


FAttribute UAttributeComponent::GetAttribute(const FGameplayTag& AttributeTag) const
{
    if (const FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttribute - Attribute: %s, Value: %f"), *AttributeTag.ToString(), FoundAttribute->Value);
        return *FoundAttribute;
    }
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttribute - Attribute not found: %s"), *AttributeTag.ToString());
    return FAttribute();
}

TArray<FAttribute> UAttributeComponent::GetAllAttributes() const
{
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAllAttributes"));
    return Attributes;
}

void UAttributeComponent::GetAttributeValues(const FGameplayTag& AttributeTag, float& OutValue, float& OutMin, float& OutMax) const
{
    if (const FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        OutValue = FoundAttribute->Value;
        OutMin = FoundAttribute->MinValue;
        OutMax = FoundAttribute->MaxValue;
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttributeValues - Attribute: %s, Value: %f, Min: %f, Max: %f"), *AttributeTag.ToString(), OutValue, OutMin, OutMax);
    }
    else
    {
        OutValue = 0.0f;
        OutMin = 0.0f;
        OutMax = 0.0f;
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttributeValues - Attribute not found: %s"), *AttributeTag.ToString());
    }
}

float UAttributeComponent::GetAttributeNormalized(const FGameplayTag& AttributeTag) const
{
    if (const FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        if (FoundAttribute->MaxValue != FoundAttribute->MinValue)
        {
            float NormalizedValue = (FoundAttribute->Value - FoundAttribute->MinValue) / (FoundAttribute->MaxValue - FoundAttribute->MinValue);
            //UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttributeNormalized - Attribute: %s, Normalized Value: %f"), *AttributeTag.ToString(), NormalizedValue);
            return NormalizedValue;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttributeNormalized - Attribute not found or invalid range: %s"), *AttributeTag.ToString());
    return 0.0f;
}

TMap<FGameplayTag, FAttribute> UAttributeComponent::GetAttributeMap() const
{
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::GetAttributeMap"));
    return AttributeMap;
}

void UAttributeComponent::SetUseRegen(const FGameplayTag& AttributeTag, bool bUseRegen)
{
    if (FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        FoundAttribute->bUseRegen = bUseRegen;
        CheckRegenStatus(AttributeTag);
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetUseRegen - Attribute: %s, UseRegen: %s"), *AttributeTag.ToString(), bUseRegen ? TEXT("true") : TEXT("false"));
    }
}

bool UAttributeComponent::IsValidAttribute(const FGameplayTag& AttributeTag) const
{
    return AttributeMap.Contains(AttributeTag);
}

void UAttributeComponent::AddAttribute(const FAttribute& NewAttribute)
{
    if (!IsValidAttribute(NewAttribute.AttributeTag))
    {
        Attributes.Add(NewAttribute);
        InitializeAttributeMap();
        OnAttributeChanged.Broadcast(NewAttribute.AttributeTag, NewAttribute.Value);
        OnAttributeAdded.Broadcast(NewAttribute.AttributeTag, NewAttribute.Value);
        CheckRegenStatus(NewAttribute.AttributeTag);
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::AddAttribute - Attribute added: %s"), *NewAttribute.AttributeTag.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::AddAttribute - Attribute already exists: %s"), *NewAttribute.AttributeTag.ToString());
    }
}

void UAttributeComponent::RemoveAttribute(const FGameplayTag& AttributeTag)
{
    if (IsValidAttribute(AttributeTag))
    {
        Attributes.RemoveAll([&](const FAttribute& Attribute) { return Attribute.AttributeTag == AttributeTag; });
        InitializeAttributeMap();
        OnAttributeChanged.Broadcast(AttributeTag, 0.0f);
        OnAttributeRemoved.Broadcast(AttributeTag);
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RemoveAttribute - Attribute removed: %s"), *AttributeTag.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RemoveAttribute - Attribute not found: %s"), *AttributeTag.ToString());
    }
}

void UAttributeComponent::CheckThresholds(const FGameplayTag& AttributeTag)
{
    if (const FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        if (FoundAttribute->Value <= FoundAttribute->MinValue)
        {
            OnAttributeThresholdReached.Broadcast(AttributeTag, FoundAttribute->Value);
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::CheckThresholds - Threshold reached for Attribute: %s"), *AttributeTag.ToString());
        }
    }
}

void UAttributeComponent::ApplyModifier(const FAttributeModifier& Modifier)
{
    if (IsValidAttribute(Modifier.AttributeTag))
    {
        FAttribute* FoundAttribute = AttributeMap.Find(Modifier.AttributeTag);
        if (FoundAttribute)
        {
            FoundAttribute->Value = FMath::Clamp(FoundAttribute->Value + Modifier.Value, FoundAttribute->MinValue, FoundAttribute->MaxValue);
            ActiveModifiers.Add(Modifier);
            OnAttributeChanged.Broadcast(Modifier.AttributeTag, FoundAttribute->Value);
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::ApplyModifier - Modifier applied to Attribute: %s, New Value: %f"), *Modifier.AttributeTag.ToString(), FoundAttribute->Value);

            if (Modifier.Duration > 0.0f)
            {
                GetWorld()->GetTimerManager().SetTimerForNextTick([this, Modifier]()
                    {
                        FTimerHandle TimerHandle;
                        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Modifier]()
                            {
                                RemoveModifier(Modifier.AttributeTag, Modifier.Value);
                            }, Modifier.Duration, false);
                    });
            }
        }
    }
}

void UAttributeComponent::RemoveModifier(const FGameplayTag& AttributeTag, float ModifierValue)
{
    if (IsValidAttribute(AttributeTag))
    {
        FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag);
        if (FoundAttribute)
        {
            FoundAttribute->Value = FMath::Clamp(FoundAttribute->Value - ModifierValue, FoundAttribute->MinValue, FoundAttribute->MaxValue);
            ActiveModifiers.RemoveAll([&](const FAttributeModifier& Modifier) { return Modifier.AttributeTag == AttributeTag && Modifier.Value == ModifierValue; });
            OnAttributeChanged.Broadcast(AttributeTag, FoundAttribute->Value);
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RemoveModifier - Modifier removed from Attribute: %s, New Value: %f"), *AttributeTag.ToString(), FoundAttribute->Value);
        }
    }
}

void UAttributeComponent::DecreaseHealth(float Amount)
{
    SetAttributeValue(FGameplayTag::RequestGameplayTag(FName("Attribute.Health")), Amount, EAttributeOperation::Subtract);
    float CachedHealth = GetAttributeValue(FGameplayTag::RequestGameplayTag(FName("Attribute.Health")));
    if (CachedHealth <= 0.f)
    {
        OnDeath.Broadcast();
    }
}

void UAttributeComponent::IncreaseHealth(float Amount)
{
    SetAttributeValue(FGameplayTag::RequestGameplayTag(FName("Attribute.Health")), Amount, EAttributeOperation::Add);
}


void UAttributeComponent::ResetAllAttributes()
{
    for (auto& Elem : RegenTimers)
    {
        GetWorld()->GetTimerManager().ClearTimer(Elem.Value);
    }
    RegenTimers.Empty();

    Attributes = InitialAttributesBackup;

    InitializeAttributeMap();

    for (const FAttribute& Attribute : Attributes)
    {
        OnAttributeChanged.Broadcast(Attribute.AttributeTag, Attribute.Value);
        CheckRegenStatus(Attribute.AttributeTag);
        UE_LOG(LogTemp, Warning, TEXT("Reset Attribute - Tag: %s, Value: %f, Min: %f, Max: %f"),
            *Attribute.AttributeTag.ToString(), Attribute.Value, Attribute.MinValue, Attribute.MaxValue);
    }

    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::ResetAllAttributes - All attributes reset and AttributeMap reinitialized"));
}

void UAttributeComponent::StartRegenTimer()
{
    for (FAttribute& Attribute : Attributes)
    {
        if (Attribute.bUseRegen && Attribute.Value < Attribute.MaxValue)
        {
            FTimerHandle& TimerHandle = RegenTimers.FindOrAdd(Attribute.AttributeTag);
            if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
            {
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAttributeComponent::RegenerateAttributes, Attribute.RegenRate, true);
                UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::StartRegenTimer - Regen started for Attribute: %s"), *Attribute.AttributeTag.ToString());
            }
        }
    }
}

void UAttributeComponent::RegenerateAttributes()
{
    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RegenerateAttributes - Timer triggered"));

    for (auto& Elem : RegenTimers)
    {
        FGameplayTag AttributeTag = Elem.Key;
        FAttribute* Attribute = AttributeMap.Find(AttributeTag);

        if (Attribute && Attribute->bUseRegen)
        {
            float OldValue = Attribute->Value;
            float NewValue = FMath::Clamp(OldValue + Attribute->RegenValue, Attribute->MinValue, Attribute->MaxValue);

            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RegenerateAttributes - Current Value: %f, RegenValue: %f, New Value: %f, MaxValue: %f"), OldValue, Attribute->RegenValue, NewValue, Attribute->MaxValue);

            if (OldValue != NewValue)
            {
                Attribute->Value = NewValue;
                OnAttributeChanged.Broadcast(Attribute->AttributeTag, Attribute->Value);
                UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RegenerateAttributes - Attribute: %s, Old Value: %f, New Value: %f"), *Attribute->AttributeTag.ToString(), OldValue, Attribute->Value);

                if (Attribute->Value >= Attribute->MaxValue)
                {
                    FTimerHandle* TimerHandle = RegenTimers.Find(AttributeTag);
                    if (TimerHandle)
                    {
                        GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
                        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RegenerateAttributes - Regen stopped for Attribute: %s"), *AttributeTag.ToString());
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::RegenerateAttributes - Attribute value did not change: %s, Current Value: %f"), *Attribute->AttributeTag.ToString(), Attribute->Value);
            }
        }
    }
}

void UAttributeComponent::GetHealthAttributeValues(float& OutValue, float& OutMin, float& OutMax) const
{
    GetAttributeValues(FGameplayTag::RequestGameplayTag(FName("Attribute.Health")), OutValue, OutMin, OutMax);
}

float UAttributeComponent::GetHealth() const
{
    return GetAttributeValue(FGameplayTag::RequestGameplayTag(FName("Attribute.Health")));
}

float UAttributeComponent::GetMaxHealth() const
{
    float Value, Min, Max;
    GetAttributeValues(FGameplayTag::RequestGameplayTag(FName("Attribute.Health")), Value, Min, Max);
    return Max;
}

float UAttributeComponent::GetHealthNormalized() const
{
    return GetAttributeNormalized(FGameplayTag::RequestGameplayTag(FName("Attribute.Health")));
}

void UAttributeComponent::SetAttributeValueInternal(const FGameplayTag& AttributeTag, float Value, EAttributeOperation Operation)
{
    if (FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        float OldValue = FoundAttribute->Value;
        float NewValue = OldValue;

        switch (Operation)
        {
        case EAttributeOperation::Add:
            NewValue += Value;
            break;
        case EAttributeOperation::Subtract:
            NewValue -= Value;
            break;
        case EAttributeOperation::Multiply:
            NewValue *= Value;
            break;
        case EAttributeOperation::Divide:
            if (Value != 0)
            {
                NewValue /= Value;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeValue - Divide by zero error for Attribute: %s"), *AttributeTag.ToString());
            }
            break;
        case EAttributeOperation::Override:
            NewValue = Value;
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeValue - Invalid operation for Attribute: %s"), *AttributeTag.ToString());
            break;
        }

        OnPreAttributeChanged.Broadcast(AttributeTag, NewValue);

        FoundAttribute->Value = FMath::Clamp(NewValue, FoundAttribute->MinValue, FoundAttribute->MaxValue);

        if (OldValue != FoundAttribute->Value)
        {
            OnAttributeChanged.Broadcast(AttributeTag, FoundAttribute->Value);
            CheckRegenStatus(AttributeTag);
            UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeValue - Attribute: %s, Old Value: %f, New Value: %f, Operation: %d"), *AttributeTag.ToString(), OldValue, FoundAttribute->Value, (int32)Operation);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::SetAttributeValue - Attribute not found: %s"), *AttributeTag.ToString());
    }
}

void UAttributeComponent::CheckRegenStatus(const FGameplayTag& AttributeTag)
{
    if (FAttribute* FoundAttribute = AttributeMap.Find(AttributeTag))
    {
        if (FoundAttribute->bUseRegen)
        {
            if (FoundAttribute->Value < FoundAttribute->MaxValue)
            {
                FTimerHandle& TimerHandle = RegenTimers.FindOrAdd(AttributeTag);
                if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
                {
                    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAttributeComponent::RegenerateAttributes, FoundAttribute->RegenRate, true);
                    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::CheckRegenStatus - Regen started for Attribute: %s"), *AttributeTag.ToString());
                }
            }
            else
            {
                if (FTimerHandle* TimerHandle = RegenTimers.Find(AttributeTag))
                {
                    GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
                    RegenTimers.Remove(AttributeTag);
                    UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::CheckRegenStatus - Regen stopped for Attribute: %s"), *AttributeTag.ToString());
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeComponent::CheckRegenStatus - Attribute not found: %s"), *AttributeTag.ToString());
    }
}

void UAttributeComponent::SaveAttributes(FString SlotName, int32 Index)
{
    UAttributeSaveGame* SaveGameInstance = Cast<UAttributeSaveGame>(UGameplayStatics::CreateSaveGameObject(UAttributeSaveGame::StaticClass()));

    if (SaveGameInstance)
    {
        for (const auto& AttributePair : AttributeMap)
        {
            SaveGameInstance->SavedAttributes.Add(AttributePair.Key, AttributePair.Value.Value);
        }

        if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, Index))
        {
            UE_LOG(LogTemp, Warning, TEXT("Attributes saved successfully to slot %s."), *SlotName);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to save attributes to slot %s."), *SlotName);
        }
    }
}

void UAttributeComponent::LoadAttributes(FString SlotName, int32 Index)
{
    UAttributeSaveGame* LoadGameInstance = Cast<UAttributeSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, Index));

    if (LoadGameInstance)
    {
        for (auto& AttributePair : AttributeMap)
        {
            if (LoadGameInstance->SavedAttributes.Contains(AttributePair.Key))
            {
                AttributePair.Value.Value = LoadGameInstance->SavedAttributes[AttributePair.Key];
                OnAttributeChanged.Broadcast(AttributePair.Key, AttributePair.Value.Value);
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Attributes loaded successfully from slot %s."), *SlotName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Save file not found in slot %s. Creating a new one."), *SlotName);

        SaveAttributes(SlotName, Index);
    }
}