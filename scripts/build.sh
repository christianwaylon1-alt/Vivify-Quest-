#!/usr/bin/env bash
#
# Qivify build script.
# Cross-compiles libqivify.so for Meta Quest (Android arm64-v8a) using the
# Android NDK, CMake and Ninja. The resulting library is written to build/.
#
set -euo pipefail

cd "$(dirname "$0")/.."

if [[ ! -f ndkpath.txt ]]; then
    echo "error: ndkpath.txt is missing (it must contain the path to the Android NDK)." >&2
    exit 1
fi

NDK="$(cat ndkpath.txt)"
BUILD_DIR="build"

if [[ ! -d "$NDK" ]]; then
    echo "error: Android NDK not found at '$NDK'." >&2
    echo "       Update ndkpath.txt to point at a valid NDK (r26 recommended)." >&2
    exit 1
fi

echo ">> Configuring Qivify (NDK: $NDK)"
cmake -G Ninja \
    -B "$BUILD_DIR" \
    -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_ANDROID_NDK="$NDK" \
    -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=24 \
    -DANDROID_STL=c++_static

echo ">> Building libqivify.so"
cmake --build "$BUILD_DIR"

echo ">> Done: $BUILD_DIR/libqivify.so"
