#!/usr/bin/env bash

set -euo pipefail

PROJECT_DIR="$1"
EXECUTABLE="ttcnexec"

echo '==> Building compiler & runtime'
inv build --sanitizers --target 'vanadiumc vanadium_rt vanadium_hostc'

echo '==> Compiling TTCN to LLVM IR'
(set -x;
./out/build/bin/compiler/vanadiumc -g "${PROJECT_DIR}"
)

echo '==> Compiling LLVM IR to object'
for SRC_IR in "${PROJECT_DIR}"/*.ll; do
  OUT=$(basename "${SRC_IR}".ll)
  (set -x;
  llc-19 -filetype=obj -relocation-model=pic "${SRC_IR}" -o "${PROJECT_DIR}/${OUT}.o"
  )
done

echo '==> Linking'
(set -x;
clang -lstdc++ -fsanitize=address "${PROJECT_DIR}"/*.o \
      "out/build/src/compiler/hostc/libvanadium_hostc.a" \
      "out/build/src/compiler/runtime/libvanadium_rt.a" \
      -o "${PROJECT_DIR}/${EXECUTABLE}"
)

echo "==> Built: ${PROJECT_DIR}/${EXECUTABLE}"
