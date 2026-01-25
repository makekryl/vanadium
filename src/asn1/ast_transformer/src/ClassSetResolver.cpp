#include "vanadium/asn1/ast/ClassSetResolver.h"

#include <format>
#include <string_view>

#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <magic_enum/magic_enum.hpp>

#include "vanadium/asn1/ast/ClassObjectParser.h"

namespace vanadium::asn1::ast {

namespace {

// to make the code below a little bit more readable
constexpr bool kShouldContinue = true;
constexpr bool kShouldNOTContinue = false;

bool ResolveClassValue(const asn1p_value_t* value, const asn1p_expr_t* cls_expr,
                       const ClassSetElementConsumer& consumer) {
  switch (value->type) {
    case asn1p_value_s::ATV_UNPARSED: {
      const bool should_continue = consumer.accept_class([&](const ClassObjectConsumer& co_consumer) {
        ParseClassObject(std::string_view{(char*)value->value.string.buf, (size_t)value->value.string.size},
                         cls_expr->with_syntax, co_consumer);
      });
      if (!should_continue) {
        return kShouldNOTContinue;
      }
      break;
    }

    case asn1p_value_s::ATV_REFERENCED: {
      const asn1p_expr_t* referenced_expr = consumer.resolve(value->value.reference);
      if (!referenced_expr) {
        return kShouldContinue;
      }

      return ResolveClassValue(referenced_expr->value, cls_expr, consumer);
    }

    default: {
      // TODO(ranges)
      consumer.emit_error({},
                          std::format("unexpected constraint value type: '{}'", magic_enum::enum_name(value->type)));
      break;
    }
  }

  return kShouldContinue;
}

bool ResolveClassSetConstraint(const asn1p_constraint_t* constr, const asn1p_expr_t* cls_expr,
                               const ClassSetElementConsumer& consumer) {
  // TODO: pass proper ranges to emit_error calls below

  switch (constr->type) {
    case ACT_CA_CSV:
    case ACT_CA_UNI: {
      for (unsigned int i = 0; i < constr->el_count; i++) {
        if (ResolveClassSetConstraint(constr->elements[i], cls_expr, consumer) == kShouldNOTContinue) {
          return kShouldNOTContinue;
        }
      }
      return kShouldContinue;
    }

    case ACT_EL_EXT: {
      // Paging-eDRXInformation-ExtIEs S1AP-PROTOCOL-EXTENSION ::= {
      //    ...
      // }
      return kShouldContinue;
    }

    case ACT_EL_VALUE: {  // inline class value or reference to it
      assert(constr->value);
      return ResolveClassValue(constr->value, cls_expr, consumer);
    }

    case ACT_EL_TYPE: {  // reference to another set
      assert(constr->containedSubtype);
      const asn1p_expr_t* referenced_expr = consumer.resolve(constr->containedSubtype->value.v_type->reference);
      if (!referenced_expr) {
        return kShouldContinue;
      }
      return ResolveClassSetConstraint(referenced_expr->constraints, cls_expr, consumer);
    }

    default: {
      consumer.emit_error(constr->_src_range,
                          std::format("unexpected constraint type: '{}'", magic_enum::enum_name(constr->type)));
      return kShouldContinue;
    }
  }
}

}  // namespace

void ResolveClassSet(const asn1p_expr_t* s_expr, const asn1p_expr_t* cls_expr,
                     const ClassSetElementConsumer& consumer) {
  ResolveClassSetConstraint(s_expr->constraints, cls_expr, consumer);
}

}  // namespace vanadium::asn1::ast
