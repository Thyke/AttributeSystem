# Attribute System for Unreal Engine

A robust, modular, and data-driven Attribute System for Unreal Engine. Built on **GameplayTags**, this system handles complex attribute logic, modifiers, temporary buffs/debuffs, and automatic regeneration with ease. It is designed to be production-ready, thread-safe, and fully exposed to Blueprints.

> **Copyright (C) Thyke. All Rights Reserved.**

-----

## 🇬🇧 English Documentation

### Features

  * **GameplayTag Based:** Uses `FGameplayTag` for efficient attribute identification and lookup. No hardcoded strings or enums.
  * **Data-Driven Design:** Define attributes (Health, Mana, Strength, etc.) using `UAttributeDefinition` Data Assets.
  * **Runtime Modification:** Add or remove attributes dynamically at runtime.
  * **Advanced Modifier System:**
      * Supports **Additive**, **Multiplicative**, **Divide**, and **Override** operations.
      * **Priority System** for calculation order.
      * **Temporary Modifiers** with automatic duration handling (Buffs/Debuffs).
  * **Automatic Regeneration:** Built-in support for `PerSecond`, `PerTick`, or `Delayed` (e.g., shield regen after damage stops) regeneration logic.
  * **Smart Clamping:** Attributes can be clamped by min/max values or **clamped to another attribute** (e.g., Current Health clamped to Max Health).
  * **Blueprint Function Library:** Extensive library for easy access to values, checking costs, and applying changes without C++.
  * **Event System:** Rich delegates for UI updates (`OnAttributeChanged`, `OnAttributeReachedZero`, `OnAttributeReachedMax`, etc.).

### Installation

1.  Copy the `AttributeSystem` folder into your project's `Plugins` or `Source` directory.
2.  Add `"AttributeSystem"` to your `.uproject` file or `PublicDependencyModuleNames` in your project's `.Build.cs` file:
    ```csharp
    PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTags", "AttributeSystem" });
    ```
3.  Regenerate project files and compile.

### Getting Started

#### 1\. Define an Attribute

Create a Data Asset derived from `AttributeDefinition`.

  * **Right Click in Content Browser** -\> Miscellaneous -\> Data Asset -\> `AttributeDefinition`.
  * **Attribute Tag:** `Attribute.Health`
  * **Type:** Resource (for HP/Mana) or Flat (for Strength/Agility).
  * **Clamp Config:** Enable Max Clamp and check "Clamp To Another Attribute" if needed.

#### 2\. Add the Component

Add the `AttributeSystemComponent` to your Character or Actor.

```cpp
// In your Character's constructor
AttributeComponent = CreateDefaultSubobject<UAttributeSystemComponent>(TEXT("AttributeComponent"));
```

#### 3\. Initialize Attributes

In your Actor's `BeginPlay` or via the Component's details panel, assign the Default Attributes list.

### Usage Examples

#### Accessing Values (C++)

```cpp
// Get current value
float Health = AttributeComponent->GetAttributeCurrentValue(Tag_Health);

// Get value as percentage (0.0 to 1.0)
float HealthPercent = AttributeComponent->GetAttributePercent(Tag_Health);
```

#### Modifying Values (Blueprint / C++)

You can use the `AttributeSystemBlueprintLibrary` for quick access:

  * **Apply Damage:** `ModifyActorAttribute(Actor, Tag_Health, -10.0f)`
  * **Heal:** `ModifyActorAttribute(Actor, Tag_Health, +20.0f)`
  * **Set Value:** `SetActorAttributeValue(Actor, Tag_Health, 100.0f)`

#### Applying Modifiers (Buffs)

To create a temporary buff (e.g., +50 Strength for 10 seconds):

```cpp
FAttributeModifier StrBuff = UAttributeSystemBlueprintLibrary::MakeTemporaryModifier(
    Tag_Strength_Modifier, 
    EAttributeModifierOperation::Add, 
    50.0f, 
    10.0f // Duration
);

AttributeComponent->AddModifier(Tag_Strength, StrBuff);
```

### Regeneration System

Regeneration is handled automatically if configured in the `AttributeDefinition` Data Asset.

  * **Regen Type:** Per Second, Per Tick, or Delayed.
  * **Pause on Damage:** Useful for "Shield" type attributes that only regen when not taking damage.

-----

-----

## 🇹🇷 Türkçe Dokümantasyon

# Thyke Öznitelik Sistemi

Unreal Engine için geliştirilmiş sağlam, modüler ve veri odaklı (data-driven) bir Öznitelik (Attribute) Sistemi. **GameplayTags** üzerine kurulu olan bu sistem; karmaşık stat mantığını, değiştiricileri (modifiers), süreli güçlendirmeleri (buffs) ve otomatik yenilenme mekaniklerini kolayca yönetir.

