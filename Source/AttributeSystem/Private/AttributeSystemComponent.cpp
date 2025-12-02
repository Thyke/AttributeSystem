// Copyright (C) Thyke. All Rights Reserved.


#include "AttributeSystemComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(AttributeSystemComponent)

UAttributeSystemComponent::UAttributeSystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = RegenTickInterval;
}

void UAttributeSystemComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize default attributes
    if (DefaultAttributes.Num() > 0)
    {
        InitializeAttributes(DefaultAttributes);
    }
}

void UAttributeSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Cleanup timers
    if (UWorld* World = GetWorld())
    {
        for (auto& AttributePair : Attributes)
        {
            for (FAttributeModifier& Modifier : AttributePair.Value.Modifiers)
            {
                if (Modifier.TimerHandle.IsValid())
                {
                    World->GetTimerManager().ClearTimer(Modifier.TimerHandle);
                }
            }
        }
    }
    
    Super::EndPlay(EndPlayReason);
}

void UAttributeSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!bEnableRegeneration)
    {
        return;
    }
    
    // Update modifier durations
    // UpdateModifierDurations(DeltaTime);
    
    // Process regeneration
    // FScopeLock Lock(&AttributeMutex);
    for (auto& AttributePair : Attributes)
    {
        ProcessRegeneration(AttributePair.Key, AttributePair.Value, DeltaTime);
    }
}

// ============================
// INITIALIZATION
// ============================

void UAttributeSystemComponent::InitializeAttributes(const TArray<UAttributeDefinition*>& AttributeDefinitions)
{
    // FScopeLock Lock(&AttributeMutex);
    
    for (UAttributeDefinition* Definition : AttributeDefinitions)
    {
        if (!Definition || !Definition->AttributeTag.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("AttributeSystem: Invalid attribute definition"));
            continue;
        }
        
        // Skip if already exists
        if (Attributes.Contains(Definition->AttributeTag))
        {
            UE_LOG(LogTemp, Warning, TEXT("AttributeSystem: Attribute %s already exists"), *Definition->AttributeTag.ToString());
            continue;
        }
        
        FAttributeRuntimeData RuntimeData;
        RuntimeData.Definition = Definition;
        RuntimeData.BaseValue = Definition->DefaultBaseValue;
        RuntimeData.CurrentValue = Definition->AttributeType == EAttributeType::Resource 
            ? Definition->DefaultCurrentValue 
            : Definition->DefaultBaseValue;
        RuntimeData.MarkDirty();
        
        Attributes.Add(Definition->AttributeTag, RuntimeData);
        
        if (bGlobalDebugMode || Definition->bDebugMode)
        {
            UE_LOG(LogTemp, Log, TEXT("AttributeSystem: Initialized %s - Base: %.2f, Current: %.2f"), 
                *Definition->AttributeTag.ToString(), RuntimeData.BaseValue, RuntimeData.CurrentValue);
        }
        
        // Broadcast event
        OnAttributeAdded.Broadcast(Definition->AttributeTag, Definition, RuntimeData.CurrentValue);
    }
}

void UAttributeSystemComponent::InitializeFromDataTable(UDataTable* DataTable)
{
    if (!DataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("AttributeSystem: Invalid DataTable"));
        return;
    }
    
    TArray<UAttributeDefinition*> Definitions;
    // Implementation depends on your data table structure
    // This is a placeholder
    
    InitializeAttributes(Definitions);
}

// ============================
// RUNTIME ATTRIBUTE MANAGEMENT
// ============================

