#pragma once

#include <optional>
#include <string_view>

#include <asn1c/libasn1parser/asn1parser_cxx.h>

namespace vanadium::asn1::ast {
// Parses ASN identifiers parsing of which was postponed to the class object parsing stage
// It would be wonderful to utilize the existing asn1p infrastructure, but it will be too
// inconvenient and costly due to the architecture, so we're here
std::optional<asn1p_expr_type> ParseAsnTypeIdentifier(std::string_view);
}  // namespace vanadium::asn1::ast
