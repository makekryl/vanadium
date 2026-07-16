#include "vanadium/asn1/ast/Asn1TypeIdentifierParser.h"

#include <cctype>

#include <asn1c/libasn1parser/asn1p_expr.h>

#include <vanadium/lib/StaticMap.h>

namespace vanadium::asn1::ast {

namespace {
constexpr auto kAsnTypeIdentifiers = lib::MakeStaticMap<std::string_view, asn1p_expr_type>({
    {"NULL", ASN_BASIC_NULL},
    {"BOOLEAN", ASN_BASIC_BOOLEAN},
    {"INTEGER", ASN_BASIC_INTEGER},
    {"REAL", ASN_BASIC_REAL},

    {"BIT STRING", ASN_BASIC_BIT_STRING},
    {"OCTET STRING", ASN_BASIC_OCTET_STRING},
    {"CHARACTER STRING", ASN_BASIC_CHARACTER_STRING},

    {"OBJECT IDENTIFIER", ASN_BASIC_OBJECT_IDENTIFIER},
    {"RELATIVE-OID", ASN_BASIC_RELATIVE_OID},

    {"EXTERNAL", ASN_BASIC_EXTERNAL},
    {"EMBEDDED PDV", ASN_BASIC_EMBEDDED_PDV},

    {"IA5String", ASN_STRING_IA5String},
    {"PrintableString", ASN_STRING_PrintableString},
    {"VisibleString", ASN_STRING_VisibleString},
    {"ISO646String", ASN_STRING_ISO646String},
    {"NumericString", ASN_STRING_NumericString},
    {"UniversalString", ASN_STRING_UniversalString},
    {"BMPString", ASN_STRING_BMPString},
    {"UTF8String", ASN_STRING_UTF8String},
    {"GeneralString", ASN_STRING_GeneralString},
    {"GraphicString", ASN_STRING_GraphicString},
    {"TeletexString", ASN_STRING_TeletexString},
    {"T61String", ASN_STRING_T61String},
    {"VideotexString", ASN_STRING_VideotexString},
    {"ObjectDescriptor", ASN_STRING_ObjectDescriptor},
});
}

std::optional<asn1p_expr_type> ParseAsnTypeIdentifier(std::string_view s) {
  if (const auto* t = kAsnTypeIdentifiers.get(s); t) {
    return *t;
  }

  const auto consume = [&](std::string_view word) {
    const auto eat_ws = [&] {
      while (!s.empty() && std::isspace(s.front())) {
        s.remove_prefix(1);
      }
    };

    eat_ws();
    if (!s.starts_with(word)) {
      return false;
    }

    s.remove_prefix(word.size());
    if (s.empty()) {
      return true;
    }
    if (s.front() == ' ') {
      return true;
    }
    return s.empty() || std::isspace(s.front());
  };
  const auto done = [&] -> bool {
    return s.empty();
  };

  if (consume("BIT") && consume("STRING") && done()) {
    return ASN_BASIC_BIT_STRING;
  }
  if (consume("OCTET") && consume("STRING") && done()) {
    return ASN_BASIC_OCTET_STRING;
  }
  if (consume("CHARACTER") && consume("STRING") && done()) {
    return ASN_BASIC_CHARACTER_STRING;
  }
  if (consume("OBJECT") && consume("IDENTIFIER") && done()) {
    return ASN_BASIC_OBJECT_IDENTIFIER;
  }
  if (consume("EMBEDDED") && consume("PDV") && done()) {
    return ASN_BASIC_EMBEDDED_PDV;
  }

  return std::nullopt;
}

}  // namespace vanadium::asn1::ast