bool UAttributeSystemComponent::AddAttribute(UAttributeDefinition* AttributeDefinition, float InitialCurrentValue)
{
    if (!AttributeDefinition || !AttributeDefinition->AttributeTag.IsValid())
    {
        return false;
    }
    
    // FScopeLock Lock(&AttributeMutex);
    
    if (Attributes.Contains(AttributeDefinition->AttributeTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeSystem: Attribute %s already exists"), 
            *AttributeDefinition->AttributeTag.ToString());
        return false;
    }
    
    FAttributeRuntimeData RuntimeData;
    RuntimeData.Definition = AttributeDefinition;
    RuntimeData.BaseValue = AttributeDefinition->DefaultBaseValue;
    RuntimeData.CurrentValue = InitialCurrentValue >= 0.0f ? InitialCurrentValue : AttributeDefinition->DefaultCurrentValue;
    RuntimeData.MarkDirty();
    
    Attributes.Add(AttributeDefinition->AttributeTag, RuntimeData);
    
    if (bGlobalDebugMode || AttributeDefinition->bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("AttributeSystem: Added attribute %s at runtime"), 
            *AttributeDefinition->AttributeTag.ToString());
    }
    
    OnAttributeAdded.Broadcast(AttributeDefinition->AttributeTag, AttributeDefinition, RuntimeData.CurrentValue);
    
    return true;
}

bool UAttributeSystemComponent::RemoveAttribute(FGameplayTag AttributeTag)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return false;
    }
    
    float FinalValue = AttributeData->CurrentValue;
    
    // Cleanup modifier timers
    if (UWorld* World = GetWorld())
    {
        for (FAttributeModifier& Modifier : AttributeData->Modifiers)
        {
            if (Modifier.TimerHandle.IsValid())
            {
                World->GetTimerManager().ClearTimer(Modifier.TimerHandle);
            }
        }
    }
    
    Attributes.Remove(AttributeTag);
    
    if (bGlobalDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("AttributeSystem: Removed attribute %s"), *AttributeTag.ToString());
    }
    
    OnAttributeRemoved.Broadcast(AttributeTag, FinalValue);
    
    return true;
}

bool UAttributeSystemComponent::HasAttribute(FGameplayTag AttributeTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    return Attributes.Contains(AttributeTag);
}

TArray<FGameplayTag> UAttributeSystemComponent::GetAllAttributeTags() const
{
    // FScopeLock Lock(&AttributeMutex);
    
    TArray<FGameplayTag> Tags;
    Attributes.GetKeys(Tags);
    return Tags;
}

const UAttributeDefinition* UAttributeSystemComponent::GetAttributeDefinition(FGameplayTag AttributeTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    return AttributeData ? AttributeData->Definition : nullptr;
}

// ============================
// VALUE GETTERS
// ============================

float UAttributeSystemComponent::GetAttributeCurrentValue(FGameplayTag AttributeTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    return AttributeData ? AttributeData->CurrentValue : 0.0f;
}

float UAttributeSystemComponent::GetAttributeBaseValue(FGameplayTag AttributeTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    return AttributeData ? AttributeData->BaseValue : 0.0f;
}

float UAttributeSystemComponent::GetAttributeFinalValue(FGameplayTag AttributeTag)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return 0.0f;
    }
    
    if (AttributeData->NeedsRecalculation())
    {
        CalculateFinalValue(*AttributeData);
    }
    
    return AttributeData->CachedFinalValue;
}

float UAttributeSystemComponent::GetAttributeMaxValue(FGameplayTag AttributeTag)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData || !AttributeData->Definition)
    {
        return 0.0f;
    }
    
    if (AttributeData->Definition->AttributeType == EAttributeType::Resource)
    {
        if (AttributeData->Definition->ClampConfig.bClampToAnotherAttribute)
        {
            return GetAttributeFinalValue(AttributeData->Definition->ClampConfig.ClampTargetAttributeTag);
        }
        
        return GetAttributeFinalValue(AttributeTag);
    }
    
    return AttributeData->Definition->ClampConfig.MaxValue;
}

float UAttributeSystemComponent::GetAttributePercent(FGameplayTag AttributeTag)
{
    const float Current = GetAttributeCurrentValue(AttributeTag);
    const float Max = GetAttributeMaxValue(AttributeTag);
    
    return Max > 0.0f ? (Current / Max) : 0.0f;
}

