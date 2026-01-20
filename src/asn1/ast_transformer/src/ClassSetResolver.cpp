#include "vanadium/asn1/ast/ClassSetResolver.h"

#include <asn1c/libasn1parser/asn1parser_cxx.h>

#include <format>
#include <magic_enum/magic_enum.hpp>
#include <string_view>

#include "asn1c/libasn1parser/asn1p_constr.h"
#include "vanadium/asn1/ast/ClassObjectParser.h"

namespace vanadium::asn1::ast {

namespace {

// well actually it does not need an entire consumer, it needs only resolve & emit_error
const asn1p_expr_t* ResolveRef(const asn1p_ref_t* ref, const ClassSetElementConsumer& consumer) {
  assert(ref->comp_count > 0);
  if (ref->comp_count != 1) {
    consumer.emit_error(ref->components[0]._name_range, "(ClassSetResolver) ref->comp_count != 1");
    return nullptr;
  }

  const char* referenced_expr_name = ref->components[0].name;
  const asn1p_expr_t* referenced_expr = consumer.resolve(ref->module, referenced_expr_name);
  if (!referenced_expr) {
    consumer.emit_error(ref->components[0]._name_range,
                        std::format("unresolved reference to '{}'", referenced_expr_name));
    return nullptr;
  }

  return referenced_expr;
}

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
      const asn1p_expr_t* referenced_expr = ResolveRef(value->value.reference, consumer);
      if (!referenced_expr) {
        return kShouldContinue;
      }

      return ResolveClassValue(referenced_expr->value, cls_expr, consumer);
    }

    default: {
      // TODO
      consumer.emit_error(cls_expr->_Identifier_Range,
                          std::format("unexpected constraint value type: '{}'", magic_enum::enum_name(value->type)));
      break;
    }
  }

  return kShouldContinue;
}

bool ResolveClassSetImpl(const asn1p_expr_t* s_expr, const asn1p_expr_t* cls_expr,
                         const ClassSetElementConsumer& consumer) {
  // TODO: pass proper ranges to emit_error calls below

  const asn1p_constraint_t* constraints = s_expr->constraints;

  if (constraints->type != ACT_CA_UNI) {
    // well, there's a possible valid case:
    // { A | B | C
    //  , ... }
    if ((constraints->el_count == 2) && (constraints->elements[0]->type == ACT_CA_UNI) &&
        (constraints->elements[1]->type == ACT_EL_EXT)) {
      constraints = constraints->elements[0];
    } else {
      consumer.emit_error(s_expr->_Identifier_Range,
                          std::format("referenced set '{}' is not union-constrained: '{}'", s_expr->Identifier,
                                      magic_enum::enum_name(constraints->type)));
      return kShouldContinue;
    }
  }

  for (int i = 0; i < constraints->el_count; i++) {
    const asn1p_constraint_t* constr = constraints->elements[i];
    switch (constr->type) {
      case ACT_EL_VALUE: {  // inline class value or reference to it
        assert(constr->value);
        const bool should_continue = ResolveClassValue(constr->value, cls_expr, consumer);
        if (!should_continue) {
          return kShouldNOTContinue;
        }
        break;
      }
      case ACT_EL_TYPE: {  // reference to another set
        assert(constr->containedSubtype);
        const asn1p_expr_t* referenced_expr = ResolveRef(constr->containedSubtype->value.v_type->reference, consumer);
        if (!referenced_expr) {
          return kShouldContinue;
        }

        const bool should_continue = ResolveClassSetImpl(referenced_expr, cls_expr, consumer);
        if (!should_continue) {
          return kShouldNOTContinue;
        }

        break;
      }
      default: {
        consumer.emit_error(s_expr->_Identifier_Range,
                            std::format("unexpected constraint type: '{}'", magic_enum::enum_name(constr->type)));
        break;
      }
    }
  }

  return kShouldContinue;
}

}  // namespace

void ResolveClassSet(const asn1p_expr_t* s_expr, const asn1p_expr_t* cls_expr,
                     const ClassSetElementConsumer& consumer) {
  ResolveClassSetImpl(s_expr, cls_expr, consumer);
}

}  // namespace vanadium::asn1::ast
