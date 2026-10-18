#include "main.hpp"

#include "qivify/audiolink.hpp"
#include "qivify/compat.hpp"
#include "qivify/logger.hpp"
#include "qivify/settings.hpp"
#include "qivify/ui.hpp"

#include "scotland2/shared/modloader.h"

#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

#include "bsml/shared/BSML.hpp"

using namespace Qivify;

// Global mod metadata, consumed by scotland2 and the config system.
modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

// The single shared configuration instance for Qivify.
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// ---------------------------------------------------------------------------
// scotland2 entry points
// ---------------------------------------------------------------------------

MOD_EXTERN_FUNC void setup(CModInfo* info) noexcept {
    // Record the metadata scotland2 resolved for us.
    modInfo.assign(*info);

    // Register our paper log context so Qivify lines are written to the log.
    Paper::Logger::RegisterFileContextId(Qivify::Log.tag);

    Qivify::Log.info("Qivify {} ({}) is setting up \u2014 bring your quest to life.", modInfo.version, modInfo.id);
}

MOD_EXTERN_FUNC void late_load() noexcept {
    Qivify::Log.info("Qivify late_load starting...");

    // Bring up il2cpp and load persisted settings.
    il2cpp_functions::Init();
    Settings::Load();

    // Initialise BSML and register the single "Qivify" settings category.
    BSML::Init();
    UI::RegisterMenu();

    // Install the compatibility hooks that back the 42 settings.
    Compat::InstallHooks();

    // Start AudioLink if the user left it enabled.
    if (Settings::Get(Settings::Id::EnableAudioLink)) {
        AudioLink::Load();
    }

    Qivify::Log.info("Qivify late_load complete.");
}