TMap<FGameplayTag, float> UAttributeSystemComponent::GetMultipleAttributeValues(const TArray<FGameplayTag>& AttributeTags)
{
    // FScopeLock Lock(&AttributeMutex);
    
    TMap<FGameplayTag, float> Result;
    Result.Reserve(AttributeTags.Num());
    
    for (const FGameplayTag& Tag : AttributeTags)
    {
        if (const FAttributeRuntimeData* AttributeData = Attributes.Find(Tag))
        {
            Result.Add(Tag, AttributeData->CurrentValue);
        }
    }
    
    return Result;
}

// ============================
// VALUE SETTERS
// ============================

bool UAttributeSystemComponent::SetAttributeCurrentValue(FGameplayTag AttributeTag, float NewValue, bool bClamp)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return false;
    }
    
    const float OldValue = AttributeData->CurrentValue;
    float FinalValue = bClamp ? ClampAttributeValue(*AttributeData, NewValue) : NewValue;
    
    if (FMath::IsNearlyEqual(OldValue, FinalValue))
    {
        return false;
    }
    
    AttributeData->CurrentValue = FinalValue;
    
    // Update damage time if value decreased
    if (FinalValue < OldValue)
    {
        AttributeData->LastDamageTime = GetWorld()->GetTimeSeconds();
    }
    
    BroadcastAttributeChange(AttributeTag, OldValue, FinalValue);
    
    // Check thresholds
    if (FMath::IsNearlyZero(FinalValue) && !FMath::IsNearlyZero(OldValue))
    {
        OnAttributeReachedZero.Broadcast(AttributeTag, OldValue);
    }
    
    const float MaxValue = GetAttributeMaxValue(AttributeTag);
    if (FMath::IsNearlyEqual(FinalValue, MaxValue) && !FMath::IsNearlyEqual(OldValue, MaxValue))
    {
        OnAttributeReachedMax.Broadcast(AttributeTag, FinalValue);
    }
    
    return true;
}

bool UAttributeSystemComponent::SetAttributeBaseValue(FGameplayTag AttributeTag, float NewBaseValue)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return false;
    }
    
    AttributeData->BaseValue = NewBaseValue;
    AttributeData->MarkDirty();
    
    return true;
}

float UAttributeSystemComponent::ModifyAttributeCurrentValue(FGameplayTag AttributeTag, float Delta, bool bClamp)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return 0.0f;
    }
    
    const float OldValue = AttributeData->CurrentValue;
    const float NewValue = OldValue + Delta;
    const float FinalValue = bClamp ? ClampAttributeValue(*AttributeData, NewValue) : NewValue;
    const float ActualDelta = FinalValue - OldValue;
    
    if (!FMath::IsNearlyZero(ActualDelta))
    {
        AttributeData->CurrentValue = FinalValue;
        
        if (ActualDelta < 0.0f)
        {
            AttributeData->LastDamageTime = GetWorld()->GetTimeSeconds();
        }
        
        BroadcastAttributeChange(AttributeTag, OldValue, FinalValue);
        
        // Check thresholds
        if (FMath::IsNearlyZero(FinalValue) && !FMath::IsNearlyZero(OldValue))
        {
            OnAttributeReachedZero.Broadcast(AttributeTag, OldValue);
        }
        
        const float MaxValue = GetAttributeMaxValue(AttributeTag);
        if (FMath::IsNearlyEqual(FinalValue, MaxValue) && !FMath::IsNearlyEqual(OldValue, MaxValue))
        {
            OnAttributeReachedMax.Broadcast(AttributeTag, FinalValue);
        }
    }
    
    return ActualDelta;
}

bool UAttributeSystemComponent::SetAttributeToMax(FGameplayTag AttributeTag)
{
    const float MaxValue = GetAttributeMaxValue(AttributeTag);
    return SetAttributeCurrentValue(AttributeTag, MaxValue, false);
}

