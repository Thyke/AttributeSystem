// Copyright (C) Thyke. All Rights Reserved.


#include "AttributeSystemBlueprintLibrary.h"

#include "AttributeSystemComponent.h"
#include "GameplayTagContainer.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(AttributeSystemBlueprintLibrary)
UAttributeSystemComponent* UAttributeSystemBlueprintLibrary::GetAttributeSystemComponent(AActor* Actor)
{
    if (!Actor)
    {
        return nullptr;
    }
    
    return Actor->FindComponentByClass<UAttributeSystemComponent>();
}

UAttributeSystemComponent* UAttributeSystemBlueprintLibrary::GetOrCreateAttributeSystemComponent(AActor* Actor)
{
    if (!Actor)
    {
        return nullptr;
    }
    
    UAttributeSystemComponent* Component = Actor->FindComponentByClass<UAttributeSystemComponent>();
    if (!Component)
    {
        Component = NewObject<UAttributeSystemComponent>(Actor, NAME_None, RF_Transactional);
        Component->RegisterComponent();
        Actor->AddInstanceComponent(Component);
    }
    
    return Component;
}

float UAttributeSystemBlueprintLibrary::GetActorAttributeValue(AActor* Actor, FGameplayTag AttributeTag)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    return Component ? Component->GetAttributeCurrentValue(AttributeTag) : 0.0f;
}

bool UAttributeSystemBlueprintLibrary::SetActorAttributeValue(AActor* Actor, FGameplayTag AttributeTag, float NewValue, bool bClamp)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    return Component ? Component->SetAttributeCurrentValue(AttributeTag, NewValue, bClamp) : false;
}

float UAttributeSystemBlueprintLibrary::ModifyActorAttribute(AActor* Actor, FGameplayTag AttributeTag, float Delta, bool bClamp)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    return Component ? Component->ModifyAttributeCurrentValue(AttributeTag, Delta, bClamp) : 0.0f;
}

float UAttributeSystemBlueprintLibrary::GetActorAttributePercent(AActor* Actor, FGameplayTag AttributeTag)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    return Component ? Component->GetAttributePercent(AttributeTag) : 0.0f;
}

FAttributeModifier UAttributeSystemBlueprintLibrary::MakeAdditiveModifier(FGameplayTag ModifierTag, float Value, int32 Priority)
{
    FAttributeModifier Modifier;
    Modifier.ModifierTag = ModifierTag;
    Modifier.Operation = EAttributeModifierOperation::Add;
    Modifier.Value = Value;
    Modifier.Priority = Priority;
    return Modifier;
}

FAttributeModifier UAttributeSystemBlueprintLibrary::MakeMultiplicativeModifier(FGameplayTag ModifierTag, float Multiplier, int32 Priority)
{
    FAttributeModifier Modifier;
    Modifier.ModifierTag = ModifierTag;
    Modifier.Operation = EAttributeModifierOperation::Multiply;
    Modifier.Value = Multiplier;
    Modifier.Priority = Priority;
    return Modifier;
}

FAttributeModifier UAttributeSystemBlueprintLibrary::MakeTemporaryModifier(FGameplayTag ModifierTag, 
    EAttributeModifierOperation Operation, float Value, float Duration, int32 Priority)
{
    FAttributeModifier Modifier;
    Modifier.ModifierTag = ModifierTag;
    Modifier.Operation = Operation;
    Modifier.Value = Value;
    Modifier.Priority = Priority;
    Modifier.bIsTemporary = true;
    Modifier.Duration = Duration;
    return Modifier;
}

FAttributeModifier UAttributeSystemBlueprintLibrary::MakePercentageModifier(FGameplayTag ModifierTag, float Percentage, int32 Priority)
{
    FAttributeModifier Modifier;
    Modifier.ModifierTag = ModifierTag;
    Modifier.Operation = EAttributeModifierOperation::Multiply;
    Modifier.Value = 1.0f + (Percentage / 100.0f); // Convert percentage to multiplier
    Modifier.Priority = Priority;
    return Modifier;
}

void UAttributeSystemBlueprintLibrary::ApplyDamageToAttributes(AActor* Actor, const TMap<FGameplayTag, float>& DamageMap)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component)
    {
        return;
    }
    
    TMap<FGameplayTag, float> NegativeMap;
    for (const auto& Pair : DamageMap)
    {
        NegativeMap.Add(Pair.Key, -FMath::Abs(Pair.Value));
    }
    
    Component->ModifyMultipleAttributes(NegativeMap);
}

void UAttributeSystemBlueprintLibrary::HealMultipleAttributes(AActor* Actor, const TMap<FGameplayTag, float>& HealMap)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component)
    {
        return;
    }
    
    TMap<FGameplayTag, float> PositiveMap;
    for (const auto& Pair : HealMap)
    {
        PositiveMap.Add(Pair.Key, FMath::Abs(Pair.Value));
    }
    
    Component->ModifyMultipleAttributes(PositiveMap);
}

