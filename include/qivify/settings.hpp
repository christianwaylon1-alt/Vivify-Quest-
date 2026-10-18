#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace Qivify::Settings {

/// Identifier for every user-facing Qivify setting.
/// The order here defines the order they appear in the in-game menu.
enum class Id : int {
    MakeQivifyLogs = 0,
    DisableAllBlits,
    DisableCustomMapNotes,
    DisableCustomMapSabers,
    DisableQivifyVisualsInMultiplayer,
    RemakeGeoShadersIntoNormalMesh,
    DisableOneTextureBlits,
    EnableStandInShading,
    AutomaticShaderDetection,
    SmartShaderReplacement,
    PreserveEmissionAndTransparency,
    ConvertUnsupportedMaterials,
    ConvertUnsupportedParticleSystems,
    DisablePostProcessingEffects,
    DisableCustomMapLighting,
    ConvertPCVRBundles,
    ConvertOnlyBundlesWithUnsupportedShaders,
    RepairMissingShaderReferences,
    RepairMissingTextures,
    FixAssetDependencyReferences,
    PreserveOriginalAssetBundles,
    ConvertUnsupportedComponents,
    AssetBundleCompatibilityReport,
    DisableGrayBoxFallbacks,
    PreventCameraFollowingObjects,
    ValidateObjectTransforms,
    BlockInvalidShaderReplacements,
    PreventMissingTextureBoxes,
    PreserveOriginalMeshGeometry,
    ValidateBeforeRendering,
    HideInvalidDebugGeometry,
    PreserveWorldSpaceRendering,
    DetectAndLogGrayBoxSymptoms,
    DisableExtraCameraBoxesFollowingScreen,
    AdaptiveEffectsQuality,
    PreventDuplicateConversions,
    AssetConversionCache,
    RuntimeMemoryProtection,
    MainThreadSafeAssetConversion,
    QuestGPUCompatibilityCheck,
    SafeFallbackMode,
    ConversionFailureLogging,
    // --- Qivify extensions (beyond the core 42) ---
    EnableAudioLink,
    Count
};

inline constexpr std::size_t kCount = static_cast<std::size_t>(Id::Count);

/// Static description of a single setting.
struct Definition {
    Id id;
    std::string_view key;          ///< stable JSON key used for persistence
    std::string_view displayName;  ///< exact label shown in-game
    std::string_view tooltip;      ///< hover hint
    bool defaultValue;
};

/// All setting definitions, indexed by Id.
const std::array<Definition, kCount>& Definitions();

/// Returns the current value of a setting.
bool Get(Id id);

/// Sets a setting, optionally persisting it to disk immediately.
void Set(Id id, bool value, bool persist = true);

/// Loads settings from the config file (falling back to defaults).
void Load();

/// Writes settings to the config file.
void Save();

/// Convenience helper used by the logging subsystem.
bool LoggingEnabled();

/// Resets every setting to its default value and persists.
void ResetToDefaults();

}  // namespace Qivify::Settings
