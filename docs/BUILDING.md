# Building Qivify

Qivify is a C++20 CMake project cross-compiled for **Android arm64-v8a** with the
**Android NDK**. Dependencies are managed by **QPM** (Quest Package Manager).

## Toolchain requirements

| Tool | Version used | Notes |
|------|--------------|-------|
| Android NDK | r26 (26.x) | `aarch64-linux-android24-clang`, clang 17 |
| CMake | ≥ 3.22 | 4.x works |
| Ninja | any recent | generator |
| QPM (QPM-RS) | ≥ 0.4 | `qpm` on `PATH` |
| Python | 3.x | used by some QPM steps |

The project targets `ANDROID_PLATFORM 24` (Android 7.0) and
`ANDROID_STL c++_static`.

## 1. Point the build at your NDK

Edit `ndkpath.txt` so it contains the absolute path to your NDK, e.g.:

```
/opt/android-ndk-r26d
```

## 2. Restore dependencies

```bash
qpm restore
```

This resolves every entry in `qpm.json` into `extern/includes` (headers) and
`extern/libs` (prebuilt `.so`). It also regenerates `extern.cmake`,
`qpm_defines.cmake` and `qpm.shared.json`. These generated files are committed
so the tree builds even before the first restore, but they will be overwritten.

> **Note:** QPM does **not** add the BSML include directory automatically. The
> project's `CMakeLists.txt` adds `${EXTERN_DIR}/includes/bsml/shared` manually.

## 3. Build the shared library

```bash
bash scripts/build.sh
```

This runs CMake + Ninja and writes `build/libqivify.so`. Under the hood it is
equivalent to:

```bash
cmake -G Ninja -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_ANDROID_NDK="$(cat ndkpath.txt)" \
    -DCMAKE_TOOLCHAIN_FILE="$(cat ndkpath.txt)/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=24 \
    -DANDROID_STL=c++_static
cmake --build build
```

## 4. Package the QMOD

```bash
bash scripts/createqmod.sh
```

This stages `mod.json` (from `mod.template.json`), `libqivify.so` and
`cover.png` into a zip named `Qivify.qmod`. You can also run the whole pipeline
in one shot through QPM:

```bash
qpm qmod
```

## 5. Package the source archive

```bash
bash scripts/package-source.sh
```

Produces `Qivify-Source.zip` excluding `build/`, `extern/`, `.git/` and
binaries.

## Build notes / gotchas

- **Inline hooks.** Qivify uses beatsaber-hook's `MAKE_HOOK_MATCH`. The
  inline-hook support sources under
  `extern/includes/beatsaber-hook/shared/inline-hook/` are compiled directly
  into the mod, and the mod links `liblog` for `__android_log_print`.
- **`UnityW<T>`.** cordl returns `UnityW<T>` wrappers, which convert implicitly
  to `T*` but are **not** raw pointers. Use `auto`, never `auto*`.
- **Include paths.** paper lives at `paper2_scotland2/shared/logger.hpp`; BSML
  lives at `bsml/shared/BSML.hpp` and `bsml/shared/BSML-Lite.hpp`.
- **Stripping.** The library is stripped after the build; the exported entry
  points `setup` and `late_load` are preserved via `MOD_EXTERN_FUNC`
  (`visibility("default")`).

## Verifying a build

```bash
file build/libqivify.so
# ELF 64-bit LSB shared object, ARM aarch64, ... stripped

llvm-nm -D build/libqivify.so | grep -E 'setup|late_load'
# T late_load
# T setup
```

The `NEEDED` entries should include `libbeatsaber-hook.so`, `libbsml.so`,
`libcustom-types.so`, `libpaper2_scotland2.so`, `libsl2.so` and `liblog.so`.