void UAttributeSystemBlueprintLibrary::SetMultipleAttributesToMax(AActor* Actor, const TArray<FGameplayTag>& AttributeTags)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component)
    {
        return;
    }
    
    for (const FGameplayTag& Tag : AttributeTags)
    {
        Component->SetAttributeToMax(Tag);
    }
}

float UAttributeSystemBlueprintLibrary::CompareAttributeValues(AActor* ActorA, AActor* ActorB, FGameplayTag AttributeTag)
{
    const float ValueA = GetActorAttributeValue(ActorA, AttributeTag);
    const float ValueB = GetActorAttributeValue(ActorB, AttributeTag);
    return ValueA - ValueB;
}

AActor* UAttributeSystemBlueprintLibrary::GetActorWithHighestAttribute(const TArray<AActor*>& Actors, FGameplayTag AttributeTag)
{
    AActor* BestActor = nullptr;
    float HighestValue = -MAX_flt;
    
    for (AActor* Actor : Actors)
    {
        const float Value = GetActorAttributeValue(Actor, AttributeTag);
        if (Value > HighestValue)
        {
            HighestValue = Value;
            BestActor = Actor;
        }
    }
    
    return BestActor;
}

AActor* UAttributeSystemBlueprintLibrary::GetActorWithLowestAttribute(const TArray<AActor*>& Actors, FGameplayTag AttributeTag)
{
    AActor* BestActor = nullptr;
    float LowestValue = MAX_flt;
    
    for (AActor* Actor : Actors)
    {
        const float Value = GetActorAttributeValue(Actor, AttributeTag);
        if (Value < LowestValue)
        {
            LowestValue = Value;
            BestActor = Actor;
        }
    }
    
    return BestActor;
}

float UAttributeSystemBlueprintLibrary::InterpAttributeToTarget(AActor* Actor, FGameplayTag AttributeTag, 
    float TargetValue, float DeltaTime, float InterpSpeed)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component)
    {
        return 0.0f;
    }
    
    const float CurrentValue = Component->GetAttributeCurrentValue(AttributeTag);
    const float NewValue = FMath::FInterpTo(CurrentValue, TargetValue, DeltaTime, InterpSpeed);
    Component->SetAttributeCurrentValue(AttributeTag, NewValue, true);
    
    return NewValue;
}

float UAttributeSystemBlueprintLibrary::TransferAttributeValue(AActor* FromActor, AActor* ToActor, 
    FGameplayTag AttributeTag, float Amount, bool bClamp)
{
    UAttributeSystemComponent* FromComponent = GetAttributeSystemComponent(FromActor);
    UAttributeSystemComponent* ToComponent = GetAttributeSystemComponent(ToActor);
    
    if (!FromComponent || !ToComponent)
    {
        return 0.0f;
    }
    
    // Remove from source
    const float ActualRemoved = -FromComponent->ModifyAttributeCurrentValue(AttributeTag, -Amount, bClamp);
    
    // Add to target
    ToComponent->ModifyAttributeCurrentValue(AttributeTag, ActualRemoved, bClamp);
    
    return ActualRemoved;
}

void UAttributeSystemBlueprintLibrary::CopyAttributesToActor(AActor* SourceActor, AActor* TargetActor)
{
    UAttributeSystemComponent* SourceComponent = GetAttributeSystemComponent(SourceActor);
    UAttributeSystemComponent* TargetComponent = GetAttributeSystemComponent(TargetActor);
    
    if (!SourceComponent || !TargetComponent)
    {
        return;
    }
    
    const TArray<FGameplayTag> Tags = SourceComponent->GetAllAttributeTags();
    for (const FGameplayTag& Tag : Tags)
    {
        const float Value = SourceComponent->GetAttributeCurrentValue(Tag);
        TargetComponent->SetAttributeCurrentValue(Tag, Value, false);
    }
}

bool UAttributeSystemBlueprintLibrary::CanAffordAttributeCost(AActor* Actor, const TMap<FGameplayTag, float>& Costs)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component)
    {
        return false;
    }
    
    for (const auto& CostPair : Costs)
    {
        const float CurrentValue = Component->GetAttributeCurrentValue(CostPair.Key);
        if (CurrentValue < CostPair.Value)
        {
            return false;
        }
    }
    
    return true;
}

bool UAttributeSystemBlueprintLibrary::IsAttributeInRange(float AttributeValue, float MinValue, float MaxValue, bool bInclusive)
{
    if (bInclusive)
    {
        return AttributeValue >= MinValue && AttributeValue <= MaxValue;
    }
    else
    {
        return AttributeValue > MinValue && AttributeValue < MaxValue;
    }
}

