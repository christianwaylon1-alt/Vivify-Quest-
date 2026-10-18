#!/usr/bin/env bash
#
# Produces Qivify-Source.zip: the complete, GitHub-ready source tree.
# Build outputs, restored dependencies and local caches are excluded.
#
set -euo pipefail

cd "$(dirname "$0")/.."

OUT="Qivify-Source.zip"

echo ">> Packaging source into $OUT"
rm -f "$OUT"

zip -r -X -q "$OUT" . \
    -x 'build/*' \
    -x 'extern/*' \
    -x '.git/*' \
    -x '.git' \
    -x '*/.git/*' \
    -x 'Qivify.qmod' \
    -x 'Qivify-Source.zip' \
    -x '*.o' \
    -x '*.so' \
    -x '*.a' \
    -x 'todo.md' \
    -x 'compile_commands.json' \
    -x '.cache/*'

echo ">> Done: $OUT"
unzip -l "$OUT" | tail -n +2 | head -n 60
