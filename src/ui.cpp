#include "qivify/ui.hpp"

#include "main.hpp"
#include "qivify/audiolink.hpp"
#include "qivify/compat.hpp"
#include "qivify/logger.hpp"
#include "qivify/settings.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML-Lite.hpp"

#include "HMUI/ViewController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

using namespace Qivify::Settings;

namespace Qivify::UI {
namespace {

bool g_built = false;

void AddToggle(UnityEngine::Transform* parent, Definition const& def) {
    auto* toggle = BSML::Lite::CreateToggle(
        parent, StringW(def.displayName), Get(def.id),
        [id = def.id](bool value) {
            Set(id, value);
            Compat::OnSettingChanged(id, value);
            if (id == Id::EnableAudioLink) {
                AudioLink::SetEnabled(value);
            }
        });

    if (toggle != nullptr) {
        BSML::Lite::AddHoverHint(toggle, StringW(def.tooltip));
    }
}

void BuildMenu(HMUI::ViewController* viewController, bool firstActivation, bool /*addedToHierarchy*/,
               bool /*screenSystemEnabling*/) {
    if (viewController == nullptr) {
        return;
    }
    if (!firstActivation && g_built) {
        return;
    }

    auto* container = BSML::Lite::CreateScrollableSettingsContainer(viewController->get_transform());
    if (container == nullptr) {
        Qivify::Log.error("Failed to create the Qivify settings container.");
        return;
    }
    auto parent = container->get_transform();

    BSML::Lite::CreateText(parent, StringW("Qivify 1.0.0 \u2014 bring your quest to life"), 5.0f);
    BSML::Lite::CreateText(parent,
                           StringW("Asset-bundle compatibility & rendering repair for Beat Saber Quest 1.40.8"),
                           3.5f);

    // Every setting (the 42 core settings, then the AudioLink extension) is
    // rendered as a real, functional toggle in one flat list.
    for (auto const& def : Definitions()) {
        AddToggle(parent, def);
    }

    // Action buttons.
    BSML::Lite::CreateUIButton(parent, StringW("Reconvert All AssetBundle Maps"), []() {
        Compat::ReconvertAllAssetBundleMaps();
    });
    BSML::Lite::CreateUIButton(parent, StringW("Write Asset Bundle Compatibility Report"), []() {
        Compat::EmitCompatibilityReport();
    });
    BSML::Lite::CreateUIButton(parent, StringW("Reset Qivify Settings To Defaults"), []() {
        Settings::ResetToDefaults();
    });

    g_built = true;
}

}  // namespace

void RegisterMenu() {
    bool ok = BSML::Register::RegisterSettingsMenu("Qivify", BuildMenu, false);
    if (ok) {
        Qivify::Log.info("Registered the Qivify settings menu.");
    } else {
        Qivify::Log.error("Failed to register the Qivify settings menu.");
    }
}

}  // namespace Qivify::UI
