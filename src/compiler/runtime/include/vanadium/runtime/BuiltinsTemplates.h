#pragma once

#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/runtime.h"

struct vrt_integer_template_t {
  vrt_template_sel_e tsel;

  //
  // moved out of the inner struct to avoid extra padding, saving up 8 bytes
  bool vmin_exclusive;
  bool vmax_exclusive;
  bool vmin_present;
  bool vmax_present;
  //

  union {
    vrt_integer_t val;
    vanadium::rt::tpl::ValueList<vrt_integer_template_t> list;
    struct {
      vrt_native_int_t vmin;
      vrt_native_int_t vmax;
    } range;
    vanadium::rt::tpl::Implication<vrt_integer_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

struct vrt_float_template_t {
  vrt_template_sel_e tsel;

  //
  // moved out of the inner struct to avoid extra padding, saving up 8 bytes
  bool vmin_exclusive;
  bool vmax_exclusive;
  bool vmin_present;
  bool vmax_present;
  //

  union {
    vrt_float_t val;
    vanadium::rt::tpl::ValueList<vrt_float_template_t> list;
    struct {
      double vmin;
      double vmax;
    } range;
    vanadium::rt::tpl::Implication<vrt_float_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

struct vrt_boolean_template_t {
  vrt_template_sel_e tsel;

  union {
    vrt_boolean_t val;
    vanadium::rt::tpl::ValueList<vrt_boolean_template_t> list;
    vanadium::rt::tpl::Implication<vrt_boolean_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

struct vrt_charstring_template_t {
  vrt_template_sel_e tsel;

  union {
    vrt_charstring_t val;
    vanadium::rt::tpl::ValueList<vrt_charstring_template_t> list;
    vanadium::rt::tpl::Implication<vrt_charstring_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

struct vrt_octetstring_template_t {
  vrt_template_sel_e tsel;

  union {
    vrt_octetstring_t val;
    vanadium::rt::tpl::ValueList<vrt_octetstring_template_t> list;
    vanadium::rt::tpl::Implication<vrt_octetstring_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

struct vrt_bitstring_template_t {
  vrt_template_sel_e tsel;

  union {
    vrt_bitstring_t val;
    vanadium::rt::tpl::ValueList<vrt_bitstring_template_t> list;
    vanadium::rt::tpl::Implication<vrt_bitstring_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

struct vrt_hexstring_template_t {
  vrt_template_sel_e tsel;

  union {
    vrt_hexstring_t val;
    vanadium::rt::tpl::ValueList<vrt_hexstring_template_t> list;
    vanadium::rt::tpl::Implication<vrt_hexstring_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};
