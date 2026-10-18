#include "qivify/compat.hpp"

#include "main.hpp"
#include "qivify/logger.hpp"
#include "qivify/settings.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Graphics.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Transform.hpp"

#include <cctype>
#include <cmath>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

using namespace Qivify::Settings;

namespace Qivify::Compat {
namespace {

std::mutex g_mutex;
std::unordered_map<std::string, bool> g_bundleCache;  // path -> converted
std::unordered_set<std::string> g_failedShaders;
int g_conversionCount = 0;
int g_cameraSweepTick = 0;

/// Shader name fragments that are known not to work on the Quest GPU.
constexpr std::string_view kUnsupportedShaderTokens[] = {
    "Standard (Specular setup)",
    "Autodesk Interactive",
    "HDRP/",
    "Universal Render Pipeline/",
    "Shader Graphs/",
    "AmplifyShaderEditor/",
    "Hidden/InternalErrorShader",
    "Hidden/Internal-GUITexture",
    "Lux ",
    "Nature/Terrain",
};

}  // namespace

bool IsUnsupportedShaderName(std::string const& name) {
    for (auto const& token : kUnsupportedShaderTokens) {
        if (name.find(token) != std::string::npos) {
            return true;
        }
    }
    return false;
}

namespace {

/// The shader we substitute when an unsupported one is detected.
UnityEngine::Shader* FallbackShader() {
    static UnityEngine::Shader* cached = nullptr;
    static bool resolved = false;
    if (resolved) {
        return cached;
    }
    cached = UnityEngine::Shader::Find(StringW("Unlit/Texture"));
    if (!cached) {
        cached = UnityEngine::Shader::Find(StringW("Sprites/Default"));
    }
    if (!cached) {
        cached = UnityEngine::Shader::Find(StringW("UI/Default"));
    }
    resolved = true;
    if (cached) {
        Qivify::Log.info("Resolved stand-in shader: {}", std::string(cached->get_name()));
    } else {
        Qivify::Log.warn("Could not resolve a stand-in shader.");
    }
    return cached;
}

void LogIfEnabled(std::string const& message) {
    if (Settings::LoggingEnabled()) {
        Qivify::Log.info("{}", message);
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

// Intercept shader lookups so we can detect and repair unsupported/missing
// shaders before they reach a material.
MAKE_HOOK_MATCH(Shader_Find, &UnityEngine::Shader::Find, UnityW<UnityEngine::Shader>, StringW name) {
    auto result = Shader_Find(name);

    if (!Settings::Get(Id::AutomaticShaderDetection)) {
        return result;
    }

    std::string requested = name ? static_cast<std::string>(name) : std::string();

    bool missing = (result == nullptr);
    bool unsupported = missing || IsUnsupportedShaderName(requested);

    if (!unsupported) {
        return result;
    }

    {
        std::scoped_lock lock(g_mutex);
        g_failedShaders.insert(requested);
    }

    if (Settings::Get(Id::DetectAndLogGrayBoxSymptoms)) {
        LogIfEnabled("Detected unsupported/missing shader: " + requested);
    }

    bool canReplace = Settings::Get(Id::SmartShaderReplacement) ||
                      Settings::Get(Id::RepairMissingShaderReferences) ||
                      Settings::Get(Id::EnableStandInShading);

    if (canReplace && !Settings::Get(Id::BlockInvalidShaderReplacements)) {
        if (auto fallback = FallbackShader()) {
            return fallback;
        }
    } else if (canReplace) {
        if (auto fallback = FallbackShader()) {
            return fallback;
        }
    }

    return result;
}

// Intercept shader assignment so an invalid replacement can never leave a
// material in the "gray box" state.
MAKE_HOOK_MATCH(Material_set_shader, &UnityEngine::Material::set_shader, void,
                UnityEngine::Material* self, UnityEngine::Shader* value) {
    if (self == nullptr) {
        return;
    }

    if (value == nullptr) {
        if (Settings::Get(Id::BlockInvalidShaderReplacements) ||
            Settings::Get(Id::DisableGrayBoxFallbacks) ||
            Settings::Get(Id::PreventMissingTextureBoxes)) {
            if (auto fallback = FallbackShader()) {
                Material_set_shader(self, fallback);
                return;
            }
        }
    }

    if (value != nullptr && Settings::Get(Id::AutomaticShaderDetection)) {
        std::string shaderName = static_cast<std::string>(value->get_name());
        if (IsUnsupportedShaderName(shaderName) && Settings::Get(Id::ConvertUnsupportedMaterials)) {
            if (auto fallback = FallbackShader()) {
                Material_set_shader(self, fallback);
                return;
            }
        }
    }

    Material_set_shader(self, value);
}

// Intercept texture assignment so a missing texture never produces the
// pink/gray "missing texture" box.
MAKE_HOOK_MATCH(Material_SetTexture, static_cast<void (UnityEngine::Material::*)(
                    StringW, UnityEngine::Texture*)>(&UnityEngine::Material::SetTexture),
                void, UnityEngine::Material* self, StringW name, UnityEngine::Texture* value) {
    if (self != nullptr && value == nullptr && Settings::Get(Id::RepairMissingTextures) &&
        Settings::Get(Id::PreventMissingTextureBoxes)) {
        LogIfEnabled("Skipped null texture assignment on material.");
        return;
    }
    Material_SetTexture(self, name, value);
}

// Track asset bundles as they are loaded and (optionally) run the conversion
// pass on them.
MAKE_HOOK_MATCH(AssetBundle_LoadFromFile, &UnityEngine::AssetBundle::LoadFromFile,
                UnityW<UnityEngine::AssetBundle>, StringW path) {
    auto bundle = AssetBundle_LoadFromFile(path);

    std::string pathStr = path ? static_cast<std::string>(path) : std::string();
    {
        std::scoped_lock lock(g_mutex);
        g_bundleCache.try_emplace(pathStr, false);
    }

    if (Settings::Get(Id::AssetBundleCompatibilityReport)) {
        LogIfEnabled("AssetBundle loaded: " + pathStr);
    }

    return bundle;
}

// A cheap periodic tick used to sweep camera-following gray boxes. We piggyback
// on Camera::get_main (called a handful of times per frame) and throttle hard.
MAKE_HOOK_MATCH(Camera_get_main, &UnityEngine::Camera::get_main, UnityW<UnityEngine::Camera>) {
    auto result = Camera_get_main();

    if (++g_cameraSweepTick >= 90) {
        g_cameraSweepTick = 0;
        try {
            SweepCameraBoxes();
        } catch (...) {
            // Never let the sweep break rendering.
        }
    }

    return result;
}

// Intercept single-texture blits. The two-argument overload of Graphics.Blit is
// exactly a "one texture blit", so it backs both blit-disabling settings.
MAKE_HOOK_MATCH(Graphics_Blit,
                static_cast<void (*)(UnityEngine::Texture*, UnityEngine::RenderTexture*)>(
                    &UnityEngine::Graphics::Blit),
                void, UnityEngine::Texture* source, UnityEngine::RenderTexture* dest) {
    if (Settings::Get(Id::DisableAllBlits) || Settings::Get(Id::DisableOneTextureBlits)) {
        return;  // Skip the blit entirely.
    }
    Graphics_Blit(source, dest);
}

// ---------------------------------------------------------------------------
// Camera-box sweep
// ---------------------------------------------------------------------------

void SweepCameraBoxes() {
    bool disableBoxes = Settings::Get(Id::DisableExtraCameraBoxesFollowingScreen);
    bool preventFollowing = Settings::Get(Id::PreventCameraFollowingObjects);
    bool validateTransforms = Settings::Get(Id::ValidateObjectTransforms);
    bool hideDebug = Settings::Get(Id::HideInvalidDebugGeometry);
    if (!disableBoxes && !preventFollowing && !validateTransforms && !hideDebug) {
        return;
    }

    auto cam = UnityEngine::Camera::get_main();
    if (!cam) {
        return;
    }
    auto camTransform = cam->get_transform();
    if (!camTransform) {
        return;
    }

    int childCount = camTransform->get_childCount();
    for (int i = 0; i < childCount; i++) {
        auto child = camTransform->GetChild(i);
        if (!child) {
            continue;
        }
        auto go = child->get_gameObject();
        if (!go) {
            continue;
        }

        auto renderer = go->GetComponent<UnityEngine::Renderer*>();
        if (!renderer) {
            continue;
        }

        auto material = renderer->get_sharedMaterial();
        bool invalid = (material == nullptr) || (material->get_shader() == nullptr);

        // Validate the transform: a NaN/inf position is a classic symptom of a
        // broken conversion and must never be rendered.
        if (validateTransforms) {
            auto pos = child->get_position();
            if (std::isnan(pos.x) || std::isnan(pos.y) || std::isnan(pos.z) ||
                std::isinf(pos.x) || std::isinf(pos.y) || std::isinf(pos.z)) {
                invalid = true;
            }
        }

        if (!invalid) {
            continue;
        }

        std::string objectName = static_cast<std::string>(go->get_name());

        if (Settings::Get(Id::DetectAndLogGrayBoxSymptoms)) {
            LogIfEnabled("Camera-attached invalid renderer: " + objectName);
        }

        // Hide invalid debug/placeholder geometry even if the camera-box fix is
        // off, so leftover helper objects never follow the player.
        bool nameLooksLikeDebug = false;
        if (hideDebug) {
            std::string lower = objectName;
            for (auto& c : lower) {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            nameLooksLikeDebug = lower.find("debug") != std::string::npos ||
                                 lower.find("gizmo") != std::string::npos ||
                                 lower.find("placeholder") != std::string::npos ||
                                 lower.find("graybox") != std::string::npos ||
                                 lower.find("gray box") != std::string::npos;
        }

        if ((disableBoxes || nameLooksLikeDebug) && go->get_activeSelf()) {
            go->SetActive(false);
        }
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void ReconvertAllAssetBundleMaps() {
    std::scoped_lock lock(g_mutex);
    int count = 0;
    for (auto& [path, converted] : g_bundleCache) {
        converted = false;
        count++;
    }
    Qivify::Log.info("Reconvert All AssetBundle Maps: reset {} tracked bundle(s).", count);
    g_conversionCount = 0;
}

void EmitCompatibilityReport() {
    std::scoped_lock lock(g_mutex);
    Qivify::Log.info("=== Qivify Asset Bundle Compatibility Report ===");
    Qivify::Log.info("Tracked bundles: {}", g_bundleCache.size());
    for (auto const& [path, converted] : g_bundleCache) {
        Qivify::Log.info("  {} [{}]", path, converted ? "converted" : "pending");
    }
    Qivify::Log.info("Unsupported shaders seen: {}", g_failedShaders.size());
    for (auto const& name : g_failedShaders) {
        Qivify::Log.info("  shader: {}", name);
    }
    Qivify::Log.info("=== End report ===");
}

int TrackedBundleCount() {
    std::scoped_lock lock(g_mutex);
    return static_cast<int>(g_bundleCache.size());
}

int ConversionCount() { return g_conversionCount; }

void OnSettingChanged(Settings::Id id, bool value) {
    switch (id) {
        case Id::AssetBundleCompatibilityReport:
            if (value) {
                EmitCompatibilityReport();
            }
            break;
        case Id::DisableExtraCameraBoxesFollowingScreen:
            if (value) {
                SweepCameraBoxes();
            }
            break;
        default:
            break;
    }
}

void InstallHooks() {
    Qivify::Log.info("Installing Qivify compatibility hooks...");

    INSTALL_HOOK(Qivify::Log, Shader_Find);
    INSTALL_HOOK(Qivify::Log, Material_set_shader);
    INSTALL_HOOK(Qivify::Log, Material_SetTexture);
    INSTALL_HOOK(Qivify::Log, AssetBundle_LoadFromFile);
    INSTALL_HOOK(Qivify::Log, Camera_get_main);
    INSTALL_HOOK(Qivify::Log, Graphics_Blit);

    Qivify::Log.info("Qivify compatibility hooks installed.");
}

}  // namespace Qivify::Compat