bool UAttributeSystemComponent::SetAttributeToMin(FGameplayTag AttributeTag)
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData || !AttributeData->Definition)
    {
        return false;
    }
    
    const float MinValue = AttributeData->Definition->ClampConfig.bUseMinClamp 
        ? AttributeData->Definition->ClampConfig.MinValue 
        : 0.0f;
    
    return SetAttributeCurrentValue(AttributeTag, MinValue, false);
}

// ============================
// MODIFIER SYSTEM
// ============================

bool UAttributeSystemComponent::AddModifier(FGameplayTag AttributeTag, const FAttributeModifier& Modifier)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return false;
    }
    
    FAttributeModifier NewModifier = Modifier;
    NewModifier.RemainingDuration = Modifier.Duration;
    
    // Insert modifier in priority order
    int32 InsertIndex = AttributeData->Modifiers.Num();
    for (int32 i = 0; i < AttributeData->Modifiers.Num(); ++i)
    {
        if (NewModifier.Priority > AttributeData->Modifiers[i].Priority)
        {
            InsertIndex = i;
            break;
        }
    }
    
    AttributeData->Modifiers.Insert(NewModifier, InsertIndex);
    AttributeData->MarkDirty();
    
    // Setup timer for temporary modifiers
    if (Modifier.bIsTemporary && Modifier.Duration > 0.0f)
    {
        if (UWorld* World = GetWorld())
        {
            FTimerHandle& TimerHandle = AttributeData->Modifiers[InsertIndex].TimerHandle;
            
            // DÜZELTME: Lambda kullan veya helper fonksiyon kullan
            FTimerDelegate TimerDelegate;
            TimerDelegate.BindLambda([this, AttributeTag, ModifierTag = Modifier.ModifierTag, Source = Modifier.Source]()
            {
                RemoveModifier(AttributeTag, ModifierTag, Source);
            });
            
            World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Modifier.Duration, false);
        }
    }
    
    if (bGlobalDebugMode || (AttributeData->Definition && AttributeData->Definition->bDebugMode))
    {
        UE_LOG(LogTemp, Log, TEXT("AttributeSystem: Added modifier %s to %s (Value: %.2f, Operation: %d)"), 
            *Modifier.ModifierTag.ToString(), *AttributeTag.ToString(), Modifier.Value, (int32)Modifier.Operation);
    }
    
    OnModifierAdded.Broadcast(AttributeTag, Modifier.ModifierTag, Modifier.Value, Modifier.Source);
    
    return true;
}

bool UAttributeSystemComponent::RemoveModifier(FGameplayTag AttributeTag, FGameplayTag ModifierTag, UObject* Source)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return false;
    }
    
    bool bRemoved = false;
    for (int32 i = AttributeData->Modifiers.Num() - 1; i >= 0; --i)
    {
        const FAttributeModifier& Modifier = AttributeData->Modifiers[i];
        if (Modifier.ModifierTag == ModifierTag && (Source == nullptr || Modifier.Source == Source))
        {
            // Clear timer if exists
            if (Modifier.TimerHandle.IsValid())
            {
                if (UWorld* World = GetWorld())
                {
                    World->GetTimerManager().ClearTimer(Modifier.TimerHandle);
                }
            }
            
            AttributeData->Modifiers.RemoveAt(i);
            bRemoved = true;
            
            if (bGlobalDebugMode || (AttributeData->Definition && AttributeData->Definition->bDebugMode))
            {
                UE_LOG(LogTemp, Log, TEXT("AttributeSystem: Removed modifier %s from %s"), 
                    *ModifierTag.ToString(), *AttributeTag.ToString());
            }
            
            OnModifierRemoved.Broadcast(AttributeTag, ModifierTag, Source);
            
            if (Source != nullptr)
            {
                break; // Only remove one if source specified
            }
        }
    }
    
    if (bRemoved)
    {
        AttributeData->MarkDirty();
    }
    
    return bRemoved;
}

