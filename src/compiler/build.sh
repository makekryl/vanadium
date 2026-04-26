#!/usr/bin/env bash

set -euo pipefail

SRC_TTCN="$1"
SRC_IR="${SRC_TTCN}.ll"
BUILD_DIR="./out/irbuild"
OUT="ttcnexec"

mkdir -p "${BUILD_DIR}"

echo '==> Building compiler & runtime'
inv build --sanitizers --target 'vanadiumc vanadium_rt vanadium_hostc'

echo '==> Compiling TTCN to LLVM IR'
./out/build/bin/compiler/vanadiumc -g "${SRC_TTCN}"

echo '==> Compiling LLVM IR to object'
llc-19 -filetype=obj -relocation-model=pic "${SRC_IR}" -o "${BUILD_DIR}/${OUT}.o"

echo '==> Linking'
clang -lstdc++ -fsanitize=address "${BUILD_DIR}/${OUT}.o" \
      "out/build/src/compiler/hostc/libvanadium_hostc.a" \
      "out/build/src/compiler/runtime/libvanadium_rt.a" \
      -o "${BUILD_DIR}/${OUT}"

echo "==> Built: ${BUILD_DIR}/${OUT}"
