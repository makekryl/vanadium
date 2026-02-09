#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 2 ]]; then
  echo "Usage: $0 BINARIES_DIR SYMBOLS_DIR" >&2
  exit 1
fi

readonly BINARIES_DIR="$1"
readonly SYMBOLS_DIR="$2"

is_elf_file() {
  local -r file_path="$1"
  readelf -h "$file_path" &>/dev/null
}

while IFS= read -r file_path; do
  if ! is_elf_file "${file_path}"; then
    continue
  fi

  relative_path="${file_path#${BINARIES_DIR}/}"
  symbol_path="${SYMBOLS_DIR}/${relative_path}.sym"

  mkdir -p "$(dirname "$symbol_path")"

  echo "Stripping ${relative_path}"

  objcopy --only-keep-debug "$file_path" "$symbol_path"
  strip --strip-unneeded "$file_path"
  objcopy --add-gnu-debuglink="$symbol_path" "$file_path"

done < <(find "$BINARIES_DIR" -type f)