bool UAttributeSystemComponent::RemoveAllModifiers(FGameplayTag AttributeTag)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData || AttributeData->Modifiers.Num() == 0)
    {
        return false;
    }
    
    // Clear all timers
    if (UWorld* World = GetWorld())
    {
        for (const FAttributeModifier& Modifier : AttributeData->Modifiers)
        {
            if (Modifier.TimerHandle.IsValid())
            {
                World->GetTimerManager().ClearTimer(Modifier.TimerHandle);
            }
        }
    }
    
    AttributeData->Modifiers.Empty();
    AttributeData->MarkDirty();
    
    return true;
}

int32 UAttributeSystemComponent::RemoveModifiersByTag(FGameplayTag ModifierTag)
{
    // FScopeLock Lock(&AttributeMutex);
    
    int32 RemovedCount = 0;
    
    for (auto& AttributePair : Attributes)
    {
        FAttributeRuntimeData& AttributeData = AttributePair.Value;
        
        for (int32 i = AttributeData.Modifiers.Num() - 1; i >= 0; --i)
        {
            if (AttributeData.Modifiers[i].ModifierTag == ModifierTag)
            {
                if (AttributeData.Modifiers[i].TimerHandle.IsValid())
                {
                    if (UWorld* World = GetWorld())
                    {
                        World->GetTimerManager().ClearTimer(AttributeData.Modifiers[i].TimerHandle);
                    }
                }
                
                AttributeData.Modifiers.RemoveAt(i);
                AttributeData.MarkDirty();
                RemovedCount++;
            }
        }
    }
    
    return RemovedCount;
}

int32 UAttributeSystemComponent::RemoveModifiersBySource(UObject* Source)
{
    if (!Source)
    {
        return 0;
    }
    
    // FScopeLock Lock(&AttributeMutex);
    
    int32 RemovedCount = 0;
    
    for (auto& AttributePair : Attributes)
    {
        FAttributeRuntimeData& AttributeData = AttributePair.Value;
        
        for (int32 i = AttributeData.Modifiers.Num() - 1; i >= 0; --i)
        {
            if (AttributeData.Modifiers[i].Source == Source)
            {
                if (AttributeData.Modifiers[i].TimerHandle.IsValid())
                {
                    if (UWorld* World = GetWorld())
                    {
                        World->GetTimerManager().ClearTimer(AttributeData.Modifiers[i].TimerHandle);
                    }
                }
                
                AttributeData.Modifiers.RemoveAt(i);
                AttributeData.MarkDirty();
                RemovedCount++;
            }
        }
    }
    
    return RemovedCount;
}

TArray<FAttributeModifier> UAttributeSystemComponent::GetAttributeModifiers(FGameplayTag AttributeTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    return AttributeData ? AttributeData->Modifiers : TArray<FAttributeModifier>();
}

bool UAttributeSystemComponent::HasModifier(FGameplayTag AttributeTag, FGameplayTag ModifierTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return false;
    }
    
    for (const FAttributeModifier& Modifier : AttributeData->Modifiers)
    {
        if (Modifier.ModifierTag == ModifierTag)
        {
            return true;
        }
    }
    
    return false;
}

float UAttributeSystemComponent::GetTotalModifierValue(FGameplayTag AttributeTag, EAttributeModifierOperation Operation)
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return 0.0f;
    }
    
    float Total = 0.0f;
    for (const FAttributeModifier& Modifier : AttributeData->Modifiers)
    {
        if (Modifier.Operation == Operation)
        {
            Total += Modifier.Value;
        }
    }
    
    return Total;
}

// ============================
// REGENERATION
// ============================

void UAttributeSystemComponent::SetAttributeRegenEnabled(FGameplayTag AttributeTag, bool bEnabled)
{
    // Implementation would modify a runtime flag per attribute
    // For brevity, using the paused system
    if (!bEnabled)
    {
        PauseRegeneration(AttributeTag, -1.0f); // -1 = infinite
    }
    else
    {
        PausedRegenAttributes.Remove(AttributeTag);
    }
}

bool UAttributeSystemComponent::IsAttributeRegenEnabled(FGameplayTag AttributeTag) const
{
    return !PausedRegenAttributes.Contains(AttributeTag);
}

