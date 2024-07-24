# AttributeSystem for Unreal Engine

AttributeSystem is a flexible and powerful attribute management system for Unreal Engine. It provides a robust framework for handling character attributes, including health, mana, stamina, and custom attributes. The system supports attribute modification, regeneration, serialization, and integration with Unreal Engine's Gameplay Tag system.

## Features

- **Data-Driven Attribute Management**: Use UAttributeData assets to define and configure attributes.
- **Gameplay Tag Integration**: Utilize Unreal Engine's Gameplay Tag system for attribute identification.
- **Dynamic Attribute Modification**: Add, remove, and modify attributes at runtime.
- **Attribute Regeneration**: Support for automatic attribute regeneration with customizable rates.
- **Event-Driven Architecture**: Provides delegates for attribute changes, thresholds, and other important events.
- **Serialization Support**: Save and load attribute states easily.
- **Blueprint Support**: Full exposure to Blueprint for easy integration in visual scripting.
- **Modular Design**: Implement as an ActorComponent for easy addition to any actor.

## Installation

1. Copy the `AttributeSystem` folder into your Unreal Engine project's `Plugins` directory.
2. Rebuild your project.
3. Enable the AttributeSystem plugin in your project settings.

## Usage

### Setting Up AttributeSystem

1. Add `UAttributeComponent` to your character blueprint or C++ class:

   ```cpp
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
   UAttributeComponent* AttributeComponent;
   ```

2. Initialize the component in your character's constructor:

   ```cpp
   AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
   ```

### Configuring Attributes

1. Create a new Attribute Data asset in the Content Browser: Right-click > Miscellaneous > Data Asset > AttributeData
2. Configure the attributes in the newly created asset:
   - Add attributes with their tags, initial values, min/max values, and regeneration settings.

3. Assign the Attribute Data asset to your Attribute Component in the blueprint or C++:

   ```cpp
   AttributeComponent->AttributeDataAsset = YourAttributeDataAsset;
   ```

### Using Attributes in Code

```cpp
// Get an attribute value
float health = AttributeComponent->GetAttributeValue(AttributeTags::Health);

// Modify an attribute
AttributeComponent->SetAttributeValue(AttributeTags::Mana, 50.0f, EAttributeOperation::Add);

// Check if an attribute exists
bool hasStamina = AttributeComponent->IsValidAttribute(AttributeTags::Stamina);

// Add a new attribute
FAttribute newAttribute(CustomTag, 100.0f, 0.0f, 200.0f, true, 1.0f, 5.0f);
AttributeComponent->AddAttribute(newAttribute);
```

### Listening for Attribute Changes

```cpp
AttributeComponent->OnAttributeChanged.AddDynamic(this, &AYourClass::OnAttributeChangedCallback);

void AYourClass::OnAttributeChangedCallback(FGameplayTag AttributeTag, float NewValue)
{
    if (AttributeTag == AttributeTags::Health)
    {
        // React to health change
    }
}
```

## Attribute Configuration

- `AttributeTag`: Gameplay tag identifying the attribute
- `Value`: Current value of the attribute
- `MinValue`: Minimum allowed value
- `MaxValue`: Maximum allowed value
- `bUseRegen`: Whether the attribute should automatically regenerate
- `RegenRate`: How often (in seconds) the attribute regenerates
- `RegenValue`: How much the attribute regenerates each tick

## Best Practices

- Use the provided AttributeTags namespace for common attributes (Health, Mana, Stamina).
- Create custom gameplay tags for game-specific attributes.
- Utilize the event system to react to attribute changes and thresholds.
- Consider performance when setting up regeneration for many attributes.

## Performance Considerations

- The system uses timers for regeneration. Be mindful of having too many frequently regenerating attributes.
- Use the `LoadAttributesFromDataAsset()` function during initialization to set up attributes efficiently.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
