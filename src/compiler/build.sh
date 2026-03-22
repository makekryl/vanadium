#!/usr/bin/env bash

set -euo pipefail

SRC_TTCN="$1"
SRC_IR="${SRC_TTCN}.ll"
BUILD_DIR="./out/irbuild"
OUT="ttcnexec"

mkdir -p "${BUILD_DIR}"

echo "==> Compiling TTCN to LLVM IR"
inv build --sanitizers --target 'vanadiumc vanadium_rt' \
  && ./out/build/bin/compiler/vanadiumc "${SRC_TTCN}"

echo "==> Compiling LLVM IR to object"
llc-19 -filetype=obj -relocation-model=pic "${SRC_IR}" -o "${BUILD_DIR}/${OUT}.o"

echo "==> Linking"
clang -lstdc++ -fsanitize=address "${BUILD_DIR}/${OUT}.o" \
      "out/build/src/compiler/runtime/libvanadium_rt.a" \
      -o "${BUILD_DIR}/${OUT}"

echo "==> Built: ${BUILD_DIR}/${OUT}"
