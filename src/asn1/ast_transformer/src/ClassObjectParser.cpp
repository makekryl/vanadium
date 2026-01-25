#include "vanadium/asn1/ast/ClassObjectParser.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <format>
#include <string_view>

#include <asn1c/libasn1parser/asn1parser_cxx.h>

// TODO: for some reason, clangd ignores .clang-format config for this file

namespace vanadium::asn1::ast {

namespace {

// Based on asn1c/libasn1fix/asn1fix_cws.c:asn1f_next_literal_chunk
const asn1p_wsyntx_chunk_t *NextLiteralChunk(const asn1p_wsyntx_t *syntax, const asn1p_wsyntx_chunk_t *chunk,
                                             const char *buf) {
  const asn1p_wsyntx_chunk_t *next_chunk = TQ_NEXT(chunk, next);
  do {
    if (!next_chunk) {
      if (!syntax->parent) {
        break;
      }
      next_chunk = TQ_NEXT(syntax->parent, next);
      continue;
    }

    if (next_chunk->type == asn1p_wsyntx_chunk_s::WC_LITERAL) {
      if (std::strstr(buf, next_chunk->content.token)) {
        break;
      }
      if (!syntax->parent) {
        break;
      }
      next_chunk = TQ_NEXT(syntax->parent, next);
      continue;
    }

    if (next_chunk->type == asn1p_wsyntx_chunk_s::WC_WHITESPACE) {
      next_chunk = TQ_NEXT(next_chunk, next);
      continue;
    }

    if (next_chunk->type == asn1p_wsyntx_chunk_s::WC_OPTIONALGROUP) {
      syntax = next_chunk->content.syntax;
      next_chunk = TQ_FIRST(&(next_chunk->content.syntax->chunks));
      continue;
    }

    break;
  } while (next_chunk);
  return next_chunk;
}

// Based on asn1c/libasn1fix/asn1fix_cws.c:_asn1f_parse_class_object_data
bool ParseClassObject(const ClassObjectConsumer &consumer, const char *buf, const char *bend,
                      const asn1p_wsyntx_t *syntax, const char **newpos, bool is_optional) {
  const auto skip_spaces = [&] {
    while (buf < bend && std::isspace(*buf)) {
      ++buf;
    }
  };

  // TODO: provide error ranges

  struct asn1p_wsyntx_chunk_s *chunk;
  TQ_FOR(chunk, &(syntax->chunks), next) {
    switch (chunk->type) {
      case asn1p_wsyntx_chunk_s::WC_WHITESPACE:
        // Ignore whitespace
        break;

      case asn1p_wsyntx_chunk_s::WC_LITERAL: {
        skip_spaces();
        const int token_len = std::strlen(chunk->content.token);
        if ((bend - buf < token_len) || (std::memcmp(buf, chunk->content.token, token_len) != 0)) {
          if (!is_optional) {
            consumer.emit_error({}, std::format("expected pattern like '{}', got '{}'", chunk->content.token, buf));
          }
          *newpos = buf;
          return false;
        }
        buf += token_len;
        break;
      }

      case asn1p_wsyntx_chunk_s::WC_FIELD: {
        const char *buf_old = buf;
        const char *value_end = nullptr;

        skip_spaces();

        const asn1p_wsyntx_chunk_t *next_literal = NextLiteralChunk(syntax, chunk, buf);
        if (!next_literal) {
          value_end = bend;
        } else {
          value_end = std::strstr(buf, next_literal->content.token);
          if (!value_end) {
            if (!is_optional) {
              consumer.emit_error({}, std::format("literal '{}' not found", chunk->content.token));
            }

            *newpos = buf_old;
            return false;
          }
        }
        while ((value_end > buf) && std::isspace(value_end[-1])) {
          --value_end;
        }
        const bool should_continue = consumer.accept_row({
            .name = chunk->content.token,
            .value = std::string_view{buf, value_end},
        });
        if (!should_continue) {
          *newpos = bend;
          return false;
        }
        buf = value_end;
        *newpos = buf;
        break;
      }

      case asn1p_wsyntx_chunk_s::WC_OPTIONALGROUP: {
        const char *np = nullptr;  // todo: pass newpos itself
        skip_spaces();
        const bool ret = ParseClassObject(consumer, buf, bend, chunk->content.syntax, &np, true);
        *newpos = np;
        if (!ret && np != buf) {
          return ret;
        }
        buf = np;
        break;
      }
    }
  }

  *newpos = buf;
  return true;
}

}  // namespace

void ParseClassObject(std::string_view buf, const asn1p_wsyntx_t *syntax, const ClassObjectConsumer &consumer) {
  const char *newpos{nullptr};  // ignored
  ParseClassObject(consumer,
                   buf.begin() + 1,  // +1 to skip {
                   buf.end() - 1,    // -1 to drop {
                   syntax, &newpos, false);
}

}  // namespace vanadium::asn1::ast
