#pragma once

#include <cstdint>
#include <expected>
#include <string_view>
#include <vector>

#include "Arena.h"

using asn1p_t = struct asn1p_s;

namespace vanadium::asn::ast {

using pos_t = std::uint32_t;

struct Asn1cSyntaxError {
  pos_t pos;
  std::string message;
};

class Asn1cAstWrapper {
 public:
  Asn1cAstWrapper(asn1p_t* ast, lib::Arena& arena);
  ~Asn1cAstWrapper();

  Asn1cAstWrapper(const Asn1cAstWrapper&) = delete;
  Asn1cAstWrapper(Asn1cAstWrapper&&) noexcept;

  Asn1cAstWrapper& operator=(const Asn1cAstWrapper&) = delete;
  Asn1cAstWrapper& operator=(Asn1cAstWrapper&&) noexcept;

  const asn1p_t* operator->() const {
    return ast_;
  }
  const asn1p_t& operator*() const {
    return *ast_;
  }

  static std::expected<Asn1cAstWrapper, std::vector<Asn1cSyntaxError>> Parse(lib::Arena&, std::string_view);

 private:
  asn1p_t* ast_;
  lib::Arena* arena_;
};

}  // namespace vanadium::asn::ast
