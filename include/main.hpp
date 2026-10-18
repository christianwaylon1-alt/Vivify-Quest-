#pragma once

// Qivify — "bring your quest to life"
// Shared includes and the global mod metadata used across the project.

#include "scotland2/shared/modloader.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "paper2_scotland2/shared/logger.hpp"

// MOD_ID and VERSION are injected by CMake (see CMakeLists.txt). Fall back to
// sane defaults so the headers can also be parsed by tooling that does not run
// the full build system.
#ifndef MOD_ID
#define MOD_ID "qivify"
#endif
#ifndef VERSION
#define VERSION "1.0.0"
#endif

// Entry points exported to scotland2. `extern "C"` keeps the symbol names
// unmangled so the modloader can find `setup` and `late_load`.
#ifndef MOD_EXTERN_FUNC
#define MOD_EXTERN_FUNC extern "C" __attribute__((visibility("default")))
#endif

/// Global mod metadata handed to scotland2 during `setup`.
extern modloader::ModInfo modInfo;

/// Returns the shared configuration instance for Qivify.
Configuration& getConfig();