### Özellikler

  * **GameplayTag Tabanlı:** Öznitelikleri tanımlamak için `FGameplayTag` kullanır. Hardcode string veya enum karmaşası yoktur.
  * **Veri Odaklı Tasarım:** Can, Mana, Güç gibi öznitelikleri `UAttributeDefinition` Data Asset'leri üzerinden tanımlarsınız.
  * **Runtime Yönetimi:** Oyun çalışırken dinamik olarak öznitelik ekleyip çıkarabilirsiniz.
  * **Gelişmiş Modifier (Değiştirici) Sistemi:**
      * **Toplama, Çarpma, Bölme** ve **Üzerine Yazma (Override)** işlemlerini destekler.
      * Hesaplamalar için **Öncelik (Priority)** sistemi.
      * Otomatik süre takibi yapan **Geçici Modifierlar** (Buff/Debuff sistemleri için).
  * **Otomatik Yenilenme (Regen):** Saniye başı, Tick başı veya Hasar sonrası gecikmeli yenilenme (örn: Halo tarzı kalkanlar) desteği.
  * **Akıllı Sınırlama (Clamping):** Öznitelikler Min/Max değerlere veya **başka bir özniteliğe** (Örn: Mevcut Can'ın Maksimum Can'ı geçememesi) kilitlenebilir.
  * **Blueprint Kütüphanesi:** C++ yazmadan değerlere erişmek, maliyet kontrolü yapmak ve değer değiştirmek için geniş kapsamlı BP kütüphanesi.
  * **Event Sistemi:** UI güncellemeleri için `OnAttributeChanged`, `OnAttributeReachedZero` gibi detaylı delegeler.

### Kurulum

1.  `AttributeSystem` klasörünü projenizin `Plugins` veya `Source` klasörüne kopyalayın.
2.  Projenizin `.Build.cs` dosyasına `"AttributeSystem"` modülünü ekleyin:
    ```csharp
    PublicDependencyModuleNames.AddRange(new string[] { "Core", "GameplayTags", "AttributeSystem" });
    ```
3.  Projeyi yeniden derleyin (Compile).

### Başlangıç

#### 1\. Öznitelik Tanımlama (Data Asset)

`AttributeDefinition` sınıfından türetilmiş bir Data Asset oluşturun.

  * **Content Browser'da Sağ Tık** -\> Miscellaneous -\> Data Asset -\> `AttributeDefinition`.
  * **Attribute Tag:** `Attribute.Health` (Can)
  * **Type:** Resource (Can/Mana gibi harcanabilir) veya Flat (Güç/Defans gibi sabit).
  * **Regen Config:** İstenirse otomatik yenilenme ayarlarını buradan yapabilirsiniz.

#### 2\. Komponenti Ekleme

Karakterinize `AttributeSystemComponent` ekleyin.

```cpp
// Karakterin constructor'ında
AttributeComponent = CreateDefaultSubobject<UAttributeSystemComponent>(TEXT("AttributeComponent"));
```

#### 3\. Öznitelikleri Başlatma

Aktörünüzün `BeginPlay` fonksiyonunda veya Editör üzerinden Component detaylarında "Default Attributes" listesini doldurarak başlangıç özniteliklerini belirleyin.

### Kullanım Örnekleri

#### Değerlere Erişim (Blueprint & C++)

```cpp
// Mevcut değeri al
float Health = AttributeComponent->GetAttributeCurrentValue(Tag_Health);

// Yüzdelik değeri al (0.0 ile 1.0 arası, Health Bar için ideal)
float HealthPercent = AttributeComponent->GetAttributePercent(Tag_Health);
```

#### Değer Değiştirme

`AttributeSystemBlueprintLibrary` kullanarak hızlı işlemler yapabilirsiniz:

  * **Hasar Verme:** `ModifyActorAttribute(Actor, Tag_Health, -10.0f)`
  * **İyileştirme:** `ModifyActorAttribute(Actor, Tag_Health, +20.0f)`
  * **Maliyet Kontrolü (Mana yeterli mi?):** `CanAffordAttributeCost` fonksiyonu.

#### Modifier Ekleme (Buff/Debuff)

Geçici bir güçlendirme (Örn: 10 saniyeliğine +50 Güç) oluşturmak için:

```cpp
// Modifier oluştur
FAttributeModifier StrBuff = UAttributeSystemBlueprintLibrary::MakeTemporaryModifier(
    Tag_Strength_Modifier,          // Modifier Tag
    EAttributeModifierOperation::Add, // İşlem Tipi (Ekleme)
    50.0f,                          // Değer
    10.0f                           // Süre (Saniye)
);

// Modifier'ı sisteme ekle
AttributeComponent->AddModifier(Tag_Strength, StrBuff);
```

### Yenilenme Sistemi (Regeneration)

Yenilenme mantığı `AttributeDefinition` içinde ayarlanır ve otomatik çalışır.

  * **Delayed:** Hasar aldıktan sonra belirli bir süre (örn: 3 sn) bekler, sonra yenilenmeye başlar.
  * **Per Second:** Her saniye sabit miktarda yeniler.

-----

### License

This project is licensed under the MIT License - see the LICENSE file for details.