void UAttributeSystemComponent::ForceRegenTick(FGameplayTag AttributeTag)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (AttributeData)
    {
        ProcessRegeneration(AttributeTag, *AttributeData, RegenTickInterval);
    }
}

void UAttributeSystemComponent::SetAttributeRegenRate(FGameplayTag AttributeTag, float NewRegenRate)
{
    // FScopeLock Lock(&AttributeMutex);
    
    FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (AttributeData && AttributeData->Definition)
    {
        // This would require a mutable copy of the definition or runtime override
        // For production, you'd want a runtime override system
        UE_LOG(LogTemp, Warning, TEXT("AttributeSystem: SetAttributeRegenRate not fully implemented - requires runtime override system"));
    }
}

void UAttributeSystemComponent::PauseRegeneration(FGameplayTag AttributeTag, float Duration)
{
    PausedRegenAttributes.Add(AttributeTag, Duration);
}

// ============================
// BATCH OPERATIONS
// ============================

void UAttributeSystemComponent::ModifyMultipleAttributes(const TMap<FGameplayTag, float>& AttributeDeltas)
{
    for (const auto& Pair : AttributeDeltas)
    {
        ModifyAttributeCurrentValue(Pair.Key, Pair.Value, true);
    }
}

void UAttributeSystemComponent::AddMultipleModifiers(const TMap<FGameplayTag, FAttributeModifier>& Modifiers)
{
    for (const auto& Pair : Modifiers)
    {
        AddModifier(Pair.Key, Pair.Value);
    }
}

// ============================
// QUERY FUNCTIONS
// ============================

bool UAttributeSystemComponent::IsAttributeAtMax(FGameplayTag AttributeTag) const
{
    const float Current = GetAttributeCurrentValue(AttributeTag);
    const float Max = const_cast<UAttributeSystemComponent*>(this)->GetAttributeMaxValue(AttributeTag);
    return FMath::IsNearlyEqual(Current, Max);
}

bool UAttributeSystemComponent::IsAttributeAtMin(FGameplayTag AttributeTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData || !AttributeData->Definition)
    {
        return false;
    }
    
    const float Current = AttributeData->CurrentValue;
    const float Min = AttributeData->Definition->ClampConfig.bUseMinClamp 
        ? AttributeData->Definition->ClampConfig.MinValue 
        : 0.0f;
    
    return FMath::IsNearlyEqual(Current, Min);
}

bool UAttributeSystemComponent::IsAttributeAtZero(FGameplayTag AttributeTag) const
{
    return FMath::IsNearlyZero(GetAttributeCurrentValue(AttributeTag));
}

TArray<FGameplayTag> UAttributeSystemComponent::GetAttributesByTagQuery(const FGameplayTagQuery& Query) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    TArray<FGameplayTag> Result;
    
    for (const auto& AttributePair : Attributes)
    {
        if (AttributePair.Value.Definition)
        {
            FGameplayTagContainer Container;
            Container.AddTag(AttributePair.Key);
            Container.AppendTags(AttributePair.Value.Definition->GrantedTags);
            
            if (Query.Matches(Container))
            {
                Result.Add(AttributePair.Key);
            }
        }
    }
    
    return Result;
}

TArray<FGameplayTag> UAttributeSystemComponent::GetAttributesBelowPercent(float Threshold) const
{
    TArray<FGameplayTag> Result;
    
    for (const auto& AttributePair : Attributes)
    {
        const float Percent = const_cast<UAttributeSystemComponent*>(this)->GetAttributePercent(AttributePair.Key);
        if (Percent < Threshold)
        {
            Result.Add(AttributePair.Key);
        }
    }
    
    return Result;
}

// ============================
// DEBUG
// ============================

