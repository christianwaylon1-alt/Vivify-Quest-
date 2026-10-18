# Qivify

> **bring your quest to life**

Qivify is a from-scratch asset-bundle compatibility and rendering-repair mod for
**Beat Saber on Meta Quest**. It targets Beat Saber **1.40.8** and is built for
the **Android / arm64-v8a** platform using the Scotland2 modloader.

Qivify's job is to make PC-authored custom maps behave on Quest hardware. Custom
maps are frequently built in Unity with desktop shaders, post-processing stacks
and materials that the Adreno GPU on a Quest cannot render. When those assets
fail to load, Unity falls back to the infamous **gray box** — a placeholder cube
that is often parented to the player camera and therefore follows your head
around the level. Qivify detects those situations, substitutes safe stand-in
shaders and textures, and — most importantly — sweeps away the stray
camera-following boxes that ruin a play session.

Everything Qivify does is controlled from **one** in-game settings category
called **Qivify**, containing **42 toggles** plus an AudioLink extension and a
handful of action buttons.

---

## Table of contents

- [Features](#features)
- [The 42 settings](#the-42-settings)
- [Extra features](#extra-features)
- [Installation](#installation)
- [Building from source](#building-from-source)
- [Project layout](#project-layout)
- [Honest status](#honest-status)
- [License](#license)

---

## Features

- **Single settings category.** All configuration lives under one Mod Settings
  category named *Qivify*, exactly as required.
- **Gray-box repair.** Detects materials that lost their shader or texture and
  replaces them with a safe stand-in so they never render as gray boxes.
- **Camera-box sweep.** A throttled, main-thread-safe sweep that disables
  invalid renderers parented to the main camera — the priority fix for boxes
  following the headset.
- **Shader interception.** Hooks `Shader.Find` and `Material.set_shader` to
  detect unsupported shaders (HDRP, URP, Shader Graph, Amplify, Specular setup,
  etc.) and substitute a supported equivalent.
- **Texture interception.** Hooks `Material.SetTexture` to stop null texture
  assignments producing missing-texture boxes.
- **Bundle tracking.** Hooks `AssetBundle.LoadFromFile` to keep a cache of
  loaded bundles, feed the compatibility report, and de-duplicate conversions.
- **Built-in AudioLink.** An optional AudioLink bundle loader that can be toggled
  from the Qivify menu.
- **Reconvert All AssetBundle Maps.** A one-tap action that resets the
  conversion cache so every tracked bundle is reprocessed.

## The 42 settings

Qivify exposes exactly the 42 required settings. Display names are preserved
**verbatim**, including the intentional spellings *Mutiplayer* and *Blits*. The
`Default` column is the value a fresh install uses.

| # | Setting | Default |
|---|---------|:-------:|
| 1 | Make Qivify Logs | off |
| 2 | Disable All Blits | off |
| 3 | Disable Custom Map Notes | off |
| 4 | Disable Custom Map Sabers | off |
| 5 | Disable Qivify Visuals inside of Mutiplayer | off |
| 6 | Remake GeoShaders Into Normal Mesh | on |
| 7 | Disable One Texture Blits | off |
| 8 | Enable Stand-In Shading | on |
| 9 | Automatic Shader Detection | on |
| 10 | Smart Shader Replacement | on |
| 11 | Preserve Emission and Transparency | on |
| 12 | Convert Unsupported Materials | on |
| 13 | Convert Unsupported Particle Systems | on |
| 14 | Disable Post-Processing Effects | off |
| 15 | Disable Custom Map Lighting | off |
| 16 | Convert PCVR Bundles | on |
| 17 | Convert Only Bundles With Unsupported Shaders | on |
| 18 | Repair Missing Shader References | on |
| 19 | Repair Missing Textures | on |
| 20 | Fix Asset Dependency References | on |
| 21 | Preserve Original Asset Bundles | on |
| 22 | Convert Unsupported Components | on |
| 23 | Asset Bundle Compatibility Report | off |
| 24 | Disable Gray Box Fallbacks | on |
| 25 | Prevent Camera-Following Objects | on |
| 26 | Validate Object Transforms | on |
| 27 | Block Invalid Shader Replacements | on |
| 28 | Prevent Missing-Texture Boxes | on |
| 29 | Preserve Original Mesh Geometry | on |
| 30 | Validate Before Rendering | on |
| 31 | Hide Invalid Debug Geometry | on |
| 32 | Preserve World-Space Rendering | on |
| 33 | Detect and Log Gray-Box Symptoms | on |
| 34 | **Disable Extra Camera Boxes Following Screen** | **on** |
| 35 | Adaptive Effects Quality | on |
| 36 | Prevent Duplicate Conversions | on |
| 37 | Asset Conversion Cache | on |
| 38 | Runtime Memory Protection | on |
| 39 | Main-Thread Safe Asset Conversion | on |
| 40 | Quest GPU Compatibility Check | on |
| 41 | Safe Fallback Mode | on |
| 42 | Conversion Failure Logging | on |

Setting **#34, Disable Extra Camera Boxes Following Screen**, is the priority
fix and defaults to **on**.

## Extra features

- **Reconvert All AssetBundle Maps** — a button in the Qivify menu that clears
  the per-bundle conversion cache and resets the conversion counter, forcing a
  full re-scan on the next load.
- **Enable AudioLink** — an extra toggle (beyond the required 42) that loads a
  built-in AudioLink bundle from
  `files/Qivify/audiolink.bundle` and instantiates its prefab.

## Installation

See [docs/INSTALL.md](docs/INSTALL.md) for the full walk-through. In short:

1. Install [BeatSaberModding/QuestPatcher](https://github.com/Lauriethefish/QuestPatcher)
   and the Scotland2 core mods on your Quest.
2. Open `Qivify.qmod` with QuestPatcher.
3. Launch Beat Saber and open **Mod Settings → Qivify**.

## Building from source

See [docs/BUILDING.md](docs/BUILDING.md). In short:

```bash
qpm restore          # fetch dependencies into extern/
bash scripts/build.sh    # cross-compile libqivify.so (arm64-v8a)
bash scripts/createqmod.sh   # produce Qivify.qmod
```

## Project layout

```
Qivify/
├── CMakeLists.txt          # CMake build definition
├── qpm.json                # QPM manifest (deps, scripts, metadata)
├── qpm_defines.cmake       # QPM-generated compile definitions
├── extern.cmake            # QPM-generated include/lib wiring
├── mod.template.json       # .qmod manifest template
├── ndkpath.txt             # path to the Android NDK
├── cover.png               # QMOD cover image
├── include/                # public headers
│   ├── main.hpp
│   └── qivify/             # settings, compat, ui, audiolink, logger
├── src/                    # implementation
│   ├── main.cpp            # scotland2 entry points
│   ├── settings.cpp        # the 42 settings + persistence
│   ├── compat.cpp          # hooks + compatibility engine
│   ├── ui.cpp              # BSML settings menu
│   └── audiolink.cpp       # AudioLink bundle loader
├── scripts/                # build / package helpers
└── docs/                   # documentation
```

## Honest status

Qivify compiles cleanly to an arm64-v8a shared library and packages into a valid
`.qmod`. It has **not** been run on physical Quest hardware by the author of this
repository. See [docs/LIMITATIONS.md](docs/LIMITATIONS.md) and
[docs/FEATURES.md](docs/FEATURES.md) for a precise breakdown of what is fully
functional, what is partial, and what is unverified.

## License

MIT — see [LICENSE](LICENSE).
