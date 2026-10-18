#include "qivify/audiolink.hpp"

#include "main.hpp"
#include "qivify/logger.hpp"

#include "scotland2/shared/modloader.h"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Transform.hpp"

#include <filesystem>
#include <string>

namespace Qivify::AudioLink {
namespace {

UnityEngine::AssetBundle* g_bundle = nullptr;
UnityEngine::Object* g_instance = nullptr;

// The prefab names we try inside the bundle, in order of preference. The
// canonical AudioLink prefab is simply called "AudioLink".
constexpr char const* kPrefabNames[] = {"AudioLink", "AudioLinkController", "AudioLinkPrefab"};

}  // namespace

std::string BundlePath() {
    // /data/user/0/com.beatgames.beatsaber/files/Qivify/audiolink.bundle
    auto dir = modloader::get_files_dir() / "Qivify";
    return (dir / "audiolink.bundle").string();
}

bool Load() {
    if (g_instance != nullptr) {
        return true;  // already live
    }

    std::string path = BundlePath();
    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) {
        Qivify::Log.warn("AudioLink bundle not found at {}. Drop an AudioLink bundle there to enable it.", path);
        return false;
    }

    g_bundle = UnityEngine::AssetBundle::LoadFromFile(StringW(path));
    if (g_bundle == nullptr) {
        Qivify::Log.error("Failed to load AudioLink bundle: {}", path);
        return false;
    }

    UnityEngine::Object* prefab = nullptr;
    for (auto const* name : kPrefabNames) {
        prefab = g_bundle->LoadAsset(StringW(name));
        if (prefab != nullptr) {
            Qivify::Log.info("AudioLink: found prefab '{}'.", name);
            break;
        }
    }

    if (prefab == nullptr) {
        Qivify::Log.error("AudioLink bundle loaded but no known prefab was found.");
        g_bundle->Unload(false);
        g_bundle = nullptr;
        return false;
    }

    UnityEngine::Object* instance = UnityEngine::Object::Instantiate(prefab);
    if (instance == nullptr) {
        Qivify::Log.error("Failed to instantiate the AudioLink prefab.");
        return false;
    }

    UnityEngine::Object::DontDestroyOnLoad(instance);
    g_instance = instance;

    Qivify::Log.info("AudioLink enabled and instantiated.");
    return true;
}

void Unload() {
    if (g_instance != nullptr) {
        UnityEngine::Object::Destroy(g_instance);
        g_instance = nullptr;
    }
    if (g_bundle != nullptr) {
        g_bundle->Unload(true);
        g_bundle = nullptr;
    }
    Qivify::Log.info("AudioLink disabled.");
}

void SetEnabled(bool enabled) {
    if (enabled) {
        Load();
    } else {
        Unload();
    }
}

bool IsActive() { return g_instance != nullptr; }

}  // namespace Qivify::AudioLink