void UAttributeSystemComponent::DebugPrintAllAttributes() const
{
    // FScopeLock Lock(&AttributeMutex);
    
    UE_LOG(LogTemp, Log, TEXT("=== Attribute System Debug ==="));
    UE_LOG(LogTemp, Log, TEXT("Total Attributes: %d"), Attributes.Num());
    
    for (const auto& AttributePair : Attributes)
    {
        const FAttributeRuntimeData& Data = AttributePair.Value;
        UE_LOG(LogTemp, Log, TEXT("  %s: Current=%.2f, Base=%.2f, Modifiers=%d"), 
            *AttributePair.Key.ToString(), Data.CurrentValue, Data.BaseValue, Data.Modifiers.Num());
    }
}

FString UAttributeSystemComponent::GetAttributeDebugString(FGameplayTag AttributeTag) const
{
    // FScopeLock Lock(&AttributeMutex);
    
    const FAttributeRuntimeData* AttributeData = Attributes.Find(AttributeTag);
    if (!AttributeData)
    {
        return FString::Printf(TEXT("Attribute %s not found"), *AttributeTag.ToString());
    }
    
    return FString::Printf(TEXT("%s: Current=%.2f, Base=%.2f, Modifiers=%d, Type=%s"), 
        *AttributeTag.ToString(),
        AttributeData->CurrentValue,
        AttributeData->BaseValue,
        AttributeData->Modifiers.Num(),
        AttributeData->Definition ? *UEnum::GetValueAsString(AttributeData->Definition->AttributeType) : TEXT("Unknown"));
}

// ============================
// INTERNAL FUNCTIONS
// ============================

float UAttributeSystemComponent::CalculateFinalValue(FAttributeRuntimeData& AttributeData)
{
    if (!AttributeData.Definition)
    {
        return AttributeData.BaseValue;
    }
    
    float FinalValue = AttributeData.BaseValue;
    
    // Process modifiers by priority (already sorted)
    float AdditiveBonus = 0.0f;
    float MultiplicativeBonus = 1.0f;
    
    for (const FAttributeModifier& Modifier : AttributeData.Modifiers)
    {
        switch (Modifier.Operation)
        {
            case EAttributeModifierOperation::Add:
                AdditiveBonus += Modifier.Value;
                break;
                
            case EAttributeModifierOperation::Multiply:
                MultiplicativeBonus *= Modifier.Value;
                break;
                
            case EAttributeModifierOperation::Divide:
                if (!FMath::IsNearlyZero(Modifier.Value))
                {
                    MultiplicativeBonus /= Modifier.Value;
                }
                break;
                
            case EAttributeModifierOperation::Override:
                FinalValue = Modifier.Value;
                AdditiveBonus = 0.0f;
                MultiplicativeBonus = 1.0f;
                break;
        }
    }
    
    FinalValue = (FinalValue + AdditiveBonus) * MultiplicativeBonus;
    
    AttributeData.CachedFinalValue = FinalValue;
    AttributeData.bIsDirty = false;
    
    return FinalValue;
}

float UAttributeSystemComponent::ClampAttributeValue(const FAttributeRuntimeData& AttributeData, float Value) const
{
    if (!AttributeData.Definition)
    {
        return Value;
    }
    
    const FAttributeClampConfig& ClampConfig = AttributeData.Definition->ClampConfig;
    
    float MinValue = ClampConfig.bUseMinClamp ? ClampConfig.MinValue : -MAX_flt;
    float MaxValue = ClampConfig.bUseMaxClamp ? ClampConfig.MaxValue : MAX_flt;
    
    // Clamp to another attribute if specified
    if (ClampConfig.bClampToAnotherAttribute && ClampConfig.ClampTargetAttributeTag.IsValid())
    {
        MaxValue = const_cast<UAttributeSystemComponent*>(this)->GetAttributeFinalValue(ClampConfig.ClampTargetAttributeTag);
    }
    
    return FMath::Clamp(Value, MinValue, MaxValue);
}

