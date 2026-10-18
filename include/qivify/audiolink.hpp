#pragma once

#include <string>

namespace Qivify::AudioLink {

/// Absolute path Qivify looks in for the built-in AudioLink asset bundle.
/// Users may also drop their own bundle here.
std::string BundlePath();

/// Loads (or reloads) the AudioLink bundle and instantiates its prefab.
/// Returns true if an AudioLink instance is now live.
bool Load();

/// Destroys the live AudioLink instance, if any.
void Unload();

/// Convenience: Load() when enabled, Unload() when disabled.
void SetEnabled(bool enabled);

/// True if an AudioLink instance is currently active.
bool IsActive();

}  // namespace Qivify::AudioLink
