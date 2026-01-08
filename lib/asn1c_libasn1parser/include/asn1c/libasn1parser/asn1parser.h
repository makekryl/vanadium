/*
 * This is a parser of the ASN.1 grammar.
 */
#ifndef	ASN1PARSER_H
#define	ASN1PARSER_H

#include <stdint.h>  //

#include "asn1c/libasn1common/asn1_ref.h"

#include "asn1p_alloc.h"
#include "asn1p_integer.h"
#include "asn1p_list.h"
#include "asn1p_oid.h"		/* Object identifiers (OIDs) */
#include "asn1p_module.h"	/* ASN.1 definition module */
#include "asn1p_value.h"	/* Value definition */
#include "asn1p_param.h"	/* Parameterization */
#include "asn1p_constr.h"	/* Type Constraints */
#include "asn1p_xports.h"	/* IMports/EXports */
#include "asn1p_class.h"	/* CLASS-related stuff */
#include "asn1p_expr.h"		/* A single ASN.1 expression */

typedef struct {
  int pos;
  char *msg;
} asn1p_err_t;

typedef struct {
  asn1p_err_t *data;
  size_t size;
  size_t capacity;
} asn1p_errs_t;

int  asn1p_errs_init(asn1p_errs_t *);
void asn1p_errs_free(asn1p_errs_t *);

/*
 * Perform low-level parsing of ASN.1 module[s]
 * and return a list of module trees.
 */
asn1p_t	*asn1p_parse_buffer(const char *buffer, int size, asn1p_errs_t *errs);

#endif	/* ASN1PARSER_H */
