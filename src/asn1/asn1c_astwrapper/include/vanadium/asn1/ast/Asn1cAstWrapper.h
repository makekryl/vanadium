#pragma once

#include <vanadium/lib/Arena.h>

#include <cstdint>
#include <string_view>
#include <vector>

using asn1p_t = struct asn1p_s;

namespace vanadium::asn1::ast {

using pos_t = std::uint32_t;

struct Asn1cSourceRange {
  pos_t begin;
  pos_t end;
};

struct Asn1cSyntaxError {
  Asn1cSourceRange range;
  std::string message;
};

class Asn1cAstWrapper {
 public:
  Asn1cAstWrapper(asn1p_t* ast, std::vector<Asn1cSyntaxError> errors, lib::Arena& arena);
  ~Asn1cAstWrapper();

  Asn1cAstWrapper(const Asn1cAstWrapper&) = delete;
  Asn1cAstWrapper(Asn1cAstWrapper&&) noexcept;

  Asn1cAstWrapper& operator=(const Asn1cAstWrapper&) = delete;
  Asn1cAstWrapper& operator=(Asn1cAstWrapper&&) noexcept;

  operator bool() const noexcept {
    return ast_ != nullptr;
  }

  const asn1p_t* operator->() const {
    return ast_;
  }
  const asn1p_t& operator*() const {
    return *ast_;
  }
  [[nodiscard]] const asn1p_t* Raw() const {
    return ast_;
  }

  [[nodiscard]] const std::vector<Asn1cSyntaxError>& Errors() const {
    return errors_;
  }
  [[nodiscard]] std::vector<Asn1cSyntaxError>& ErrorsMut() {
    return errors_;
  }

 private:
  asn1p_t* ast_;
  std::vector<Asn1cSyntaxError> errors_;
  lib::Arena* arena_;
};

Asn1cAstWrapper Parse(lib::Arena&, std::string_view);

}  // namespace vanadium::asn1::ast