void UAttributeSystemComponent::ProcessRegeneration(FGameplayTag AttributeTag, FAttributeRuntimeData& AttributeData, float DeltaTime)
{
    if (!AttributeData.Definition || AttributeData.Definition->RegenConfig.RegenType == EAttributeRegenType::None)
    {
        return;
    }
    
    // Check if paused
    if (float* PausedDuration = PausedRegenAttributes.Find(AttributeTag))
    {
        if (*PausedDuration > 0.0f)
        {
            *PausedDuration -= DeltaTime;
            if (*PausedDuration <= 0.0f)
            {
                PausedRegenAttributes.Remove(AttributeTag);
            }
            return;
        }
        else if (*PausedDuration < 0.0f) // -1 = permanently paused
        {
            return;
        }
    }
    
    const FAttributeRegenConfig& RegenConfig = AttributeData.Definition->RegenConfig;
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    
    // Check delay after damage
    if (RegenConfig.bPauseRegenOnDamage)
    {
        const float TimeSinceDamage = CurrentTime - AttributeData.LastDamageTime;
        if (TimeSinceDamage < RegenConfig.RegenDelay)
        {
            return;
        }
    }
    
    // Check if we need to regen
    const float TargetValue = RegenConfig.bRegenToMax 
        ? GetAttributeMaxValue(AttributeTag)
        : RegenConfig.RegenTargetValue;
    
    if (AttributeData.CurrentValue >= TargetValue)
    {
        return;
    }
    
    // Calculate regen amount
    float RegenAmount = 0.0f;
    
    switch (RegenConfig.RegenType)
    {
        case EAttributeRegenType::PerSecond:
            RegenAmount = RegenConfig.RegenRate * DeltaTime;
            break;
            
        case EAttributeRegenType::PerTick:
            RegenAmount = RegenConfig.RegenRate;
            break;
            
        case EAttributeRegenType::Delayed:
            if (CurrentTime - AttributeData.LastDamageTime >= RegenConfig.RegenDelay)
            {
                RegenAmount = RegenConfig.RegenRate * DeltaTime;
            }
            break;
    }
    
    if (RegenAmount > 0.0f)
    {
        const float OldValue = AttributeData.CurrentValue;
        const float NewValue = FMath::Min(AttributeData.CurrentValue + RegenAmount, TargetValue);
        AttributeData.CurrentValue = NewValue;
        AttributeData.LastRegenTime = CurrentTime;
        
        BroadcastAttributeChange(AttributeTag, OldValue, NewValue);
        OnAttributeRegenerated.Broadcast(AttributeTag, RegenAmount, NewValue);
        
        if (FMath::IsNearlyEqual(NewValue, TargetValue))
        {
            OnAttributeReachedMax.Broadcast(AttributeTag, NewValue);
        }
    }
}

void UAttributeSystemComponent::UpdateModifierDurations(float DeltaTime)
{
    // FScopeLock Lock(&AttributeMutex);
    
    for (auto& AttributePair : Attributes)
    {
        FAttributeRuntimeData& AttributeData = AttributePair.Value;
        
        for (int32 i = AttributeData.Modifiers.Num() - 1; i >= 0; --i)
        {
            FAttributeModifier& Modifier = AttributeData.Modifiers[i];
            if (Modifier.bIsTemporary && Modifier.RemainingDuration > 0.0f)
            {
                Modifier.RemainingDuration -= DeltaTime;
            }
        }
    }
}

void UAttributeSystemComponent::BroadcastAttributeChange(FGameplayTag AttributeTag, float OldValue, float NewValue)
{
    const float Delta = NewValue - OldValue;
    OnAttributeChanged.Broadcast(AttributeTag, OldValue, NewValue, Delta);
}

FAttributeRuntimeData* UAttributeSystemComponent::GetAttributeData(FGameplayTag AttributeTag)
{
    return Attributes.Find(AttributeTag);
}

const FAttributeRuntimeData* UAttributeSystemComponent::GetAttributeData(FGameplayTag AttributeTag) const
{
    return Attributes.Find(AttributeTag);
}

void UAttributeSystemComponent::RemoveModifierViaTimer(FGameplayTag AttributeTag, FGameplayTag ModifierTag,
    UObject* Source)
{
    RemoveModifier(AttributeTag, ModifierTag, Source);
}
