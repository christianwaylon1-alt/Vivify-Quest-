#!/usr/bin/env bash
#
# Packages build/libqivify.so into a distributable Qivify.qmod.
# A .qmod is simply a zip containing mod.json, the mod library and a cover image.
#
set -euo pipefail

cd "$(dirname "$0")/.."

BUILD_DIR="build"
SO="$BUILD_DIR/libqivify.so"
OUT="Qivify.qmod"
STAGE="$BUILD_DIR/qmod"

if [[ ! -f "$SO" ]]; then
    echo ">> libqivify.so not found, building first..."
    bash ./scripts/build.sh
fi

if [[ ! -f "$SO" ]]; then
    echo "error: failed to produce $SO" >&2
    exit 1
fi

echo ">> Staging qmod contents"
rm -rf "$STAGE"
mkdir -p "$STAGE"

# Substitute the QPM template placeholders. We do this manually so the script
# works without invoking the QPM CLI.
sed -e 's/${mod_name}/Qivify/g' \
    -e 's/${mod_id}/qivify/g' \
    -e 's/${version}/1.0.0/g' \
    -e 's/${binary}/libqivify.so/g' \
    mod.template.json > "$STAGE/mod.json"

cp "$SO" "$STAGE/libqivify.so"

if [[ -f cover.png ]]; then
    cp cover.png "$STAGE/cover.png"
fi

echo ">> Creating $OUT"
rm -f "$OUT"
( cd "$STAGE" && zip -r -X -q "../../$OUT" . )

echo ">> Done: $OUT"
unzip -l "$OUT"
