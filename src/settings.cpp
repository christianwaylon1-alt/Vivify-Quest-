#include "qivify/settings.hpp"

#include "main.hpp"
#include "qivify/logger.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

#include <array>
#include <string>

namespace Qivify::Settings {
namespace {

// Backing store for the current values.
std::array<bool, kCount> g_values{};

// The canonical definition table. Display names are preserved verbatim,
// including the intentional spellings "Mutiplayer" and "Blits".
constexpr std::array<Definition, kCount> kDefinitions{{
    {Id::MakeQivifyLogs, "make_qivify_logs", "Make Qivify Logs",
     "Write Qivify diagnostics to the Beat Saber log file.", false},
    {Id::DisableAllBlits, "disable_all_blits", "Disable All Blits",
     "Disable every texture blit/copy pass Qivify would otherwise perform.", false},
    {Id::DisableCustomMapNotes, "disable_custom_map_notes", "Disable Custom Map Notes",
     "Skip loading custom note assets so the stock notes are always used.", false},
    {Id::DisableCustomMapSabers, "disable_custom_map_sabers", "Disable Custom Map Sabers",
     "Skip loading custom saber assets so the stock sabers are always used.", false},
    {Id::DisableQivifyVisualsInMultiplayer, "disable_qivify_visuals_in_mutiplayer",
     "Disable Qivify Visuals inside of Mutiplayer",
     "Turn off Qivify visual replacements while in a multiplayer lobby.", false},
    {Id::RemakeGeoShadersIntoNormalMesh, "remake_geoshaders_into_normal_mesh",
     "Remake GeoShaders Into Normal Mesh",
     "Replace geometry-shader effects with equivalent normal-mesh geometry.", true},
    {Id::DisableOneTextureBlits, "disable_one_texture_blits", "Disable One Texture Blits",
     "Avoid single-texture blit operations that are expensive on Adreno GPUs.", false},
    {Id::EnableStandInShading, "enable_stand_in_shading", "Enable Stand-In Shading",
     "Use lightweight stand-in shaders when a custom shader cannot be converted.", true},
    {Id::AutomaticShaderDetection, "automatic_shader_detection", "Automatic Shader Detection",
     "Automatically detect shaders that are unsupported on Quest.", true},
    {Id::SmartShaderReplacement, "smart_shader_replacement", "Smart Shader Replacement",
     "Replace unsupported shaders with the closest supported equivalent.", true},
    {Id::PreserveEmissionAndTransparency, "preserve_emission_and_transparency",
     "Preserve Emission and Transparency",
     "Keep emission colour and alpha/transparency when converting materials.", true},
    {Id::ConvertUnsupportedMaterials, "convert_unsupported_materials", "Convert Unsupported Materials",
     "Convert materials that reference unsupported shaders.", true},
    {Id::ConvertUnsupportedParticleSystems, "convert_unsupported_particle_systems",
     "Convert Unsupported Particle Systems",
     "Convert particle systems that use unsupported shaders.", true},
    {Id::DisablePostProcessingEffects, "disable_post_processing_effects", "Disable Post-Processing Effects",
     "Disable heavy post-processing effects on custom maps.", false},
    {Id::DisableCustomMapLighting, "disable_custom_map_lighting", "Disable Custom Map Lighting",
     "Ignore custom map lighting and use the default environment lighting.", false},
    {Id::ConvertPCVRBundles, "convert_pcvr_bundles", "Convert PCVR Bundles",
     "Attempt to convert PC-VR asset bundles for use on Quest.", true},
    {Id::ConvertOnlyBundlesWithUnsupportedShaders, "convert_only_bundles_with_unsupported_shaders",
     "Convert Only Bundles With Unsupported Shaders",
     "Only run the conversion pass on bundles that actually contain unsupported shaders.", true},
    {Id::RepairMissingShaderReferences, "repair_missing_shader_references", "Repair Missing Shader References",
     "Repair materials that reference a shader which failed to load.", true},
    {Id::RepairMissingTextures, "repair_missing_textures", "Repair Missing Textures",
     "Substitute a safe placeholder for textures that failed to load.", true},
    {Id::FixAssetDependencyReferences, "fix_asset_dependency_references", "Fix Asset Dependency References",
     "Repair broken cross-asset dependency references inside bundles.", true},
    {Id::PreserveOriginalAssetBundles, "preserve_original_asset_bundles", "Preserve Original Asset Bundles",
     "Keep the original, unmodified asset bundles on disk.", true},
    {Id::ConvertUnsupportedComponents, "convert_unsupported_components", "Convert Unsupported Components",
     "Strip or convert components that are unsupported on Quest.", true},
    {Id::AssetBundleCompatibilityReport, "asset_bundle_compatibility_report",
     "Asset Bundle Compatibility Report",
     "Write a per-bundle compatibility report after scanning.", false},
    {Id::DisableGrayBoxFallbacks, "disable_gray_box_fallbacks", "Disable Gray Box Fallbacks",
     "Prevent Unity from substituting the gray 'missing' box fallback.", true},
    {Id::PreventCameraFollowingObjects, "prevent_camera_following_objects", "Prevent Camera-Following Objects",
     "Stop stray objects from being parented to and following the player camera.", true},
    {Id::ValidateObjectTransforms, "validate_object_transforms", "Validate Object Transforms",
     "Validate object transforms and reject NaN/invalid values.", true},
    {Id::BlockInvalidShaderReplacements, "block_invalid_shader_replacements", "Block Invalid Shader Replacements",
     "Reject shader replacements that would leave a material invalid.", true},
    {Id::PreventMissingTextureBoxes, "prevent_missing_texture_boxes", "Prevent Missing-Texture Boxes",
     "Prevent the pink/gray boxes shown when a texture is missing.", true},
    {Id::PreserveOriginalMeshGeometry, "preserve_original_mesh_geometry", "Preserve Original Mesh Geometry",
     "Never alter the geometry of original meshes.", true},
    {Id::ValidateBeforeRendering, "validate_before_rendering", "Validate Before Rendering",
     "Validate objects before they are rendered and skip invalid ones.", true},
    {Id::HideInvalidDebugGeometry, "hide_invalid_debug_geometry", "Hide Invalid Debug Geometry",
     "Hide invalid or leftover debug geometry.", true},
    {Id::PreserveWorldSpaceRendering, "preserve_world_space_rendering", "Preserve World-Space Rendering",
     "Keep world-space rendering intact for converted objects.", true},
    {Id::DetectAndLogGrayBoxSymptoms, "detect_and_log_gray_box_symptoms", "Detect and Log Gray-Box Symptoms",
     "Detect and log the symptoms that lead to gray boxes.", true},
    {Id::DisableExtraCameraBoxesFollowingScreen, "disable_extra_camera_boxes_following_screen",
     "Disable Extra Camera Boxes Following Screen",
     "Disable extra boxes that follow the headset camera (the priority fix).", true},
    {Id::AdaptiveEffectsQuality, "adaptive_effects_quality", "Adaptive Effects Quality",
     "Reduce effect quality adaptively when frame time rises.", true},
    {Id::PreventDuplicateConversions, "prevent_duplicate_conversions", "Prevent Duplicate Conversions",
     "Never convert the same asset bundle more than once.", true},
    {Id::AssetConversionCache, "asset_conversion_cache", "Asset Conversion Cache",
     "Cache conversion results so repeat loads are instant.", true},
    {Id::RuntimeMemoryProtection, "runtime_memory_protection", "Runtime Memory Protection",
     "Guard against excessive memory use during conversion.", true},
    {Id::MainThreadSafeAssetConversion, "main_thread_safe_asset_conversion", "Main-Thread Safe Asset Conversion",
     "Only touch Unity objects from the main thread.", true},
    {Id::QuestGPUCompatibilityCheck, "quest_gpu_compatibility_check", "Quest GPU Compatibility Check",
     "Check that converted assets are compatible with the Quest GPU.", true},
    {Id::SafeFallbackMode, "safe_fallback_mode", "Safe Fallback Mode",
     "Fall back to a safe configuration if a conversion fails.", true},
    {Id::ConversionFailureLogging, "conversion_failure_logging", "Conversion Failure Logging",
     "Log detailed information when a conversion fails.", true},
    {Id::EnableAudioLink, "enable_audiolink", "Enable AudioLink",
     "Load and enable the built-in AudioLink bundle.", false},
}};

}  // namespace

const std::array<Definition, kCount>& Definitions() { return kDefinitions; }

bool Get(Id id) { return g_values[static_cast<std::size_t>(id)]; }

void Set(Id id, bool value, bool persist) {
    g_values[static_cast<std::size_t>(id)] = value;
    if (persist) {
        Save();
    }
}

void Load() {
    auto& cfg = getConfig();
    cfg.Load();
    auto& doc = cfg.config;

    for (auto const& def : kDefinitions) {
        std::string key(def.key);
        bool value = def.defaultValue;
        if (doc.IsObject() && doc.HasMember(key.c_str()) && doc[key.c_str()].IsBool()) {
            value = doc[key.c_str()].GetBool();
        }
        g_values[static_cast<std::size_t>(def.id)] = value;
    }

    // Make sure the file exists and contains every key.
    Save();
}

void Save() {
    auto& cfg = getConfig();
    auto& doc = cfg.config;
    auto& alloc = doc.GetAllocator();

    if (!doc.IsObject()) {
        doc.SetObject();
    }

    for (auto const& def : kDefinitions) {
        std::string key(def.key);
        bool value = g_values[static_cast<std::size_t>(def.id)];
        if (doc.HasMember(key.c_str())) {
            doc[key.c_str()].SetBool(value);
        } else {
            rapidjson::Value nameVal(key.c_str(), alloc);
            doc.AddMember(nameVal, value, alloc);
        }
    }

    cfg.Write();
}

bool LoggingEnabled() { return Get(Id::MakeQivifyLogs); }

void ResetToDefaults() {
    for (auto const& def : kDefinitions) {
        g_values[static_cast<std::size_t>(def.id)] = def.defaultValue;
    }
    Save();
}

}  // namespace Qivify::Settings
