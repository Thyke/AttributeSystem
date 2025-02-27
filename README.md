# Attribute System

A component-based, modular, and Blueprint-friendly attribute management system for Unreal Engine.

---

*Read this in other languages: [English](#attribute-system), [Turkish](#attribute-system-türkçe)*

---

## Overview

Attribute System is a flexible solution for managing gameplay attributes (health, mana, stamina, etc.) in Unreal Engine. It provides a component-based architecture for easy integration into any actor, with support for attribute modifications, regeneration, thresholds, and persistence.

## Features

- **Component-Based Architecture**: Easy to add to any actor with minimal setup
- **Data-Driven Design**: Configure attributes using Data Assets
- **GameplayTags Integration**: Use tags to identify and reference attributes
- **Full Blueprint Support**: Complete access to all functionality from Blueprints
- **Attribute Modifiers**: Apply temporary or permanent modifications to attributes
- **Automatic Regeneration**: Configure attributes to regenerate over time
- **Event-Based System**: React to attribute changes with delegates
- **Save/Load Support**: Persist attribute data between game sessions
- **Threshold Notifications**: Receive events when attributes reach specific values
- **Performance Optimized**: Designed for minimal overhead

## Installation

1. Clone this repository
2. Copy the `AttributeSystem` folder to your project's `Plugins` directory
3. Regenerate project files and recompile your project
4. Enable the plugin in your project settings

Alternatively, you can copy the source files directly into your project's source directory.

## Basic Usage

### Creating Attribute Data Assets

1. In the Content Browser, right-click and select **Create > Miscellaneous > Data Asset**
2. Choose **AttributeData** as the asset type
3. Add attributes with the following properties:
   - **AttributeTag**: GameplayTag identifying the attribute (e.g., "Attributes.Health")
   - **Value**: Starting value
   - **MinValue**: Minimum possible value
   - **MaxValue**: Maximum possible value
   - **bUseRegen**: Whether the attribute should regenerate over time
   - **RegenRate**: Seconds between regeneration ticks
   - **RegenValue**: Amount to regenerate per tick

```cpp
// Example of creating an attribute data asset in code
UAttributeData* AttributeData = NewObject<UAttributeData>();
FAttribute HealthAttribute = FAttribute(
    AttributeTags::Health,  // FGameplayTag
    100.0f,                 // Initial value
    0.0f,                   // Min value
    100.0f,                 // Max value
    true,                   // Use regeneration
    5.0f,                   // Regeneration rate (seconds)
    1.0f                    // Regeneration value per tick
);
AttributeData->Attributes.Add(HealthAttribute);
```

### Adding the Attribute Component to an Actor

```cpp
// In your actor's header file
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
class UAttributeComponent* AttributeComponent;

// In your actor's constructor
AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
```

### Loading Attributes from a Data Asset

```cpp
// In your actor's BeginPlay or constructor
if (UAttributeData* MyAttributeData = LoadObject<UAttributeData>(nullptr, TEXT("/Game/Data/DA_PlayerAttributes")))
{
    AttributeComponent->AttributeDataAsset = MyAttributeData;
    AttributeComponent->LoadAttributesFromDataAsset();
}
```

### Working with Attributes

```cpp
// Getting attribute values
float CurrentHealth = AttributeComponent->GetAttributeValue(AttributeTags::Health);
float HealthPercentage = AttributeComponent->GetAttributeNormalized(AttributeTags::Health);

// Modifying attributes
AttributeComponent->DecreaseHealth(10.0f);  // Convenience method for health
AttributeComponent->IncreaseAttributeValue(AttributeTags::Mana, 15.0f);
AttributeComponent->SetAttributeValue(AttributeTags::Stamina, 50.0f, EAttributeOperation::Override);

// Applying temporary modifiers
FAttributeModifier SpeedBoost(AttributeTags::MovementSpeed, 1.5f, 10.0f); // 50% boost for 10 seconds
AttributeComponent->ApplyModifier(SpeedBoost);

// Binding to events
AttributeComponent->OnAttributeChanged.AddDynamic(this, &AMyActor::HandleAttributeChanged);
AttributeComponent->OnDeath.AddDynamic(this, &AMyActor::HandleCharacterDeath);
```

### Saving and Loading Attributes

```cpp
// Save current attributes
AttributeComponent->SaveAttributes("PlayerSaveSlot", 0);

// Load saved attributes
AttributeComponent->LoadAttributes("PlayerSaveSlot", 0);
```

## Example: Character Implementation

Here's a complete example of a character with health, mana, and stamina:

```cpp
// MyCharacter.h
UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    AMyCharacter();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAttributeComponent* AttributeComponent;
    
    UFUNCTION()
    void HandleHealthChanged(float CurrentValue, float NewValue);
    
    UFUNCTION()
    void HandleDeath();
    
    UFUNCTION(BlueprintCallable)
    void UseAbility(float ManaCost, float StaminaCost);
};

// MyCharacter.cpp
AMyCharacter::AMyCharacter()
{
    AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (AttributeComponent)
    {
        AttributeComponent->LoadAttributesFromDataAsset();
        AttributeComponent->OnHealthChanged.AddDynamic(this, &AMyCharacter::HandleHealthChanged);
        AttributeComponent->OnDeath.AddDynamic(this, &AMyCharacter::HandleDeath);
    }
}

void AMyCharacter::HandleHealthChanged(float CurrentValue, float NewValue)
{
    if (NewValue <= CurrentValue * 0.2f)
    {
        // Low health effects
        PlayLowHealthEffects();
    }
}

void AMyCharacter::HandleDeath()
{
    // Handle character death
    DisableInput(nullptr);
    PlayDeathAnimation();
    // ...
}

void AMyCharacter::UseAbility(float ManaCost, float StaminaCost)
{
    bool bHasEnoughMana = AttributeComponent->GetAttributeValue(AttributeTags::Mana) >= ManaCost;
    bool bHasEnoughStamina = AttributeComponent->GetAttributeValue(AttributeTags::Stamina) >= StaminaCost;
    
    if (bHasEnoughMana && bHasEnoughStamina)
    {
        AttributeComponent->DecreaseAttributeValue(AttributeTags::Mana, ManaCost);
        AttributeComponent->DecreaseAttributeValue(AttributeTags::Stamina, StaminaCost);
        
        // Perform ability
        ExecuteAbility();
    }
}
```

## API Reference

### UAttributeComponent

| Method | Description |
|--------|-------------|
| `LoadAttributesFromDataAsset` | Loads attributes from the assigned data asset |
| `GetAttributeValue` | Returns the current value of an attribute |
| `SetAttributeValue` | Sets an attribute value with specified operation |
| `DecreaseAttributeValue` | Reduces an attribute by the specified amount |
| `IncreaseAttributeValue` | Increases an attribute by the specified amount |
| `GetAttributeNormalized` | Returns the attribute value as a percentage (0-1) of its range |
| `AddAttribute` | Adds a new attribute to the component |
| `RemoveAttribute` | Removes an attribute from the component |
| `ApplyModifier` | Applies a temporary or permanent modifier to an attribute |
| `RemoveModifier` | Removes a specific modifier from an attribute |
| `DecreaseHealth` | Convenience method to reduce health |
| `IncreaseHealth` | Convenience method to increase health |
| `SaveAttributes` | Saves all attribute values to a slot |
| `LoadAttributes` | Loads attribute values from a slot |

### FAttribute Structure

| Property | Description |
|----------|-------------|
| `AttributeTag` | GameplayTag identifying the attribute |
| `Value` | Current value of the attribute |
| `MinValue` | Minimum possible value |
| `MaxValue` | Maximum possible value |
| `bUseRegen` | Whether the attribute regenerates over time |
| `RegenRate` | Time in seconds between regeneration ticks |
| `RegenValue` | Amount to regenerate per tick |

### Delegates

| Delegate | Description |
|----------|-------------|
| `OnAttributeChanged` | Called when an attribute value changes |
| `OnAttributeAdded` | Called when a new attribute is added |
| `OnAttributeRemoved` | Called when an attribute is removed |
| `OnAttributeThresholdReached` | Called when an attribute reaches a threshold |
| `OnHealthChanged` | Convenience delegate for health changes |
| `OnDeath` | Called when health reaches zero |

## Best Practices

1. **Use Data Assets** for consistent attribute configuration across multiple actors
2. **Leverage GameplayTags** for flexible attribute identification
3. **Create attribute categories** with tag hierarchies (e.g., "Attributes.Primary.Strength")
4. **Handle edge cases** like division by zero in attribute operations
5. **Limit attribute updates** in performance-critical sections
6. **Use attribute thresholds** for gameplay events rather than polling values
7. **Consider network replication** needs for multiplayer games
8. **Add game-specific convenience methods** to extend functionality
9. **Document your attributes** for team collaboration

## License

This project is licensed under the MIT License - see the LICENSE file for details.

---

# Attribute System (Türkçe)

Unreal Engine için komponent tabanlı, modüler ve Blueprint-dostu öznitelik yönetim sistemi.

## Genel Bakış

Attribute System, Unreal Engine'de oyun özniteliklerini (sağlık, mana, dayanıklılık vb.) yönetmek için esnek bir çözümdür. Herhangi bir aktöre kolay entegrasyon için komponent tabanlı bir mimari sağlar ve öznitelik değişiklikleri, yenilenme, eşikler ve kalıcılık destekler.

## Özellikler

- **Komponent Tabanlı Mimari**: Minimal kurulumla herhangi bir aktöre eklemesi kolay
- **Veri Odaklı Tasarım**: Data Asset'ler kullanarak öznitelikleri yapılandırma
- **GameplayTags Entegrasyonu**: Öznitelikleri tanımlamak ve referans vermek için etiketler kullanma
- **Tam Blueprint Desteği**: Tüm işlevselliğe Blueprint'lerden erişim
- **Öznitelik Modifikatörleri**: Özniteliklere geçici veya kalıcı değişiklikler uygulama
- **Otomatik Yenilenme**: Özniteliklerin zamanla yenilenmesini yapılandırma
- **Olay Tabanlı Sistem**: Delegate'ler ile öznitelik değişikliklerine tepki verme
- **Kaydetme/Yükleme Desteği**: Oyun oturumları arasında öznitelik verilerini koruma
- **Eşik Bildirimleri**: Öznitelikler belirli değerlere ulaştığında olaylar alma
- **Performans Optimizasyonu**: Minimal yük için tasarlanmış

## Kurulum

1. Bu depoyu klonlayın
2. `AttributeSystem` klasörünü projenizin `Plugins` dizinine kopyalayın
3. Proje dosyalarını yeniden oluşturun ve projenizi derleyin
4. Eklentiyi proje ayarlarınızda etkinleştirin

Alternatif olarak, kaynak dosyalarını doğrudan projenizin kaynak dizinine kopyalayabilirsiniz.

## Temel Kullanım

### Attribute Data Asset Oluşturma

1. Content Browser'da sağ tıklayın ve **Create > Miscellaneous > Data Asset** seçin
2. Asset tipi olarak **AttributeData** seçin
3. Aşağıdaki özelliklere sahip öznitelikler ekleyin:
   - **AttributeTag**: Özniteliği tanımlayan GameplayTag (örn., "Attributes.Health")
   - **Value**: Başlangıç değeri
   - **MinValue**: Olası minimum değer
   - **MaxValue**: Olası maksimum değer
   - **bUseRegen**: Özniteliğin zamanla yenilenip yenilenmeyeceği
   - **RegenRate**: Yenilenme tikleri arasındaki saniye
   - **RegenValue**: Tik başına yenilenecek miktar

```cpp
// Kodda attribute data asset oluşturma örneği
UAttributeData* AttributeData = NewObject<UAttributeData>();
FAttribute HealthAttribute = FAttribute(
    AttributeTags::Health,  // FGameplayTag
    100.0f,                 // Başlangıç değeri
    0.0f,                   // Min değer
    100.0f,                 // Max değer
    true,                   // Yenilenme kullan
    5.0f,                   // Yenilenme oranı (saniye)
    1.0f                    // Tik başına yenilenme değeri
);
AttributeData->Attributes.Add(HealthAttribute);
```

### Bir Aktöre Attribute Component Ekleme

```cpp
// Aktörünüzün header dosyasında
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
class UAttributeComponent* AttributeComponent;

// Aktörünüzün constructor'ında
AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
```

### Data Asset'ten Öznitelikleri Yükleme

```cpp
// Aktörünüzün BeginPlay veya constructor'ında
if (UAttributeData* MyAttributeData = LoadObject<UAttributeData>(nullptr, TEXT("/Game/Data/DA_PlayerAttributes")))
{
    AttributeComponent->AttributeDataAsset = MyAttributeData;
    AttributeComponent->LoadAttributesFromDataAsset();
}
```

### Özniteliklerle Çalışma

```cpp
// Öznitelik değerlerini alma
float CurrentHealth = AttributeComponent->GetAttributeValue(AttributeTags::Health);
float HealthPercentage = AttributeComponent->GetAttributeNormalized(AttributeTags::Health);

// Öznitelikleri değiştirme
AttributeComponent->DecreaseHealth(10.0f);  // Sağlık için kolaylık metodu
AttributeComponent->IncreaseAttributeValue(AttributeTags::Mana, 15.0f);
AttributeComponent->SetAttributeValue(AttributeTags::Stamina, 50.0f, EAttributeOperation::Override);

// Geçici modifikatörler uygulama
FAttributeModifier SpeedBoost(AttributeTags::MovementSpeed, 1.5f, 10.0f); // 10 saniye için %50 artış
AttributeComponent->ApplyModifier(SpeedBoost);

// Olaylara bağlanma
AttributeComponent->OnAttributeChanged.AddDynamic(this, &AMyActor::HandleAttributeChanged);
AttributeComponent->OnDeath.AddDynamic(this, &AMyActor::HandleCharacterDeath);
```

### Öznitelikleri Kaydetme ve Yükleme

```cpp
// Mevcut öznitelikleri kaydet
AttributeComponent->SaveAttributes("PlayerSaveSlot", 0);

// Kaydedilmiş öznitelikleri yükle
AttributeComponent->LoadAttributes("PlayerSaveSlot", 0);
```

## Örnek: Karakter İmplementasyonu

İşte sağlık, mana ve dayanıklılığa sahip bir karakter için tam bir örnek:

```cpp
// MyCharacter.h
UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    AMyCharacter();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAttributeComponent* AttributeComponent;
    
    UFUNCTION()
    void HandleHealthChanged(float CurrentValue, float NewValue);
    
    UFUNCTION()
    void HandleDeath();
    
    UFUNCTION(BlueprintCallable)
    void UseAbility(float ManaCost, float StaminaCost);
};

// MyCharacter.cpp
AMyCharacter::AMyCharacter()
{
    AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (AttributeComponent)
    {
        AttributeComponent->LoadAttributesFromDataAsset();
        AttributeComponent->OnHealthChanged.AddDynamic(this, &AMyCharacter::HandleHealthChanged);
        AttributeComponent->OnDeath.AddDynamic(this, &AMyCharacter::HandleDeath);
    }
}

void AMyCharacter::HandleHealthChanged(float CurrentValue, float NewValue)
{
    if (NewValue <= CurrentValue * 0.2f)
    {
        // Düşük sağlık efektleri
        PlayLowHealthEffects();
    }
}

void AMyCharacter::HandleDeath()
{
    // Karakter ölümünü işle
    DisableInput(nullptr);
    PlayDeathAnimation();
    // ...
}

void AMyCharacter::UseAbility(float ManaCost, float StaminaCost)
{
    bool bHasEnoughMana = AttributeComponent->GetAttributeValue(AttributeTags::Mana) >= ManaCost;
    bool bHasEnoughStamina = AttributeComponent->GetAttributeValue(AttributeTags::Stamina) >= StaminaCost;
    
    if (bHasEnoughMana && bHasEnoughStamina)
    {
        AttributeComponent->DecreaseAttributeValue(AttributeTags::Mana, ManaCost);
        AttributeComponent->DecreaseAttributeValue(AttributeTags::Stamina, StaminaCost);
        
        // Yeteneği gerçekleştir
        ExecuteAbility();
    }
}
```

## API Referansı

### UAttributeComponent

| Metot | Açıklama |
|--------|-------------|
| `LoadAttributesFromDataAsset` | Atanmış data asset'ten öznitelikleri yükler |
| `GetAttributeValue` | Bir özniteliğin mevcut değerini döndürür |
| `SetAttributeValue` | Belirtilen işlemle bir öznitelik değerini ayarlar |
| `DecreaseAttributeValue` | Bir özniteliği belirtilen miktarda azaltır |
| `IncreaseAttributeValue` | Bir özniteliği belirtilen miktarda artırır |
| `GetAttributeNormalized` | Öznitelik değerini aralığının yüzdesi olarak (0-1) döndürür |
| `AddAttribute` | Bileşene yeni bir öznitelik ekler |
| `RemoveAttribute` | Bileşenden bir özniteliği kaldırır |
| `ApplyModifier` | Bir özniteliğe geçici veya kalıcı bir modifikatör uygular |
| `RemoveModifier` | Bir öznitelikten belirli bir modifikatörü kaldırır |
| `DecreaseHealth` | Sağlığı azaltmak için kolaylık metodu |
| `IncreaseHealth` | Sağlığı artırmak için kolaylık metodu |
| `SaveAttributes` | Tüm öznitelik değerlerini bir slota kaydeder |
| `LoadAttributes` | Bir slottan öznitelik değerlerini yükler |

### FAttribute Yapısı

| Özellik | Açıklama |
|----------|-------------|
| `AttributeTag` | Özniteliği tanımlayan GameplayTag |
| `Value` | Özniteliğin mevcut değeri |
| `MinValue` | Olası minimum değer |
| `MaxValue` | Olası maksimum değer |
| `bUseRegen` | Özniteliğin zamanla yenilenip yenilenmediği |
| `RegenRate` | Yenilenme tikleri arasındaki saniye cinsinden süre |
| `RegenValue` | Tik başına yenilenecek miktar |

### Delegate'ler

| Delegate | Açıklama |
|----------|-------------|
| `OnAttributeChanged` | Bir öznitelik değeri değiştiğinde çağrılır |
| `OnAttributeAdded` | Yeni bir öznitelik eklendiğinde çağrılır |
| `OnAttributeRemoved` | Bir öznitelik kaldırıldığında çağrılır |
| `OnAttributeThresholdReached` | Bir öznitelik bir eşiğe ulaştığında çağrılır |
| `OnHealthChanged` | Sağlık değişiklikleri için kolaylık delegate'i |
| `OnDeath` | Sağlık sıfıra ulaştığında çağrılır |

## En İyi Uygulamalar

1. **Birden çok aktörde tutarlı öznitelik yapılandırması için Data Asset'leri kullanın**
2. **Esnek öznitelik tanımlama için GameplayTags'ten yararlanın**
3. **Etiket hiyerarşileri ile öznitelik kategorileri oluşturun** (örn., "Attributes.Primary.Strength")
4. **Öznitelik işlemlerinde sıfıra bölme gibi uç durumları ele alın**
5. **Performans açısından kritik bölümlerde öznitelik güncellemelerini sınırlayın**
6. **Değerleri sürekli kontrol etmek yerine oyun olayları için öznitelik eşiklerini kullanın**
7. **Çok oyunculu oyunlar için ağ replikasyon ihtiyaçlarını göz önünde bulundurun**
8. **İşlevselliği genişletmek için oyuna özgü kolaylık metotları ekleyin**
9. **Takım işbirliği için özniteliklerinizi belgeleyin**

## Lisans

Bu proje MIT Lisansı altında lisanslanmıştır - detaylar için LICENSE dosyasına bakın.