float UAttributeSystemBlueprintLibrary::GetMissingAttributeAmount(AActor* Actor, FGameplayTag AttributeTag, float TargetValue)
{
    const float CurrentValue = GetActorAttributeValue(Actor, AttributeTag);
    return FMath::Max(0.0f, TargetValue - CurrentValue);
}

FText UAttributeSystemBlueprintLibrary::FormatAttributeValue(float Value, int32 DecimalPlaces)
{
    FNumberFormattingOptions Options;
    Options.MaximumFractionalDigits = DecimalPlaces;
    Options.MinimumFractionalDigits = DecimalPlaces;
    return FText::AsNumber(Value, &Options);
}

FText UAttributeSystemBlueprintLibrary::FormatAttributePercent(float Percent, int32 DecimalPlaces)
{
    FNumberFormattingOptions Options;
    Options.MaximumFractionalDigits = DecimalPlaces;
    Options.MinimumFractionalDigits = DecimalPlaces;
    return FText::Format(FText::FromString("{0}%"), FText::AsNumber(Percent * 100.0f, &Options));
}

FText UAttributeSystemBlueprintLibrary::GetAttributeDisplayText(AActor* Actor, FGameplayTag AttributeTag)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component)
    {
        return FText::FromString("N/A");
    }
    
    const float Current = Component->GetAttributeCurrentValue(AttributeTag);
    const float Max = Component->GetAttributeMaxValue(AttributeTag);
    
    return FText::Format(FText::FromString("{0} / {1}"), 
        FText::AsNumber(FMath::RoundToInt(Current)), 
        FText::AsNumber(FMath::RoundToInt(Max)));
}

FLinearColor UAttributeSystemBlueprintLibrary::GetAttributeColorByPercent(float Percent)
{
    if (Percent > 0.6f)
    {
        // Green to Yellow
        return FLinearColor::LerpUsingHSV(FLinearColor::Green, FLinearColor::Yellow, (1.0f - Percent) / 0.4f);
    }
    else if (Percent > 0.3f)
    {
        // Yellow to Orange
        return FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor(1.0f, 0.5f, 0.0f), (0.6f - Percent) / 0.3f);
    }
    else
    {
        // Orange to Red
        return FLinearColor::LerpUsingHSV(FLinearColor(1.0f, 0.5f, 0.0f), FLinearColor::Red, (0.3f - Percent) / 0.3f);
    }
}

FLinearColor UAttributeSystemBlueprintLibrary::GetColorGradient(float Percent, FLinearColor LowColor, 
    FLinearColor MidColor, FLinearColor HighColor)
{
    if (Percent < 0.5f)
    {
        return FLinearColor::LerpUsingHSV(LowColor, MidColor, Percent * 2.0f);
    }
    else
    {
        return FLinearColor::LerpUsingHSV(MidColor, HighColor, (Percent - 0.5f) * 2.0f);
    }
}

FString UAttributeSystemBlueprintLibrary::SerializeAttributes(AActor* Actor)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component)
    {
        return FString();
    }
    
    FString Result;
    const TArray<FGameplayTag> Tags = Component->GetAllAttributeTags();
    
    for (const FGameplayTag& Tag : Tags)
    {
        const float CurrentValue = Component->GetAttributeCurrentValue(Tag);
        const float BaseValue = Component->GetAttributeBaseValue(Tag);
        
        Result += FString::Printf(TEXT("%s|%.2f|%.2f;"), *Tag.ToString(), CurrentValue, BaseValue);
    }
    
    return Result;
}

bool UAttributeSystemBlueprintLibrary::DeserializeAttributes(AActor* Actor, const FString& SerializedData)
{
    UAttributeSystemComponent* Component = GetAttributeSystemComponent(Actor);
    if (!Component || SerializedData.IsEmpty())
    {
        return false;
    }
    
    TArray<FString> AttributeStrings;
    SerializedData.ParseIntoArray(AttributeStrings, TEXT(";"), true);
    
    for (const FString& AttrString : AttributeStrings)
    {
        TArray<FString> Parts;
        AttrString.ParseIntoArray(Parts, TEXT("|"), true);
        
        if (Parts.Num() == 3)
        {
            const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*Parts[0]));
            const float CurrentValue = FCString::Atof(*Parts[1]);
            const float BaseValue = FCString::Atof(*Parts[2]);
            
            if (Component->HasAttribute(Tag))
            {
                Component->SetAttributeBaseValue(Tag, BaseValue);
                Component->SetAttributeCurrentValue(Tag, CurrentValue, false);
            }
        }
    }
    
    return true;
}

FGameplayTag UAttributeSystemBlueprintLibrary::MakeAttributeTag(const FString& TagString)
{
    return FGameplayTag::RequestGameplayTag(FName(*TagString));
}

bool UAttributeSystemBlueprintLibrary::IsValidAttributeTag(FGameplayTag Tag)
{
    return Tag.IsValid() && Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Attribute")));
}