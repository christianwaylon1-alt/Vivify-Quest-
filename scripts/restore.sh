#!/usr/bin/env bash
#
# Restores all QPM dependencies into extern/ (includes + libs).
# Requires the QPM CLI (https://github.com/QuestPackageManager/QPM.RS).
#
set -euo pipefail

cd "$(dirname "$0")/.."

if ! command -v qpm >/dev/null 2>&1; then
    echo "error: the 'qpm' CLI was not found on PATH." >&2
    echo "       Install QPM-RS and re-run this script." >&2
    exit 1
fi

echo ">> Restoring QPM dependencies"
qpm restore

echo ">> Done. Dependencies are in extern/"
