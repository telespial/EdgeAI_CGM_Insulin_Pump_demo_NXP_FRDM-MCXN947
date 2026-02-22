#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

state_file="docs/PROJECT_STATE.md"
flash_cmd="${FLASH_CMD:-}"
bin_path="${BIN_PATH:-}"

if [[ -z "$flash_cmd" && -f "$state_file" ]]; then
  flash_cmd="$(sed -n 's/^- Flash Command:[[:space:]]*//p' "$state_file" | head -n1)"
fi

if [[ -z "$bin_path" && -f "$state_file" ]]; then
  bin_path="$(sed -n 's/^- Binary:[[:space:]]*//p' "$state_file" | head -n1)"
fi

if [[ -z "$flash_cmd" ]]; then
  echo "Flash Command is empty. Set FLASH_CMD or docs/PROJECT_STATE.md" >&2
  exit 1
fi

if [[ -z "$bin_path" ]]; then
  echo "Binary is empty. Set BIN_PATH or docs/PROJECT_STATE.md" >&2
  exit 1
fi

resolved_cmd="${flash_cmd//\{BIN_PATH\}/$bin_path}"
echo "running: $resolved_cmd"
bash -lc "$resolved_cmd"
