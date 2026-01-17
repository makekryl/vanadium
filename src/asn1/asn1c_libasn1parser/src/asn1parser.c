#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <threads.h>

#include "asn1parser.h"
#include "asn1p_list.h"

#include "asn1p_y.h"
#include "asn1p_l.h"

static int _asn1p_fix_modules(asn1p_t *a, const char *fname);

/*
 * Parse the given buffer.
 */
asn1p_t *
asn1p_parse_buffer(const char *buffer, int size, asn1p_errs_t *errs) {
  asn1p_yctx_t yctx = { .errors = errs };
  asn1p_lctx_t lctx = {0};
  asn1p_t *a = NULL;
  //
  yyscan_t scanner;
  YY_BUFFER_STATE ybuf;
	int ret;

  if (asn1p_lex_init_extra(&lctx, &scanner) != 0) {
    assert(0 && "asn1p_lex_init_extra");
  	return NULL;
  }

	ybuf = asn1p__scan_bytes(buffer, size, scanner);
	if(!ybuf) {
    asn1p_lex_destroy(scanner);
		assert(0 && "asn1p__scan_bytes");
		return 0;
	}

  asn1p_set_lineno(0, scanner);
  ret = asn1p_parse(&yctx, &a, scanner);

  asn1p__delete_buffer(ybuf, scanner);
  asn1p_lex_destroy(scanner);

  if (ret != 0) {
    if (a) {
      asn1p_delete(a);
    }
    return NULL;
  }

  assert(a);
  if (_asn1p_fix_modules(a, "-")) {
    asn1p_delete(a);
    return NULL;	/* FIXME: destroy (a) */
  }

  return a;
}


extern int asn1p_lexer_types_year;
extern int asn1p_lexer_constructs_year;
extern int asn1p_lexer_extended_values;

static int
_asn1p_fix_modules(asn1p_t *a, const char *fname) {
	asn1p_module_t *mod;
	TQ_FOR(mod, &(a->modules), mod_next) {
		mod->source_file_name = asn1p_mem_strdup(fname);
		if(mod->source_file_name == NULL)
			return -1;
		mod->asn1p = a;
	}
	return 0;
}
