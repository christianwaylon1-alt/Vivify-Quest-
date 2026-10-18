# Qivify feature record

This document is an **honest, per-setting breakdown** of what is implemented in
Qivify 1.0.0. It exists so nobody has to guess which of the 42 settings actually
do something at runtime.

Every one of the 42 settings is a **real, persisted, functional toggle**: it is
stored in `files/Qivify.json`, it survives restarts, it appears in the Qivify
menu, and it can be reset to defaults. The tiers below describe how *deeply* each
setting is wired into runtime behaviour in **this** build.

| Tier | Meaning |
|------|---------|
| **A — Wired** | The toggle directly gates or drives a code path that executes at runtime. |
| **B — Partial / heuristic / trivially satisfied** | The toggle is stored and influences behaviour indirectly, or the guarantee it describes already holds because Qivify never performs the risky operation. |
| **C — Stored only** | The toggle persists and displays, but there is no dedicated code path for it in this build yet. |

## Tier A — Wired to runtime behaviour (18 settings)

| # | Setting | What actually happens |
|---|---------|-----------------------|
| 1 | Make Qivify Logs | Gates all Qivify diagnostic logging (`LoggingEnabled()`). |
| 2 | Disable All Blits | `Graphics.Blit` hook returns early, skipping the blit. |
| 7 | Disable One Texture Blits | The 2-arg `Graphics.Blit` (single-texture blit) hook returns early. |
| 8 | Enable Stand-In Shading | Consulted by the `Shader.Find` hook when choosing a fallback shader. |
| 9 | Automatic Shader Detection | Gates shader detection in `Shader.Find` and `Material.set_shader`. |
| 10 | Smart Shader Replacement | Consulted when deciding whether to swap an unsupported shader. |
| 12 | Convert Unsupported Materials | `Material.set_shader` swaps unsupported shaders when set. |
| 18 | Repair Missing Shader References | Consulted by the `Shader.Find` hook for null/missing shaders. |
| 19 | Repair Missing Textures | `Material.SetTexture` hook skips null texture assignments when set. |
| 23 | Asset Bundle Compatibility Report | `AssetBundle.LoadFromFile` hook logs each bundle; button writes a report. |
| 24 | Disable Gray Box Fallbacks | `Material.set_shader` substitutes a fallback instead of a null shader. |
| 25 | Prevent Camera-Following Objects | Enables the camera-child sweep. |
| 26 | Validate Object Transforms | Sweep disables objects whose transform position is NaN/inf. |
| 27 | Block Invalid Shader Replacements | Guards both shader hooks against invalid substitutions. |
| 28 | Prevent Missing-Texture Boxes | Guards `Material.SetTexture` and `Material.set_shader`. |
| 31 | Hide Invalid Debug Geometry | Sweep disables invalid objects whose name looks like debug/placeholder geometry. |
| 33 | Detect and Log Gray-Box Symptoms | Logs detected shader and camera-box symptoms. |
| 34 | **Disable Extra Camera Boxes Following Screen** | **Priority fix.** Sweep disables invalid renderers parented to the main camera. |

## Tier B — Partial / heuristic / trivially satisfied (9 settings)

| # | Setting | Status |
|---|---------|--------|
| 21 | Preserve Original Asset Bundles | Trivially satisfied — Qivify never rewrites bundles on disk. |
| 29 | Preserve Original Mesh Geometry | Trivially satisfied — Qivify never edits meshes. |
| 32 | Preserve World-Space Rendering | Trivially satisfied — Qivify never reparents world-space objects. |
| 36 | Prevent Duplicate Conversions | Backed by the per-bundle cache (`try_emplace`), which de-duplicates tracking. |
| 37 | Asset Conversion Cache | The `g_bundleCache` map is the cache; the Reconvert button clears it. |
| 39 | Main-Thread Safe Asset Conversion | The only Unity-touching pass (the sweep) runs inside a main-thread hook. |
| 40 | Quest GPU Compatibility Check | Backed by the unsupported-shader token list used during detection. |
| 41 | Safe Fallback Mode | Backed by the fallback-shader substitution path. |
| 42 | Conversion Failure Logging | Backed by the `LogIfEnabled` helper used across the engine. |

## Tier C — Stored only (15 settings)

These settings persist and display correctly, and are read by the settings
system, but this build does **not** yet have a dedicated code path that acts on
them. They are the roadmap for the asset-conversion pipeline.

| # | Setting | Why it is not yet wired |
|---|---------|-------------------------|
| 3 | Disable Custom Map Notes | No hook into note-asset loading. |
| 4 | Disable Custom Map Sabers | No hook into saber-asset loading. |
| 5 | Disable Qivify Visuals inside of Mutiplayer | No multiplayer-state detection. |
| 6 | Remake GeoShaders Into Normal Mesh | No geometry-shader remeshing pass. |
| 11 | Preserve Emission and Transparency | Material property copying is not implemented (shaders are swapped, properties are not carried over). |
| 13 | Convert Unsupported Particle Systems | No particle-system conversion pass. |
| 14 | Disable Post-Processing Effects | No post-processing volume hook. |
| 15 | Disable Custom Map Lighting | No lighting hook. |
| 16 | Convert PCVR Bundles | Bundles are tracked but not rewritten/converted. |
| 17 | Convert Only Bundles With Unsupported Shaders | Depends on the conversion pass above. |
| 20 | Fix Asset Dependency References | No dependency-repair pass. |
| 22 | Convert Unsupported Components | No component conversion pass. |
| 30 | Validate Before Rendering | No pre-render validation hook. |
| 35 | Adaptive Effects Quality | No dynamic quality scaling. |
| 38 | Runtime Memory Protection | No memory guard. |

## Extra features (beyond the 42)

| Feature | Status |
|---------|--------|
| **Reconvert All AssetBundle Maps** (button) | Wired — clears the bundle cache and resets the conversion counter. |
| **Enable AudioLink** (extra toggle) | Wired — loads `files/Qivify/audiolink.bundle`, finds a prefab and instantiates it. Requires the user to supply the bundle. |
| **Write Asset Bundle Compatibility Report** (button) | Wired — logs the tracked bundles and unsupported shaders. |
| **Reset Qivify Settings To Defaults** (button) | Wired — restores every setting to its default and persists. |

## Runtime hooks installed

| Hook | Purpose |
|------|---------|
| `UnityEngine.Shader.Find` | Detect unsupported/missing shaders; substitute fallbacks. |
| `UnityEngine.Material.set_shader` | Block invalid shader assignments; convert unsupported materials. |
| `UnityEngine.Material.SetTexture` | Block null texture assignments (missing-texture boxes). |
| `UnityEngine.AssetBundle.LoadFromFile` | Track bundles; feed the compatibility report. |
| `UnityEngine.Camera.get_main` | Throttled trigger for the camera-box sweep. |
| `UnityEngine.Graphics.Blit` | Skip single-texture blits when the blit settings are on. |
