#!/usr/bin/env bash

cd "$(dirname "$0")" && {
  uv run python \
    -m generator \
    --plugin cpp \
    --output-dir .
}
