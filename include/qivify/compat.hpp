#pragma once

#include "qivify/settings.hpp"

#include <string>

namespace Qivify::Compat {

/// Installs every compatibility hook. Called from `late_load()`.
void InstallHooks();

/// Called whenever a setting changes so the engine can react immediately.
void OnSettingChanged(Settings::Id id, bool value);

/// Re-runs the asset-bundle compatibility pass over every known bundle.
/// Backs the "Reconvert All AssetBundle Maps" button.
void ReconvertAllAssetBundleMaps();

/// Emits a compatibility report for all tracked bundles.
void EmitCompatibilityReport();

/// Runs a single camera-box sweep, disabling gray boxes parented to the camera.
void SweepCameraBoxes();

/// Number of asset bundles tracked so far.
int TrackedBundleCount();

/// Number of assets converted so far.
int ConversionCount();

/// Returns true if the given shader name is known to be unsupported on Quest.
bool IsUnsupportedShaderName(std::string const& name);

}  // namespace Qivify::Compat
