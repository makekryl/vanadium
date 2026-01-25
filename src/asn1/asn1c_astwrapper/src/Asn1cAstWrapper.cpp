#include "vanadium/asn1/ast/Asn1cAstWrapper.h"

#include <concepts>
#include <utility>
#include <vector>

#include <asn1c/libasn1parser/asn1parser_cxx.h>

#include <vanadium/lib/Arena.h>

namespace vanadium::asn1::ast {

namespace {
void WithArenaAsn1cAllocator(lib::Arena& arena, std::invocable auto f) {
  auto previous_allocator = asn1p_mem_get_allocator();
  asn1p_mem_set_allocator(asn1p_allocator_t{
      .ctx = reinterpret_cast<void*>(&arena),
      .alloc = [](void* ctx, std::size_t size) -> void* {
        return reinterpret_cast<vanadium::lib::Arena*>(ctx)->AllocBuffer(size);
      },
      .free = [](void* ctx, void* p) {},
  });
  f();
  asn1p_mem_set_allocator(previous_allocator);
}
}  // namespace

Asn1cAstWrapper::Asn1cAstWrapper(asn1p_t* ast, std::vector<Asn1cSyntaxError> errors, lib::Arena& arena)
    : ast_(ast), errors_(std::move(errors)), arena_(&arena) {}

Asn1cAstWrapper::Asn1cAstWrapper(Asn1cAstWrapper&& other) noexcept
    : ast_(std::exchange(other.ast_, nullptr)),
      errors_(std::move(other.errors_)),
      arena_(std::exchange(other.arena_, nullptr)) {}

Asn1cAstWrapper& Asn1cAstWrapper::operator=(Asn1cAstWrapper&& other) noexcept {
  Asn1cAstWrapper tmp(std::move(other));
  std::swap(ast_, other.ast_);
  std::swap(errors_, other.errors_);
  std::swap(arena_, other.arena_);
  return *this;
}

Asn1cAstWrapper::~Asn1cAstWrapper() {
  if (!ast_) {
    return;
  }
  WithArenaAsn1cAllocator(*arena_, [&] {
    asn1p_delete(ast_);
  });
}

Asn1cAstWrapper Parse(lib::Arena& arena, std::string_view src) {
  asn1p_t* ast;
  asn1p_errs_t errs;

  WithArenaAsn1cAllocator(arena, [&] {
    asn1p_errs_init(&errs);
    ast = asn1p_parse_buffer(src.data(), src.size(), &errs);
  });

  std::vector<Asn1cSyntaxError> errors(errs.size);
  for (std::size_t i = 0; i < errs.size; i++) {
    errors[i].range = {
        .begin = errs.data[i].range.begin,
        .end = errs.data[i].range.end,
    };
    errors[i].message = std::string(errs.data[i].msg);
  }

  WithArenaAsn1cAllocator(arena, [&] {
    asn1p_errs_free(&errs);
  });

  return Asn1cAstWrapper{ast, std::move(errors), arena};
}

}  // namespace vanadium::asn1::ast
