# Qivify limitations and verification record

This document is deliberately blunt. It separates what was **verified** from what
was **not**, and lists the known limitations of Qivify 1.0.0.

## Verification levels

| Level | Meaning | Qivify 1.0.0 |
|-------|---------|:------------:|
| Source inspection | The code was read and reasoned about. | ✅ Done |
| Compilation | The code was cross-compiled to arm64-v8a without errors. | ✅ Done |
| Static verification | The produced ELF was inspected (`file`, `llvm-nm`, `llvm-readelf`). | ✅ Done |
| Automated unit tests | Tests exercising the logic on a host. | ❌ Not done |
| Emulator testing | Running under a Quest emulator. | ❌ Not done |
| **Physical Quest testing** | Running on real Quest hardware in Beat Saber. | ❌ **Not done** |

**Qivify has never been executed on a Meta Quest.** No claim in this repository
should be read as "tested in game". The mod compiles and packages correctly, and
the hooks are written against real, verified il2cpp method signatures, but their
runtime behaviour on device is unverified.

## What was verified

- `build/libqivify.so` is a valid `ELF 64-bit LSB shared object, ARM aarch64,
  ... stripped`.
- It exports the two Scotland2 entry points: `setup` and `late_load`
  (confirmed with `llvm-nm -D`).
- Its `NEEDED` libraries are exactly: `libbeatsaber-hook.so`, `libbsml.so`,
  `libcustom-types.so`, `libpaper2_scotland2.so`, `libsl2.so`, `liblog.so`,
  `libm.so`, `libdl.so`, `libc.so`.
- Its `SONAME` is `libqivify.so`.
- The QMOD packages into a valid zip with a `mod.json` conforming to the
  QuestPatcher QMOD schema `_QPVersion` 1.1.0.

## Known limitations

1. **No on-device testing.** This is the single most important limitation.
2. **No real asset-bundle conversion.** Qivify is a *runtime interception layer*.
   It detects and substitutes at the API level; it does not rewrite `.bundle`
   files on disk. Consequently the Tier-C settings in
   [FEATURES.md](FEATURES.md) are stored but not enforced.
3. **Heuristic shader detection.** Unsupported shaders are detected by name
   fragments (HDRP, URP, Shader Graph, Amplify, Specular setup, etc.). A shader
   with an unusual name that is genuinely unsupported will be missed, and a
   shader whose name merely contains a token may be replaced unnecessarily.
4. **Fallback shader availability.** Qivify tries `Unlit/Texture`,
   `Sprites/Default` and `UI/Default` in that order. If none of these exist in
   the running game build, no substitution occurs.
5. **Camera sweep heuristics.** The sweep only inspects *direct children* of the
   main camera transform, and only disables objects that have a `Renderer` with a
   null material or null shader (or, with validation on, a NaN/inf position).
   Boxes parented elsewhere, or with a non-null-but-wrong material, are not
   caught.
6. **Blit settings are aggressive.** `Disable All Blits` skips *every*
   single-texture `Graphics.Blit`. Enabling it may break legitimate rendering
   (including UI). It defaults to off for that reason.
7. **AudioLink requires user-supplied assets.** The `Enable AudioLink` toggle
   loads a bundle from `files/Qivify/audiolink.bundle`. Qivify does not ship that
   bundle; without it the toggle is a no-op that logs a warning.
8. **Multiplayer visuals setting is not enforced.** There is no multiplayer-state
   detection in this build.
9. **Cover art text.** The supplied cover image reads "Vivify" rather than
   "Qivify". It is used verbatim as requested.

## Threading and safety notes

- The camera sweep runs inside the `Camera.get_main` hook, which is a main-thread
  call. It is throttled to roughly once every 90 invocations and wrapped in a
  `try/catch` so it can never break rendering.
- Shared state (`g_bundleCache`, `g_failedShaders`, counters) is guarded by a
  `std::mutex`.
- The fallback shader is resolved once and cached.

## Reporting problems

If you test Qivify on hardware and something breaks, please capture the Beat
Saber log (it contains lines prefixed with `Qivify`) and open an issue. Concrete
on-device results are the most valuable contribution this project can receive.
