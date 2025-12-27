#!/bin/bash
set -e

pushd "$(dirname $0)/src" > /dev/null

bison \
  --defines="../include/asn1c/libasn1parser/asn1p_y.h" \
  --output='asn1p_y.c' \
  'asn1p_y.y'

flex \
  --header-file="../include/asn1c/libasn1parser/asn1p_l.h" \
  --outfile='asn1p_l.c' \
  'asn1p_l.l'

popd > /dev/null
