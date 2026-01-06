/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         ASN1P_STYPE
/* Substitute the variable and function names.  */
#define yyparse         asn1p_parse
#define yylex           asn1p_lex
#define yyerror         asn1p_error
#define yydebug         asn1p_debug
#define yynerrs         asn1p_nerrs

/* First part of user prologue.  */
#line 29 "asn1p_y.y"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>

#include "asn1parser.h"

#include "asn1p_y.h"
#include "asn1p_l.h"

#define YYDEBUG 1
#define YYFPRINTF   prefixed_fprintf

/*
 * Prefix parser debug with "PARSER: " for easier human eye scanning.
 */
static int
__attribute__((format(printf, 2, 3)))
prefixed_fprintf(FILE *f, const char *fmt, ...) {
    static int line_ended = 1;
    va_list ap;
    va_start(ap, fmt);
    if(line_ended) {
        fprintf(f, "PARSER: ");
        line_ended = 0;
    }
    size_t len = strlen(fmt);
    if(len && fmt[len-1] == '\n') {
        line_ended = 1;
    }
    int ret = vfprintf(f, fmt, ap);
    va_end(ap);
    return ret;
}

static int yyerror(void **param, yyscan_t scanner, const char *msg);

void asn1p_lexer_hack_push_opaque_state(yyscan_t);
void asn1p_lexer_hack_enable_with_syntax(yyscan_t);
void asn1p_lexer_hack_push_encoding_control(yyscan_t);
#define ASN_FILENAME "TODO(asn1p_parse_debug_filename)"

/*
 * Process directives as <ASN1C:RepresentAsPointer>
 */
extern int asn1p_as_pointer;

/*
 * This temporary variable is used to solve the shortcomings of 1-lookahead
 * parser.
 */
static struct AssignedIdentifier *saved_aid;

static asn1p_value_t *_convert_bitstring2binary(char *str, int base);
static void _fixup_anonymous_identifier(asn1p_expr_t *expr, yyscan_t);

static asn1p_module_t *currentModule;
#define	NEW_EXPR()	(asn1p_expr_new(asn1p_get_lineno(yyscanner), currentModule))

#define	checkmem(ptr)	do {						\
		if(!(ptr))						\
		return yyerror(param, yyscanner, "Memory failure");			\
	} while(0)

#define	CONSTRAINT_INSERT(root, constr_type, arg1, arg2) do {		\
		if(arg1->type != constr_type) {				\
			int __ret;					\
			root = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);	\
			checkmem(root);					\
			root->type = constr_type;			\
			__ret = asn1p_constraint_insert(root,		\
				arg1);					\
			checkmem(__ret == 0);				\
		} else {						\
			root = arg1;					\
		}							\
		if(arg2) {						\
			int __ret					\
			= asn1p_constraint_insert(root, arg2);		\
			checkmem(__ret == 0);				\
		}							\
	} while(0)

#ifdef	AL_IMPORT
#error	AL_IMPORT DEFINED ELSEWHERE!
#endif
#define AL_IMPORT(to, where, from, field)                                      \
    do {                                                                       \
        if(!(from)) break;                                                     \
        while(TQ_FIRST(&((from)->where))) {                                    \
            TQ_ADD(&((to)->where), TQ_REMOVE(&((from)->where), field), field); \
        }                                                                      \
        assert(TQ_FIRST(&((from)->where)) == 0);                               \
    } while(0)


#line 178 "asn1p_y.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "asn1p_y.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOK_PPEQ = 3,                   /* TOK_PPEQ  */
  YYSYMBOL_TOK_VBracketLeft = 4,           /* TOK_VBracketLeft  */
  YYSYMBOL_TOK_VBracketRight = 5,          /* TOK_VBracketRight  */
  YYSYMBOL_TOK_whitespace = 6,             /* TOK_whitespace  */
  YYSYMBOL_TOK_opaque = 7,                 /* TOK_opaque  */
  YYSYMBOL_TOK_bstring = 8,                /* TOK_bstring  */
  YYSYMBOL_TOK_cstring = 9,                /* TOK_cstring  */
  YYSYMBOL_TOK_hstring = 10,               /* TOK_hstring  */
  YYSYMBOL_TOK_identifier = 11,            /* "identifier"  */
  YYSYMBOL_TOK_number = 12,                /* "number"  */
  YYSYMBOL_TOK_number_negative = 13,       /* "negative number"  */
  YYSYMBOL_TOK_realnumber = 14,            /* TOK_realnumber  */
  YYSYMBOL_TOK_tuple = 15,                 /* TOK_tuple  */
  YYSYMBOL_TOK_quadruple = 16,             /* TOK_quadruple  */
  YYSYMBOL_TOK_typereference = 17,         /* TOK_typereference  */
  YYSYMBOL_TOK_capitalreference = 18,      /* TOK_capitalreference  */
  YYSYMBOL_TOK_typefieldreference = 19,    /* TOK_typefieldreference  */
  YYSYMBOL_TOK_valuefieldreference = 20,   /* TOK_valuefieldreference  */
  YYSYMBOL_TOK_Literal = 21,               /* TOK_Literal  */
  YYSYMBOL_TOK_ExtValue_BIT_STRING = 22,   /* TOK_ExtValue_BIT_STRING  */
  YYSYMBOL_TOK_ABSENT = 23,                /* TOK_ABSENT  */
  YYSYMBOL_TOK_ABSTRACT_SYNTAX = 24,       /* TOK_ABSTRACT_SYNTAX  */
  YYSYMBOL_TOK_ALL = 25,                   /* TOK_ALL  */
  YYSYMBOL_TOK_ANY = 26,                   /* TOK_ANY  */
  YYSYMBOL_TOK_APPLICATION = 27,           /* TOK_APPLICATION  */
  YYSYMBOL_TOK_AUTOMATIC = 28,             /* TOK_AUTOMATIC  */
  YYSYMBOL_TOK_BEGIN = 29,                 /* TOK_BEGIN  */
  YYSYMBOL_TOK_BIT = 30,                   /* TOK_BIT  */
  YYSYMBOL_TOK_BMPString = 31,             /* TOK_BMPString  */
  YYSYMBOL_TOK_BOOLEAN = 32,               /* TOK_BOOLEAN  */
  YYSYMBOL_TOK_BY = 33,                    /* TOK_BY  */
  YYSYMBOL_TOK_CHARACTER = 34,             /* TOK_CHARACTER  */
  YYSYMBOL_TOK_CHOICE = 35,                /* TOK_CHOICE  */
  YYSYMBOL_TOK_CLASS = 36,                 /* TOK_CLASS  */
  YYSYMBOL_TOK_COMPONENT = 37,             /* TOK_COMPONENT  */
  YYSYMBOL_TOK_COMPONENTS = 38,            /* TOK_COMPONENTS  */
  YYSYMBOL_TOK_CONSTRAINED = 39,           /* TOK_CONSTRAINED  */
  YYSYMBOL_TOK_CONTAINING = 40,            /* TOK_CONTAINING  */
  YYSYMBOL_TOK_DEFAULT = 41,               /* TOK_DEFAULT  */
  YYSYMBOL_TOK_DEFINITIONS = 42,           /* TOK_DEFINITIONS  */
  YYSYMBOL_TOK_DEFINED = 43,               /* TOK_DEFINED  */
  YYSYMBOL_TOK_EMBEDDED = 44,              /* TOK_EMBEDDED  */
  YYSYMBOL_TOK_ENCODED = 45,               /* TOK_ENCODED  */
  YYSYMBOL_TOK_ENCODING_CONTROL = 46,      /* TOK_ENCODING_CONTROL  */
  YYSYMBOL_TOK_END = 47,                   /* TOK_END  */
  YYSYMBOL_TOK_ENUMERATED = 48,            /* TOK_ENUMERATED  */
  YYSYMBOL_TOK_EXPLICIT = 49,              /* TOK_EXPLICIT  */
  YYSYMBOL_TOK_EXPORTS = 50,               /* TOK_EXPORTS  */
  YYSYMBOL_TOK_EXTENSIBILITY = 51,         /* TOK_EXTENSIBILITY  */
  YYSYMBOL_TOK_EXTERNAL = 52,              /* TOK_EXTERNAL  */
  YYSYMBOL_TOK_FALSE = 53,                 /* TOK_FALSE  */
  YYSYMBOL_TOK_FROM = 54,                  /* TOK_FROM  */
  YYSYMBOL_TOK_GeneralizedTime = 55,       /* TOK_GeneralizedTime  */
  YYSYMBOL_TOK_GeneralString = 56,         /* TOK_GeneralString  */
  YYSYMBOL_TOK_GraphicString = 57,         /* TOK_GraphicString  */
  YYSYMBOL_TOK_IA5String = 58,             /* TOK_IA5String  */
  YYSYMBOL_TOK_IDENTIFIER = 59,            /* TOK_IDENTIFIER  */
  YYSYMBOL_TOK_IMPLICIT = 60,              /* TOK_IMPLICIT  */
  YYSYMBOL_TOK_IMPLIED = 61,               /* TOK_IMPLIED  */
  YYSYMBOL_TOK_IMPORTS = 62,               /* TOK_IMPORTS  */
  YYSYMBOL_TOK_INCLUDES = 63,              /* TOK_INCLUDES  */
  YYSYMBOL_TOK_INSTANCE = 64,              /* TOK_INSTANCE  */
  YYSYMBOL_TOK_INSTRUCTIONS = 65,          /* TOK_INSTRUCTIONS  */
  YYSYMBOL_TOK_INTEGER = 66,               /* TOK_INTEGER  */
  YYSYMBOL_TOK_ISO646String = 67,          /* TOK_ISO646String  */
  YYSYMBOL_TOK_MAX = 68,                   /* TOK_MAX  */
  YYSYMBOL_TOK_MIN = 69,                   /* TOK_MIN  */
  YYSYMBOL_TOK_MINUS_INFINITY = 70,        /* TOK_MINUS_INFINITY  */
  YYSYMBOL_TOK_NULL = 71,                  /* TOK_NULL  */
  YYSYMBOL_TOK_NumericString = 72,         /* TOK_NumericString  */
  YYSYMBOL_TOK_OBJECT = 73,                /* TOK_OBJECT  */
  YYSYMBOL_TOK_ObjectDescriptor = 74,      /* TOK_ObjectDescriptor  */
  YYSYMBOL_TOK_OCTET = 75,                 /* TOK_OCTET  */
  YYSYMBOL_TOK_OF = 76,                    /* TOK_OF  */
  YYSYMBOL_TOK_OPTIONAL = 77,              /* TOK_OPTIONAL  */
  YYSYMBOL_TOK_PATTERN = 78,               /* TOK_PATTERN  */
  YYSYMBOL_TOK_PDV = 79,                   /* TOK_PDV  */
  YYSYMBOL_TOK_PLUS_INFINITY = 80,         /* TOK_PLUS_INFINITY  */
  YYSYMBOL_TOK_PRESENT = 81,               /* TOK_PRESENT  */
  YYSYMBOL_TOK_PrintableString = 82,       /* TOK_PrintableString  */
  YYSYMBOL_TOK_PRIVATE = 83,               /* TOK_PRIVATE  */
  YYSYMBOL_TOK_REAL = 84,                  /* TOK_REAL  */
  YYSYMBOL_TOK_RELATIVE_OID = 85,          /* TOK_RELATIVE_OID  */
  YYSYMBOL_TOK_SEQUENCE = 86,              /* TOK_SEQUENCE  */
  YYSYMBOL_TOK_SET = 87,                   /* TOK_SET  */
  YYSYMBOL_TOK_SIZE = 88,                  /* TOK_SIZE  */
  YYSYMBOL_TOK_STRING = 89,                /* TOK_STRING  */
  YYSYMBOL_TOK_SYNTAX = 90,                /* TOK_SYNTAX  */
  YYSYMBOL_TOK_T61String = 91,             /* TOK_T61String  */
  YYSYMBOL_TOK_TAGS = 92,                  /* TOK_TAGS  */
  YYSYMBOL_TOK_TeletexString = 93,         /* TOK_TeletexString  */
  YYSYMBOL_TOK_TRUE = 94,                  /* TOK_TRUE  */
  YYSYMBOL_TOK_TYPE_IDENTIFIER = 95,       /* TOK_TYPE_IDENTIFIER  */
  YYSYMBOL_TOK_UNIQUE = 96,                /* TOK_UNIQUE  */
  YYSYMBOL_TOK_UNIVERSAL = 97,             /* TOK_UNIVERSAL  */
  YYSYMBOL_TOK_UniversalString = 98,       /* TOK_UniversalString  */
  YYSYMBOL_TOK_UTCTime = 99,               /* TOK_UTCTime  */
  YYSYMBOL_TOK_UTF8String = 100,           /* TOK_UTF8String  */
  YYSYMBOL_TOK_VideotexString = 101,       /* TOK_VideotexString  */
  YYSYMBOL_TOK_VisibleString = 102,        /* TOK_VisibleString  */
  YYSYMBOL_TOK_WITH = 103,                 /* TOK_WITH  */
  YYSYMBOL_UTF8_BOM = 104,                 /* "UTF-8 byte order mark"  */
  YYSYMBOL_TOK_EXCEPT = 105,               /* TOK_EXCEPT  */
  YYSYMBOL_106_ = 106,                     /* '^'  */
  YYSYMBOL_TOK_INTERSECTION = 107,         /* TOK_INTERSECTION  */
  YYSYMBOL_108_ = 108,                     /* '|'  */
  YYSYMBOL_TOK_UNION = 109,                /* TOK_UNION  */
  YYSYMBOL_TOK_TwoDots = 110,              /* ".."  */
  YYSYMBOL_TOK_ThreeDots = 111,            /* "..."  */
  YYSYMBOL_112_concrete_TypeDeclaration_ = 112, /* "concrete TypeDeclaration"  */
  YYSYMBOL_113_ = 113,                     /* '{'  */
  YYSYMBOL_114_ = 114,                     /* '}'  */
  YYSYMBOL_115_ = 115,                     /* '('  */
  YYSYMBOL_116_ = 116,                     /* ')'  */
  YYSYMBOL_117_ = 117,                     /* ';'  */
  YYSYMBOL_118_ = 118,                     /* ','  */
  YYSYMBOL_119_ = 119,                     /* ':'  */
  YYSYMBOL_120_ = 120,                     /* '['  */
  YYSYMBOL_121_ = 121,                     /* ']'  */
  YYSYMBOL_122_ = 122,                     /* '!'  */
  YYSYMBOL_123_ = 123,                     /* '.'  */
  YYSYMBOL_124_ = 124,                     /* '<'  */
  YYSYMBOL_125_ = 125,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 126,                 /* $accept  */
  YYSYMBOL_ParsedGrammar = 127,            /* ParsedGrammar  */
  YYSYMBOL_ModuleList = 128,               /* ModuleList  */
  YYSYMBOL_ModuleDefinition = 129,         /* ModuleDefinition  */
  YYSYMBOL_130_1 = 130,                    /* $@1  */
  YYSYMBOL_optObjectIdentifier = 131,      /* optObjectIdentifier  */
  YYSYMBOL_ObjectIdentifier = 132,         /* ObjectIdentifier  */
  YYSYMBOL_ObjectIdentifierBody = 133,     /* ObjectIdentifierBody  */
  YYSYMBOL_ObjectIdentifierElement = 134,  /* ObjectIdentifierElement  */
  YYSYMBOL_optModuleDefinitionFlags = 135, /* optModuleDefinitionFlags  */
  YYSYMBOL_ModuleDefinitionFlags = 136,    /* ModuleDefinitionFlags  */
  YYSYMBOL_ModuleDefinitionFlag = 137,     /* ModuleDefinitionFlag  */
  YYSYMBOL_optModuleBody = 138,            /* optModuleBody  */
  YYSYMBOL_ModuleBody = 139,               /* ModuleBody  */
  YYSYMBOL_AssignmentList = 140,           /* AssignmentList  */
  YYSYMBOL_Assignment = 141,               /* Assignment  */
  YYSYMBOL_142_2 = 142,                    /* $@2  */
  YYSYMBOL_optImports = 143,               /* optImports  */
  YYSYMBOL_ImportsDefinition = 144,        /* ImportsDefinition  */
  YYSYMBOL_optImportsBundleSet = 145,      /* optImportsBundleSet  */
  YYSYMBOL_ImportsBundleSet = 146,         /* ImportsBundleSet  */
  YYSYMBOL_AssignedIdentifier = 147,       /* AssignedIdentifier  */
  YYSYMBOL_ImportsBundle = 148,            /* ImportsBundle  */
  YYSYMBOL_ImportsList = 149,              /* ImportsList  */
  YYSYMBOL_ImportsElement = 150,           /* ImportsElement  */
  YYSYMBOL_optExports = 151,               /* optExports  */
  YYSYMBOL_ExportsDefinition = 152,        /* ExportsDefinition  */
  YYSYMBOL_ExportsBody = 153,              /* ExportsBody  */
  YYSYMBOL_ExportsElement = 154,           /* ExportsElement  */
  YYSYMBOL_ValueSet = 155,                 /* ValueSet  */
  YYSYMBOL_ValueSetTypeAssignment = 156,   /* ValueSetTypeAssignment  */
  YYSYMBOL_DefinedType = 157,              /* DefinedType  */
  YYSYMBOL_DataTypeReference = 158,        /* DataTypeReference  */
  YYSYMBOL_ParameterArgumentList = 159,    /* ParameterArgumentList  */
  YYSYMBOL_ParameterArgumentName = 160,    /* ParameterArgumentName  */
  YYSYMBOL_ActualParameterList = 161,      /* ActualParameterList  */
  YYSYMBOL_ActualParameter = 162,          /* ActualParameter  */
  YYSYMBOL_optComponentTypeLists = 163,    /* optComponentTypeLists  */
  YYSYMBOL_ComponentTypeLists = 164,       /* ComponentTypeLists  */
  YYSYMBOL_ComponentType = 165,            /* ComponentType  */
  YYSYMBOL_AlternativeTypeLists = 166,     /* AlternativeTypeLists  */
  YYSYMBOL_AlternativeType = 167,          /* AlternativeType  */
  YYSYMBOL_ObjectClass = 168,              /* ObjectClass  */
  YYSYMBOL_optUNIQUE = 169,                /* optUNIQUE  */
  YYSYMBOL_FieldSpec = 170,                /* FieldSpec  */
  YYSYMBOL_ClassField = 171,               /* ClassField  */
  YYSYMBOL_optWithSyntax = 172,            /* optWithSyntax  */
  YYSYMBOL_WithSyntax = 173,               /* WithSyntax  */
  YYSYMBOL_174_3 = 174,                    /* $@3  */
  YYSYMBOL_WithSyntaxList = 175,           /* WithSyntaxList  */
  YYSYMBOL_WithSyntaxToken = 176,          /* WithSyntaxToken  */
  YYSYMBOL_ExtensionAndException = 177,    /* ExtensionAndException  */
  YYSYMBOL_Type = 178,                     /* Type  */
  YYSYMBOL_TaggedType = 179,               /* TaggedType  */
  YYSYMBOL_DefinedUntaggedType = 180,      /* DefinedUntaggedType  */
  YYSYMBOL_UntaggedType = 181,             /* UntaggedType  */
  YYSYMBOL_MaybeIndirectTaggedType = 182,  /* MaybeIndirectTaggedType  */
  YYSYMBOL_NSTD_IndirectMarker = 183,      /* NSTD_IndirectMarker  */
  YYSYMBOL_MaybeIndirectTypeDeclaration = 184, /* MaybeIndirectTypeDeclaration  */
  YYSYMBOL_TypeDeclaration = 185,          /* TypeDeclaration  */
  YYSYMBOL_ConcreteTypeDeclaration = 186,  /* ConcreteTypeDeclaration  */
  YYSYMBOL_ComplexTypeReference = 187,     /* ComplexTypeReference  */
  YYSYMBOL_ComplexTypeReferenceAmpList = 188, /* ComplexTypeReferenceAmpList  */
  YYSYMBOL_ComplexTypeReferenceElement = 189, /* ComplexTypeReferenceElement  */
  YYSYMBOL_PrimitiveFieldReference = 190,  /* PrimitiveFieldReference  */
  YYSYMBOL_FieldName = 191,                /* FieldName  */
  YYSYMBOL_DefinedObjectClass = 192,       /* DefinedObjectClass  */
  YYSYMBOL_ValueAssignment = 193,          /* ValueAssignment  */
  YYSYMBOL_Value = 194,                    /* Value  */
  YYSYMBOL_195_4 = 195,                    /* $@4  */
  YYSYMBOL_SimpleValue = 196,              /* SimpleValue  */
  YYSYMBOL_DefinedValue = 197,             /* DefinedValue  */
  YYSYMBOL_RestrictedCharacterStringValue = 198, /* RestrictedCharacterStringValue  */
  YYSYMBOL_Opaque = 199,                   /* Opaque  */
  YYSYMBOL_OpaqueFirstToken = 200,         /* OpaqueFirstToken  */
  YYSYMBOL_BasicTypeId = 201,              /* BasicTypeId  */
  YYSYMBOL_BasicTypeId_UniverationCompatible = 202, /* BasicTypeId_UniverationCompatible  */
  YYSYMBOL_BuiltinType = 203,              /* BuiltinType  */
  YYSYMBOL_BasicString = 204,              /* BasicString  */
  YYSYMBOL_UnionMark = 205,                /* UnionMark  */
  YYSYMBOL_IntersectionMark = 206,         /* IntersectionMark  */
  YYSYMBOL_optConstraint = 207,            /* optConstraint  */
  YYSYMBOL_optManyConstraints = 208,       /* optManyConstraints  */
  YYSYMBOL_optSizeOrConstraint = 209,      /* optSizeOrConstraint  */
  YYSYMBOL_Constraint = 210,               /* Constraint  */
  YYSYMBOL_ManyConstraints = 211,          /* ManyConstraints  */
  YYSYMBOL_ConstraintSpec = 212,           /* ConstraintSpec  */
  YYSYMBOL_SubtypeConstraint = 213,        /* SubtypeConstraint  */
  YYSYMBOL_ElementSetSpecs = 214,          /* ElementSetSpecs  */
  YYSYMBOL_ElementSetSpec = 215,           /* ElementSetSpec  */
  YYSYMBOL_Unions = 216,                   /* Unions  */
  YYSYMBOL_Intersections = 217,            /* Intersections  */
  YYSYMBOL_IntersectionElements = 218,     /* IntersectionElements  */
  YYSYMBOL_Elements = 219,                 /* Elements  */
  YYSYMBOL_SubtypeElements = 220,          /* SubtypeElements  */
  YYSYMBOL_PermittedAlphabet = 221,        /* PermittedAlphabet  */
  YYSYMBOL_SizeConstraint = 222,           /* SizeConstraint  */
  YYSYMBOL_PatternConstraint = 223,        /* PatternConstraint  */
  YYSYMBOL_ValueRange = 224,               /* ValueRange  */
  YYSYMBOL_LowerEndValue = 225,            /* LowerEndValue  */
  YYSYMBOL_UpperEndValue = 226,            /* UpperEndValue  */
  YYSYMBOL_SingleValue = 227,              /* SingleValue  */
  YYSYMBOL_BitStringValue = 228,           /* BitStringValue  */
  YYSYMBOL_ContainedSubtype = 229,         /* ContainedSubtype  */
  YYSYMBOL_InnerTypeConstraints = 230,     /* InnerTypeConstraints  */
  YYSYMBOL_SingleTypeConstraint = 231,     /* SingleTypeConstraint  */
  YYSYMBOL_MultipleTypeConstraints = 232,  /* MultipleTypeConstraints  */
  YYSYMBOL_FullSpecification = 233,        /* FullSpecification  */
  YYSYMBOL_PartialSpecification = 234,     /* PartialSpecification  */
  YYSYMBOL_TypeConstraints = 235,          /* TypeConstraints  */
  YYSYMBOL_NamedConstraint = 236,          /* NamedConstraint  */
  YYSYMBOL_optPresenceConstraint = 237,    /* optPresenceConstraint  */
  YYSYMBOL_PresenceConstraint = 238,       /* PresenceConstraint  */
  YYSYMBOL_GeneralConstraint = 239,        /* GeneralConstraint  */
  YYSYMBOL_UserDefinedConstraint = 240,    /* UserDefinedConstraint  */
  YYSYMBOL_241_5 = 241,                    /* $@5  */
  YYSYMBOL_ContentsConstraint = 242,       /* ContentsConstraint  */
  YYSYMBOL_ConstraintRangeSpec = 243,      /* ConstraintRangeSpec  */
  YYSYMBOL_TableConstraint = 244,          /* TableConstraint  */
  YYSYMBOL_SimpleTableConstraint = 245,    /* SimpleTableConstraint  */
  YYSYMBOL_ComponentRelationConstraint = 246, /* ComponentRelationConstraint  */
  YYSYMBOL_AtNotationList = 247,           /* AtNotationList  */
  YYSYMBOL_AtNotationElement = 248,        /* AtNotationElement  */
  YYSYMBOL_ComponentIdList = 249,          /* ComponentIdList  */
  YYSYMBOL_optMarker = 250,                /* optMarker  */
  YYSYMBOL_Marker = 251,                   /* Marker  */
  YYSYMBOL_IdentifierList = 252,           /* IdentifierList  */
  YYSYMBOL_IdentifierElement = 253,        /* IdentifierElement  */
  YYSYMBOL_NamedNumberList = 254,          /* NamedNumberList  */
  YYSYMBOL_NamedNumber = 255,              /* NamedNumber  */
  YYSYMBOL_NamedBitList = 256,             /* NamedBitList  */
  YYSYMBOL_NamedBit = 257,                 /* NamedBit  */
  YYSYMBOL_Enumerations = 258,             /* Enumerations  */
  YYSYMBOL_UniverationList = 259,          /* UniverationList  */
  YYSYMBOL_UniverationElement = 260,       /* UniverationElement  */
  YYSYMBOL_SignedNumber = 261,             /* SignedNumber  */
  YYSYMBOL_RealValue = 262,                /* RealValue  */
  YYSYMBOL_optTag = 263,                   /* optTag  */
  YYSYMBOL_Tag = 264,                      /* Tag  */
  YYSYMBOL_TagTypeValue = 265,             /* TagTypeValue  */
  YYSYMBOL_TagClass = 266,                 /* TagClass  */
  YYSYMBOL_TagPlicit = 267,                /* TagPlicit  */
  YYSYMBOL_TypeRefName = 268,              /* TypeRefName  */
  YYSYMBOL_optIdentifier = 269,            /* optIdentifier  */
  YYSYMBOL_Identifier = 270,               /* Identifier  */
  YYSYMBOL_IdentifierAsReference = 271,    /* IdentifierAsReference  */
  YYSYMBOL_IdentifierAsValue = 272         /* IdentifierAsValue  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined ASN1P_STYPE_IS_TRIVIAL && ASN1P_STYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   919

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  126
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  147
/* YYNRULES -- Number of rules.  */
#define YYNRULES  342
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  522

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   365


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   122,     2,     2,     2,     2,     2,     2,
     115,   116,     2,     2,   118,     2,   123,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   119,   117,
     124,     2,     2,     2,   125,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   120,     2,   121,   106,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   113,   108,   114,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   107,   109,   110,   111,   112
};

#if ASN1P_DEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   435,   435,   438,   444,   449,   466,   466,   495,   496,
     500,   503,   509,   515,   524,   528,   532,   542,   543,   552,
     555,   564,   567,   570,   573,   577,   598,   599,   608,   621,
     624,   641,   648,   662,   670,   669,   683,   696,   697,   700,
     710,   716,   717,   720,   725,   732,   733,   737,   748,   753,
     760,   766,   772,   782,   783,   795,   798,   801,   809,   814,
     821,   827,   833,   842,   845,   865,   875,   895,   901,   917,
     923,   931,   940,   951,   955,   963,   971,   979,   990,   995,
    1002,  1003,  1011,  1019,  1042,  1043,  1046,  1051,  1055,  1063,
    1070,  1076,  1083,  1089,  1094,  1101,  1106,  1109,  1116,  1126,
    1127,  1131,  1138,  1148,  1158,  1169,  1179,  1190,  1200,  1211,
    1223,  1224,  1231,  1230,  1239,  1243,  1250,  1254,  1257,  1261,
    1267,  1275,  1284,  1295,  1298,  1305,  1328,  1351,  1375,  1382,
    1401,  1402,  1405,  1406,  1412,  1418,  1424,  1434,  1444,  1450,
    1462,  1477,  1485,  1493,  1504,  1515,  1537,  1545,  1554,  1558,
    1563,  1572,  1577,  1582,  1590,  1613,  1623,  1624,  1625,  1625,
    1633,  1638,  1643,  1648,  1649,  1650,  1651,  1655,  1656,  1674,
    1678,  1683,  1691,  1700,  1715,  1716,  1722,  1723,  1724,  1725,
    1726,  1727,  1728,  1729,  1730,  1731,  1732,  1733,  1734,  1741,
    1742,  1743,  1747,  1753,  1758,  1763,  1768,  1773,  1782,  1783,
    1787,  1791,  1792,  1793,  1794,  1795,  1799,  1800,  1801,  1802,
    1806,  1807,  1814,  1814,  1815,  1815,  1819,  1820,  1824,  1825,
    1829,  1830,  1831,  1835,  1841,  1842,  1851,  1851,  1853,  1856,
    1860,  1861,  1867,  1878,  1879,  1885,  1886,  1892,  1893,  1900,
    1901,  1907,  1908,  1919,  1925,  1931,  1932,  1934,  1935,  1936,
    1941,  1946,  1951,  1956,  1968,  1977,  1978,  1984,  1985,  1990,
    1993,  1998,  2006,  2012,  2024,  2027,  2033,  2034,  2034,  2035,
    2037,  2050,  2055,  2061,  2075,  2076,  2080,  2083,  2086,  2094,
    2095,  2096,  2101,  2100,  2112,  2121,  2122,  2123,  2124,  2127,
    2130,  2139,  2155,  2161,  2167,  2181,  2192,  2208,  2211,  2231,
    2235,  2239,  2243,  2250,  2255,  2261,  2270,  2275,  2282,  2290,
    2300,  2305,  2312,  2320,  2330,  2345,  2350,  2357,  2364,  2372,
    2380,  2387,  2398,  2402,  2409,  2440,  2441,  2445,  2452,  2458,
    2459,  2460,  2461,  2465,  2466,  2467,  2471,  2475,  2483,  2484,
    2490,  2497,  2504
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if ASN1P_DEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOK_PPEQ",
  "TOK_VBracketLeft", "TOK_VBracketRight", "TOK_whitespace", "TOK_opaque",
  "TOK_bstring", "TOK_cstring", "TOK_hstring", "\"identifier\"",
  "\"number\"", "\"negative number\"", "TOK_realnumber", "TOK_tuple",
  "TOK_quadruple", "TOK_typereference", "TOK_capitalreference",
  "TOK_typefieldreference", "TOK_valuefieldreference", "TOK_Literal",
  "TOK_ExtValue_BIT_STRING", "TOK_ABSENT", "TOK_ABSTRACT_SYNTAX",
  "TOK_ALL", "TOK_ANY", "TOK_APPLICATION", "TOK_AUTOMATIC", "TOK_BEGIN",
  "TOK_BIT", "TOK_BMPString", "TOK_BOOLEAN", "TOK_BY", "TOK_CHARACTER",
  "TOK_CHOICE", "TOK_CLASS", "TOK_COMPONENT", "TOK_COMPONENTS",
  "TOK_CONSTRAINED", "TOK_CONTAINING", "TOK_DEFAULT", "TOK_DEFINITIONS",
  "TOK_DEFINED", "TOK_EMBEDDED", "TOK_ENCODED", "TOK_ENCODING_CONTROL",
  "TOK_END", "TOK_ENUMERATED", "TOK_EXPLICIT", "TOK_EXPORTS",
  "TOK_EXTENSIBILITY", "TOK_EXTERNAL", "TOK_FALSE", "TOK_FROM",
  "TOK_GeneralizedTime", "TOK_GeneralString", "TOK_GraphicString",
  "TOK_IA5String", "TOK_IDENTIFIER", "TOK_IMPLICIT", "TOK_IMPLIED",
  "TOK_IMPORTS", "TOK_INCLUDES", "TOK_INSTANCE", "TOK_INSTRUCTIONS",
  "TOK_INTEGER", "TOK_ISO646String", "TOK_MAX", "TOK_MIN",
  "TOK_MINUS_INFINITY", "TOK_NULL", "TOK_NumericString", "TOK_OBJECT",
  "TOK_ObjectDescriptor", "TOK_OCTET", "TOK_OF", "TOK_OPTIONAL",
  "TOK_PATTERN", "TOK_PDV", "TOK_PLUS_INFINITY", "TOK_PRESENT",
  "TOK_PrintableString", "TOK_PRIVATE", "TOK_REAL", "TOK_RELATIVE_OID",
  "TOK_SEQUENCE", "TOK_SET", "TOK_SIZE", "TOK_STRING", "TOK_SYNTAX",
  "TOK_T61String", "TOK_TAGS", "TOK_TeletexString", "TOK_TRUE",
  "TOK_TYPE_IDENTIFIER", "TOK_UNIQUE", "TOK_UNIVERSAL",
  "TOK_UniversalString", "TOK_UTCTime", "TOK_UTF8String",
  "TOK_VideotexString", "TOK_VisibleString", "TOK_WITH",
  "\"UTF-8 byte order mark\"", "TOK_EXCEPT", "'^'", "TOK_INTERSECTION",
  "'|'", "TOK_UNION", "\"..\"", "\"...\"", "\"concrete TypeDeclaration\"",
  "'{'", "'}'", "'('", "')'", "';'", "','", "':'", "'['", "']'", "'!'",
  "'.'", "'<'", "'@'", "$accept", "ParsedGrammar", "ModuleList",
  "ModuleDefinition", "$@1", "optObjectIdentifier", "ObjectIdentifier",
  "ObjectIdentifierBody", "ObjectIdentifierElement",
  "optModuleDefinitionFlags", "ModuleDefinitionFlags",
  "ModuleDefinitionFlag", "optModuleBody", "ModuleBody", "AssignmentList",
  "Assignment", "$@2", "optImports", "ImportsDefinition",
  "optImportsBundleSet", "ImportsBundleSet", "AssignedIdentifier",
  "ImportsBundle", "ImportsList", "ImportsElement", "optExports",
  "ExportsDefinition", "ExportsBody", "ExportsElement", "ValueSet",
  "ValueSetTypeAssignment", "DefinedType", "DataTypeReference",
  "ParameterArgumentList", "ParameterArgumentName", "ActualParameterList",
  "ActualParameter", "optComponentTypeLists", "ComponentTypeLists",
  "ComponentType", "AlternativeTypeLists", "AlternativeType",
  "ObjectClass", "optUNIQUE", "FieldSpec", "ClassField", "optWithSyntax",
  "WithSyntax", "$@3", "WithSyntaxList", "WithSyntaxToken",
  "ExtensionAndException", "Type", "TaggedType", "DefinedUntaggedType",
  "UntaggedType", "MaybeIndirectTaggedType", "NSTD_IndirectMarker",
  "MaybeIndirectTypeDeclaration", "TypeDeclaration",
  "ConcreteTypeDeclaration", "ComplexTypeReference",
  "ComplexTypeReferenceAmpList", "ComplexTypeReferenceElement",
  "PrimitiveFieldReference", "FieldName", "DefinedObjectClass",
  "ValueAssignment", "Value", "$@4", "SimpleValue", "DefinedValue",
  "RestrictedCharacterStringValue", "Opaque", "OpaqueFirstToken",
  "BasicTypeId", "BasicTypeId_UniverationCompatible", "BuiltinType",
  "BasicString", "UnionMark", "IntersectionMark", "optConstraint",
  "optManyConstraints", "optSizeOrConstraint", "Constraint",
  "ManyConstraints", "ConstraintSpec", "SubtypeConstraint",
  "ElementSetSpecs", "ElementSetSpec", "Unions", "Intersections",
  "IntersectionElements", "Elements", "SubtypeElements",
  "PermittedAlphabet", "SizeConstraint", "PatternConstraint", "ValueRange",
  "LowerEndValue", "UpperEndValue", "SingleValue", "BitStringValue",
  "ContainedSubtype", "InnerTypeConstraints", "SingleTypeConstraint",
  "MultipleTypeConstraints", "FullSpecification", "PartialSpecification",
  "TypeConstraints", "NamedConstraint", "optPresenceConstraint",
  "PresenceConstraint", "GeneralConstraint", "UserDefinedConstraint",
  "$@5", "ContentsConstraint", "ConstraintRangeSpec", "TableConstraint",
  "SimpleTableConstraint", "ComponentRelationConstraint", "AtNotationList",
  "AtNotationElement", "ComponentIdList", "optMarker", "Marker",
  "IdentifierList", "IdentifierElement", "NamedNumberList", "NamedNumber",
  "NamedBitList", "NamedBit", "Enumerations", "UniverationList",
  "UniverationElement", "SignedNumber", "RealValue", "optTag", "Tag",
  "TagTypeValue", "TagClass", "TagPlicit", "TypeRefName", "optIdentifier",
  "Identifier", "IdentifierAsReference", "IdentifierAsValue", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-296)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-300)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      51,  -296,  -296,   217,    41,   217,  -296,  -296,   217,  -296,
    -296,   -63,    53,    43,  -296,  -296,  -296,  -296,    70,  -296,
     -45,   204,  -296,  -296,    79,    32,    30,    38,   110,    87,
     200,   204,  -296,    95,  -296,  -296,  -296,  -296,  -296,   198,
    -296,  -296,   247,    12,   178,  -296,   174,  -296,   134,  -296,
     140,  -296,   183,  -296,  -296,    72,   663,  -296,  -296,  -296,
     213,   173,  -296,   187,   213,  -296,    50,  -296,   211,  -296,
    -296,   293,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,  -296,  -296,  -296,   663,  -296,  -296,  -296,  -296,
    -296,    19,   219,  -296,  -296,  -296,  -296,   217,   213,   227,
    -296,  -296,    -2,   817,    68,   340,  -296,   740,  -296,   120,
     356,   -63,  -296,  -296,  -296,   249,  -296,  -296,   272,  -296,
     274,   285,  -296,  -296,  -296,  -296,  -296,   307,   278,  -296,
    -296,  -296,    13,  -296,   250,  -296,  -296,   251,  -296,  -296,
    -296,   359,   260,   264,   265,   261,   346,   301,   279,   281,
     315,   294,   113,   135,  -296,  -296,   280,  -296,   295,  -296,
    -296,  -296,  -296,  -296,   510,  -296,  -296,   290,  -296,  -296,
    -296,  -296,  -296,   406,   817,   213,   213,   289,   368,  -296,
     217,   309,    14,   378,   299,    29,   108,   318,   403,   280,
      61,   229,   339,  -296,  -296,    61,   341,  -296,  -296,   280,
     604,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,
     296,  -296,  -296,  -296,    84,    28,    98,  -296,    -2,  -296,
    -296,  -296,  -296,  -296,  -296,   264,   265,   311,   280,   219,
    -296,   104,   300,  -296,   388,   280,  -296,  -296,   304,   302,
     237,   241,  -296,   320,  -296,  -296,  -296,  -296,  -296,   -18,
     -17,  -296,  -296,  -296,  -296,  -296,   303,  -296,  -296,  -296,
    -296,   138,  -296,  -296,   403,   403,   305,   167,  -296,  -296,
    -296,  -296,   219,  -296,   314,   312,  -296,  -296,   308,  -296,
     170,  -296,   317,  -296,   353,   319,   316,  -296,  -296,    -3,
     219,   402,   219,   217,   324,  -296,  -296,  -296,  -296,  -296,
    -296,   323,  -296,   403,   331,   403,  -296,   329,  -296,   172,
    -296,  -296,  -296,  -296,   282,   403,  -296,  -296,   510,  -296,
      -3,    10,    -3,  -296,  -296,   351,    10,    -3,   345,   290,
    -296,  -296,   496,  -296,  -296,  -296,  -296,   280,   336,   334,
    -296,  -296,   342,  -296,  -296,   496,  -296,  -296,   496,   496,
     328,   344,   262,   330,  -296,   403,  -296,   181,  -296,   343,
     248,  -296,    29,   740,   280,  -296,  -296,   108,   248,  -296,
     403,   248,   219,  -296,    24,  -296,    -3,   347,  -296,   349,
    -296,   348,   219,  -296,  -296,   219,  -296,   604,  -296,   448,
    -296,  -296,  -296,  -296,  -296,   332,  -296,  -296,  -296,    -3,
    -296,  -296,   371,  -296,  -296,  -296,  -296,  -296,  -296,    25,
    -296,  -296,  -296,  -296,   352,   241,  -296,  -296,  -296,   350,
    -296,  -296,  -296,  -296,  -296,  -296,   403,   196,  -296,  -296,
    -296,  -296,  -296,  -296,   361,   362,  -296,   364,   369,  -296,
      42,  -296,  -296,  -296,  -296,    22,   188,  -296,  -296,  -296,
    -296,  -296,  -296,  -296,  -296,   354,   357,   202,  -296,   280,
     388,  -296,  -296,   370,   373,  -296,  -296,  -296,  -296,    16,
     282,   403,   367,  -296,  -296,   348,  -296,  -296,  -296,   403,
    -296,   403,    77,  -296,  -296,  -296,  -296,  -296,   448,   367,
     403,  -296,    57,   207,  -296,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,  -296,    57,    36,  -296,  -296,  -296,    39,  -296,
    -296,  -296
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   336,   337,     0,     0,     3,     4,     6,     2,     1,
       5,     8,     0,     0,     9,   340,    16,    11,     0,    12,
      14,    17,    10,    13,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    25,    23,    21,    24,    22,     0,
      20,    15,    53,     0,     0,    27,    37,    54,     0,    57,
       0,    58,    60,    62,     7,    41,     0,    38,    56,    55,
       0,     0,    40,     0,    42,    43,     0,    48,    50,    52,
     198,     0,   199,   200,   201,   202,   203,   211,   204,   205,
     206,   207,   208,   209,   210,    28,    29,    33,    31,    32,
      36,   325,   325,    59,    61,    39,    44,     0,     0,     0,
      34,    30,   325,     0,   329,     0,   123,     0,   326,   333,
       0,    45,    49,    51,    35,     0,    68,    67,     0,   176,
       0,     0,   190,   182,   186,   189,   177,     0,     0,   178,
     181,   185,     0,    71,     0,   188,   187,    73,   331,   332,
     330,     0,     0,   141,   142,     0,   138,     0,     0,   190,
       0,   189,   220,   220,   131,   124,   218,   130,    65,   192,
     132,   335,   334,   327,     0,    46,    47,     0,   191,   184,
     183,   180,   179,     0,     0,     0,     0,     0,     0,    64,
       0,     0,     0,     0,   191,   325,     0,     0,     0,     0,
     325,     0,     0,   221,   222,   325,     0,   126,   224,   219,
       0,   260,   169,   261,   322,   323,   324,   170,   171,   161,
     160,   162,   158,   155,   156,   157,   165,   166,   163,   164,
       0,   341,   342,   167,   325,   325,     0,   101,   325,    72,
      77,    76,    75,    74,   328,   141,   142,     0,     0,   325,
     256,     0,     0,   229,     0,   218,   263,   259,     0,   230,
     233,   235,   237,   239,   241,   245,   246,   248,   249,     0,
     243,   244,   247,   143,   149,   150,   145,   146,   148,   144,
     197,     0,   303,   305,     0,     0,   120,     0,    93,    96,
      97,   128,   325,   321,     0,   314,   315,   320,   317,   140,
       0,   306,     0,   251,     0,     0,    85,    86,    92,   299,
     325,     0,   325,   158,     0,   226,   228,   227,   279,   281,
     280,   289,   290,   338,     0,   338,   225,   160,    83,     0,
      78,    80,    81,    82,     0,     0,   154,   151,     0,   301,
     299,   299,   299,   103,   300,    99,   299,   299,   110,     0,
      70,    69,     0,   250,   262,   252,   253,     0,     0,     0,
     125,    63,     0,   212,   213,     0,   214,   215,     0,     0,
     285,     0,     0,     0,   196,     0,   139,     0,   310,     0,
       0,   133,   325,     0,   218,    95,   194,     0,     0,   193,
       0,     0,   325,   134,   325,    90,   299,     0,   284,     0,
     223,     0,   325,   339,   135,   325,    66,     0,   174,   159,
     172,   175,   168,   302,   108,     0,   107,   109,   100,   299,
     105,   106,     0,    98,   111,   102,   234,   266,   264,     0,
     265,   267,   268,   242,   231,   236,   238,   240,   286,   287,
     258,   254,   257,   147,   304,   195,     0,     0,   121,   122,
      94,   129,   127,   316,     0,     0,   307,     0,     0,    91,
     325,    87,    89,   282,   291,     0,     0,   293,   128,   128,
      79,   173,   152,   153,   104,     0,     0,     0,   271,   216,
       0,   288,   311,     0,     0,   319,   318,   309,   308,     0,
       0,     0,   295,   297,   292,     0,   136,   137,   112,     0,
     269,     0,   274,   217,   232,   312,   313,    88,   283,   296,
       0,   294,     0,     0,   272,   277,   278,   276,   273,   275,
     298,   116,   117,     0,     0,   114,   118,   270,     0,   113,
     115,   119
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -296,  -296,   461,   326,  -296,  -296,   376,  -296,   447,  -296,
    -296,   441,  -296,  -296,  -296,   399,  -296,  -296,  -296,  -296,
    -296,  -296,   428,  -296,   395,  -296,  -296,  -296,   434,   355,
    -296,  -106,  -296,  -296,   321,  -296,    99,   322,    48,   115,
    -296,   128,   287,  -296,  -296,   163,  -296,  -296,  -296,     3,
    -196,  -172,   -82,  -296,  -296,   422,  -177,  -296,  -104,   157,
    -296,   358,  -296,   168,  -164,   310,   313,  -296,  -140,  -296,
    -191,  -188,  -296,    52,  -296,   -76,  -296,  -296,   -24,  -296,
    -296,  -296,  -227,   380,  -150,  -296,  -296,  -296,   360,  -237,
    -296,   179,   182,  -258,  -296,  -296,   205,  -296,  -296,  -296,
    -296,   175,  -296,  -296,  -296,  -296,  -296,  -296,  -296,    47,
      55,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,    54,    60,  -220,  -296,  -296,   177,  -296,   164,
    -296,   107,  -296,  -296,   171,  -160,  -296,   -87,  -296,  -296,
    -296,  -296,    11,   232,   -12,  -296,  -295
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     4,     5,     6,    11,    13,    14,    18,    19,    30,
      31,    32,    44,    45,    85,    86,   114,    56,    57,    63,
      64,   166,    65,    66,    67,    46,    47,    50,    51,   318,
      87,   245,    88,   132,   133,   319,   320,   295,   296,   297,
     277,   278,   116,   409,   226,   227,   413,   414,   502,   514,
     515,   298,   105,   106,   246,   321,   299,   373,   374,   156,
     157,   158,   266,   267,   516,   331,   332,    89,   247,   324,
     214,   215,   216,   399,   400,   159,   135,   160,   136,   355,
     358,   492,   197,   192,   198,   199,   304,   305,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     431,   260,   217,   261,   262,   418,   420,   421,   422,   467,
     468,   508,   509,   307,   308,   480,   309,   362,   310,   311,
     312,   456,   457,   482,   333,   334,   271,   272,   290,   291,
     367,   368,   284,   285,   286,   218,   219,   281,   108,   109,
     141,   163,   220,   392,   221,   222,   223
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      20,   154,   193,   193,   107,   107,    20,   349,   280,   322,
     110,     7,   323,   279,     7,   107,     7,   268,   350,     7,
     117,   497,   102,    15,   213,    15,   287,   134,   450,     1,
       2,    53,    90,    15,   115,    15,    15,    48,   328,   293,
      15,     9,   511,    69,    92,   511,   326,   327,    53,   316,
      12,   328,    69,    15,    52,   264,   265,   512,   264,   265,
     512,    90,   294,   511,    15,    16,    68,    91,     1,     2,
      24,    52,    15,    92,   329,    68,   264,   265,   512,   385,
     294,    15,    16,    15,   416,    21,    69,   329,   343,     1,
       2,    33,   360,  -255,   154,   138,    91,    34,   134,   294,
     505,   427,   326,   327,    97,   375,   361,  -255,   111,    68,
     404,   406,   407,   345,   137,    15,   410,   411,   104,    15,
     204,   205,    35,   386,   469,   328,    62,   173,   270,    49,
      36,   174,   103,   405,   384,   276,   466,   107,   107,   104,
     276,   107,   330,   335,   104,   481,   341,   442,   104,   104,
     519,   139,   107,   276,   506,     3,   513,   344,   507,   513,
     521,   329,   104,   231,   233,   140,   452,    17,    98,   161,
     273,    37,   276,   282,   288,   -84,   292,   513,   300,    38,
     162,   104,   438,   300,    22,   137,   230,   232,   403,   464,
     444,   263,   269,   447,   469,   280,   469,   417,  -299,   268,
     279,   189,  -299,    39,   104,   449,   322,    15,   473,   323,
     439,    41,   338,     1,     2,   107,   339,   287,   445,   283,
     388,   448,    25,   189,    15,    54,   190,    42,   191,   346,
       1,     2,    26,   494,     1,     2,    55,   201,   202,   203,
      15,   204,   205,   206,   207,   208,   235,   236,   195,   474,
     191,    58,   364,    27,   237,    28,   365,    59,    60,    15,
     204,   205,   366,   369,    29,     1,     2,   154,   301,   302,
     201,   202,   203,    15,   204,   205,   206,   207,   208,     1,
       2,   371,   209,   238,   379,   372,   396,    94,   380,   398,
     397,   154,   239,    15,   -26,   435,    61,    43,   240,   436,
     210,   393,   484,   393,    95,   458,   485,   241,   459,   224,
     225,   100,   401,   402,   389,   209,   490,   189,   520,   493,
     491,   517,   520,   211,    99,   491,     1,     2,   264,   265,
     430,    10,   242,   210,    10,   143,   144,   347,   348,   104,
     243,   113,   303,   142,   244,   353,   354,   356,   357,   264,
     265,   462,   463,   273,   486,   487,   211,   194,   194,   164,
     282,   168,   167,   169,   170,   288,   171,   172,   292,   175,
     176,   177,   300,   178,   182,   212,   201,   202,   203,    15,
     204,   205,   206,   207,   208,   235,   236,   180,   181,   183,
     184,   187,   185,   237,   186,   191,   201,   202,   203,    15,
     204,   205,   206,   207,   208,   235,   236,   188,   200,   228,
     234,   274,   275,   237,    15,   313,   342,   315,   351,   325,
     352,   209,   238,   378,   369,   359,   363,   370,   376,   382,
     377,   239,   381,   383,   384,   387,   391,   240,   300,   210,
     390,   209,   238,   483,  -177,   394,   241,   408,   412,   419,
     423,   239,   428,   424,   429,   461,   189,   240,   437,   210,
     453,   465,   211,   454,     8,    23,   241,   488,   401,   483,
     470,   242,    40,   455,   471,   489,   189,   475,   476,   243,
     477,   212,   211,   244,   101,   478,   495,   165,   510,   496,
     500,   242,    96,   112,    93,   229,   460,   179,   479,   451,
     440,   212,   415,   244,   201,   202,   203,    15,   204,   205,
     206,   207,   208,   235,   236,   340,   518,   314,   201,   202,
     203,    15,   204,   205,   206,   207,   208,     1,     2,   155,
     441,   433,   498,   196,   425,   336,   503,   432,   337,   501,
     426,   499,   434,   472,   446,   289,   504,   395,   443,   209,
     238,   306,     0,     0,     0,     0,     0,     0,     0,   239,
       0,     0,     0,   209,     0,   240,     0,   210,     0,     0,
       0,     0,     0,     0,   241,     0,     0,     0,     0,     0,
       0,   210,     0,     0,   189,     0,     0,     0,     0,     0,
     211,     0,     0,     0,     0,     0,     0,     0,     0,   242,
       0,     0,     0,     0,   211,     0,     0,     0,     0,   212,
       0,   244,   201,   202,   203,    15,   204,   205,   206,   207,
     208,   235,   236,   212,     0,     0,   145,     0,     0,     0,
     146,     0,     0,     0,   147,    70,   119,     0,   120,   148,
       0,     0,     0,     0,     0,     0,     0,     0,   121,     0,
       0,     0,   149,     0,     0,     0,   123,   209,     0,   124,
      72,    73,    74,     0,     0,     0,     0,     0,   150,     0,
     151,    75,     0,     0,    15,   317,    76,   127,    77,   128,
       1,     2,     0,     0,     0,     0,    78,     0,   129,   130,
     152,   153,     0,     0,    70,    79,     0,    80,   211,     0,
       0,     0,    81,   131,    82,    83,    84,     0,     0,    71,
       0,     0,     0,     0,     0,     0,     0,   178,     0,    72,
      73,    74,     0,     0,     0,     0,     0,     0,     0,     0,
      75,     0,     0,     0,     0,    76,     0,    77,     0,     0,
       0,     0,     0,     0,     0,    78,     0,     0,     0,     0,
       0,     0,     0,     0,    79,     0,    80,   143,   144,     0,
       0,    81,   145,    82,    83,    84,   146,     0,     0,     0,
     147,    70,   119,     0,   120,   148,     0,     0,     0,     0,
       0,     0,     0,     0,   121,     0,     0,     0,   149,     0,
       0,     0,   123,     0,     0,   124,    72,    73,    74,     0,
       0,     0,     0,     0,   150,     0,   151,    75,     0,     0,
       0,   126,    76,   127,    77,   128,     0,     0,     0,     0,
       0,     0,    78,     0,   129,   130,   152,   153,     0,     0,
       0,    79,     0,    80,     1,     2,     0,     0,    81,   131,
      82,    83,    84,     0,     0,     0,     0,   118,    70,   119,
       0,   120,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   121,     0,     0,     0,   122,     0,     0,     0,   123,
       0,     0,   124,    72,    73,    74,     0,     0,     0,     0,
       0,     0,     0,   125,    75,     0,     0,     0,   126,    76,
     127,    77,   128,     0,     0,     0,     0,     0,     0,    78,
       0,   129,   130,     0,     0,     0,     0,     0,    79,     0,
      80,     0,     0,     0,     0,    81,   131,    82,    83,    84
};

static const yytype_int16 yycheck[] =
{
      12,   107,   152,   153,    91,    92,    18,   244,   185,   200,
      92,     0,   200,   185,     3,   102,     5,   181,   245,     8,
     102,     5,     3,    11,   164,    11,   186,   103,     4,    17,
      18,    43,    56,    11,    36,    11,    11,    25,    41,   189,
      11,     0,     6,    55,    56,     6,    18,    19,    60,   199,
     113,    41,    64,    11,    43,    19,    20,    21,    19,    20,
      21,    85,    38,     6,    11,    12,    55,    56,    17,    18,
     115,    60,    11,    85,    77,    64,    19,    20,    21,   299,
      38,    11,    12,    11,   342,    42,    98,    77,   238,    17,
      18,    12,   110,   110,   200,    27,    85,    65,   174,    38,
      23,   359,    18,    19,    54,   282,   124,   124,    97,    98,
     330,   331,   332,     9,   103,    11,   336,   337,   120,    11,
      12,    13,    92,   300,   419,    41,    54,   114,   114,   117,
      92,   118,   113,   123,   118,   111,   111,   224,   225,   120,
     111,   228,   224,   225,   120,   123,   228,   374,   120,   120,
     114,    83,   239,   111,    77,   104,   120,   239,    81,   120,
     121,    77,   120,   175,   176,    97,   386,   114,   118,    49,
     182,    61,   111,   185,   186,   114,   188,   120,   190,    92,
      60,   120,   370,   195,   114,   174,   175,   176,   328,   409,
     378,   180,   181,   381,   489,   372,   491,   347,   114,   363,
     372,    88,   118,     3,   120,   382,   397,    11,    12,   397,
     370,   116,   114,    17,    18,   302,   118,   377,   378,   111,
     302,   381,    18,    88,    11,    47,   113,    29,   115,   241,
      17,    18,    28,   470,    17,    18,    62,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,   113,   437,
     115,   117,   114,    49,    25,    51,   118,   117,   118,    11,
      12,    13,   274,   275,    60,    17,    18,   373,    39,    40,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,   114,    53,    54,   114,   118,   114,   114,   118,     7,
     118,   397,    63,    11,    47,   114,   113,    50,    69,   118,
      71,   313,   114,   315,   117,   392,   118,    78,   395,    19,
      20,    18,   324,   325,   303,    53,   114,    88,   514,   469,
     118,   114,   518,    94,   113,   118,    17,    18,    19,    20,
      68,     5,   103,    71,     8,    17,    18,    37,    38,   120,
     111,   114,   113,     3,   115,   108,   109,   106,   107,    19,
      20,    19,    20,   365,   458,   459,    94,   152,   153,     3,
     372,    89,   113,    89,    79,   377,    59,    89,   380,   119,
     119,    12,   384,   113,   113,   113,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,   123,   123,    43,
      89,    76,   113,    25,   113,   115,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,   113,   113,     3,
     121,    33,   113,    25,    11,    76,   105,    76,   114,   123,
     118,    53,    54,   115,   436,   105,   123,   122,   114,    76,
     118,    63,   115,   114,   118,    33,   113,    69,   450,    71,
     116,    53,    54,   455,   115,   114,    78,    96,   103,   113,
     116,    63,   124,   111,   110,     7,    88,    69,   115,    71,
     113,    90,    94,   114,     3,    18,    78,   113,   480,   481,
     118,   103,    31,   125,   124,   118,    88,   116,   116,   111,
     116,   113,    94,   115,    85,   116,   116,   111,   500,   116,
     123,   103,    64,    98,    60,   174,   397,   142,   450,   384,
     372,   113,   339,   115,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,   228,   513,   195,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,   107,
     373,   363,   480,   153,   355,   225,   489,   362,   225,   485,
     358,   481,   365,   436,   380,   187,   491,   315,   377,    53,
      54,   191,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      -1,    -1,    -1,    53,    -1,    69,    -1,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,
      -1,    71,    -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,
      94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,
      -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,   113,
      -1,   115,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,   113,    -1,    -1,    22,    -1,    -1,    -1,
      26,    -1,    -1,    -1,    30,    31,    32,    -1,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    52,    53,    -1,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    64,    -1,
      66,    67,    -1,    -1,    11,    71,    72,    73,    74,    75,
      17,    18,    -1,    -1,    -1,    -1,    82,    -1,    84,    85,
      86,    87,    -1,    -1,    31,    91,    -1,    93,    94,    -1,
      -1,    -1,    98,    99,   100,   101,   102,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,    -1,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    -1,    -1,    -1,    -1,    72,    -1,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    91,    -1,    93,    17,    18,    -1,
      -1,    98,    22,   100,   101,   102,    26,    -1,    -1,    -1,
      30,    31,    32,    -1,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    48,    -1,
      -1,    -1,    52,    -1,    -1,    55,    56,    57,    58,    -1,
      -1,    -1,    -1,    -1,    64,    -1,    66,    67,    -1,    -1,
      -1,    71,    72,    73,    74,    75,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    84,    85,    86,    87,    -1,    -1,
      -1,    91,    -1,    93,    17,    18,    -1,    -1,    98,    99,
     100,   101,   102,    -1,    -1,    -1,    -1,    30,    31,    32,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    48,    -1,    -1,    -1,    52,
      -1,    -1,    55,    56,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    67,    -1,    -1,    -1,    71,    72,
      73,    74,    75,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    91,    -1,
      93,    -1,    -1,    -1,    -1,    98,    99,   100,   101,   102
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,    17,    18,   104,   127,   128,   129,   268,   128,     0,
     129,   130,   113,   131,   132,    11,    12,   114,   133,   134,
     270,    42,   114,   134,   115,    18,    28,    49,    51,    60,
     135,   136,   137,    12,    65,    92,    92,    61,    92,     3,
     137,   116,    29,    50,   138,   139,   151,   152,    25,   117,
     153,   154,   268,   270,    47,    62,   143,   144,   117,   117,
     118,   113,    54,   145,   146,   148,   149,   150,   268,   270,
      31,    46,    56,    57,    58,    67,    72,    74,    82,    91,
      93,    98,   100,   101,   102,   140,   141,   156,   158,   193,
     204,   268,   270,   154,   114,   117,   148,    54,   118,   113,
      18,   141,     3,   113,   120,   178,   179,   263,   264,   265,
     178,   268,   150,   114,   142,    36,   168,   178,    30,    32,
      34,    44,    48,    52,    55,    66,    71,    73,    75,    84,
      85,    99,   159,   160,   201,   202,   204,   268,    27,    83,
      97,   266,     3,    17,    18,    22,    26,    30,    35,    48,
      64,    66,    86,    87,   157,   181,   185,   186,   187,   201,
     203,    49,    60,   267,     3,   132,   147,   113,    89,    89,
      79,    59,    89,   114,   118,   119,   119,    12,   113,   155,
     123,   123,   113,    43,    89,   113,   113,    76,   113,    88,
     113,   115,   209,   210,   222,   113,   209,   208,   210,   211,
     113,     8,     9,    10,    12,    13,    14,    15,    16,    53,
      71,    94,   113,   194,   196,   197,   198,   228,   261,   262,
     268,   270,   271,   272,    19,    20,   170,   171,     3,   160,
     268,   270,   268,   270,   121,    17,    18,    25,    54,    63,
      69,    78,   103,   111,   115,   157,   180,   194,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     227,   229,   230,   268,    19,    20,   188,   189,   190,   268,
     114,   252,   253,   270,    33,   113,   111,   166,   167,   177,
     182,   263,   270,   111,   258,   259,   260,   261,   270,   187,
     254,   255,   270,   210,    38,   163,   164,   165,   177,   182,
     270,    39,    40,   113,   212,   213,   214,   239,   240,   242,
     244,   245,   246,    76,   163,    76,   210,    71,   155,   161,
     162,   181,   196,   197,   195,   123,    18,    19,    41,    77,
     178,   191,   192,   250,   251,   178,   191,   192,   114,   118,
     168,   178,   105,   210,   178,     9,   270,    37,    38,   215,
     208,   114,   118,   108,   109,   205,   106,   107,   206,   105,
     110,   124,   243,   123,   114,   118,   270,   256,   257,   270,
     122,   114,   118,   183,   184,   182,   114,   118,   115,   114,
     118,   115,    76,   114,   118,   250,   182,    33,   178,   268,
     116,   113,   269,   270,   114,   269,   114,   118,     7,   199,
     200,   270,   270,   194,   250,   123,   250,   250,    96,   169,
     250,   250,   103,   172,   173,   171,   219,   210,   231,   113,
     232,   233,   234,   116,   111,   217,   218,   219,   124,   110,
      68,   226,   227,   189,   253,   114,   118,   115,   197,   261,
     167,   185,   208,   260,   197,   261,   255,   197,   261,   182,
       4,   165,   250,   113,   114,   125,   247,   248,   263,   263,
     162,     7,    19,    20,   250,    90,   111,   235,   236,   272,
     118,   124,   257,    12,   197,   116,   116,   116,   116,   164,
     241,   123,   249,   270,   114,   118,   184,   184,   113,   118,
     114,   118,   207,   210,   215,   116,   116,     5,   199,   249,
     123,   248,   174,   235,   236,    23,    77,    81,   237,   238,
     270,     6,    21,   120,   175,   176,   190,   114,   175,   114,
     176,   121
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   126,   127,   127,   128,   128,   130,   129,   131,   131,
     132,   132,   133,   133,   134,   134,   134,   135,   135,   136,
     136,   137,   137,   137,   137,   137,   138,   138,   139,   140,
     140,   141,   141,   141,   142,   141,   141,   143,   143,   144,
     144,   145,   145,   146,   146,   147,   147,   148,   149,   149,
     150,   150,   150,   151,   151,   152,   152,   152,   153,   153,
     154,   154,   154,   155,   156,   157,   157,   158,   158,   158,
     158,   159,   159,   160,   160,   160,   160,   160,   161,   161,
     162,   162,   162,   162,   163,   163,   164,   164,   164,   165,
     165,   165,   165,   166,   166,   167,   167,   167,   168,   169,
     169,   170,   170,   171,   171,   171,   171,   171,   171,   171,
     172,   172,   174,   173,   175,   175,   176,   176,   176,   176,
     177,   177,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   185,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   187,   187,   187,   187,   187,   188,   188,   189,   190,
     190,   191,   191,   191,   192,   193,   194,   194,   195,   194,
     196,   196,   196,   196,   196,   196,   196,   197,   197,   198,
     198,   198,   199,   199,   200,   200,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   202,
     202,   202,   203,   203,   203,   203,   203,   203,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   205,   205,   206,   206,   207,   207,   208,   208,
     209,   209,   209,   210,   211,   211,   212,   212,   213,   214,
     214,   214,   214,   215,   215,   216,   216,   217,   217,   218,
     218,   219,   219,   220,   220,   220,   220,   220,   220,   220,
     221,   222,   223,   223,   224,   225,   225,   226,   226,   227,
     228,   228,   229,   229,   230,   230,   231,   232,   232,   233,
     234,   235,   235,   236,   237,   237,   238,   238,   238,   239,
     239,   239,   241,   240,   242,   243,   243,   243,   243,   244,
     244,   245,   246,   247,   247,   248,   248,   249,   249,   250,
     250,   251,   251,   252,   252,   253,   254,   254,   255,   255,
     256,   256,   257,   257,   258,   259,   259,   260,   260,   260,
     260,   260,   261,   261,   262,   263,   263,   264,   265,   266,
     266,   266,   266,   267,   267,   267,   268,   268,   269,   269,
     270,   271,   272
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     2,     0,     9,     0,     1,
       3,     2,     1,     2,     1,     4,     1,     0,     1,     1,
       2,     2,     2,     2,     2,     2,     0,     1,     3,     1,
       2,     1,     1,     1,     0,     3,     1,     0,     1,     3,
       2,     0,     1,     1,     2,     0,     1,     4,     1,     3,
       1,     3,     1,     0,     1,     3,     3,     2,     1,     3,
       1,     3,     1,     3,     4,     1,     4,     3,     3,     6,
       6,     1,     3,     1,     3,     3,     3,     3,     1,     3,
       1,     1,     1,     1,     0,     1,     1,     3,     5,     3,
       2,     3,     1,     1,     3,     2,     1,     1,     5,     0,
       1,     1,     3,     2,     4,     3,     3,     3,     3,     3,
       0,     1,     0,     6,     1,     2,     1,     1,     1,     3,
       1,     3,     3,     1,     2,     2,     2,     3,     0,     2,
       1,     1,     1,     4,     4,     4,     6,     6,     1,     4,
       3,     1,     1,     3,     3,     3,     1,     3,     1,     1,
       1,     1,     3,     3,     1,     4,     1,     1,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     2,     1,     4,     4,     5,     4,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     0,     1,
       0,     1,     1,     3,     1,     2,     1,     1,     1,     1,
       1,     3,     5,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     3,     3,     1,     1,     1,     3,
       5,     1,     3,     3,     0,     1,     1,     1,     1,     1,
       1,     1,     0,     5,     2,     1,     2,     2,     3,     1,
       1,     3,     4,     1,     3,     2,     3,     1,     3,     0,
       1,     1,     2,     1,     3,     1,     1,     3,     4,     4,
       1,     3,     4,     4,     1,     1,     3,     1,     4,     4,
       1,     1,     1,     1,     1,     0,     1,     2,     4,     0,
       1,     1,     1,     0,     1,     1,     1,     1,     0,     1,
       1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = ASN1P_EMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == ASN1P_EMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (ctx, yyscanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use ASN1P_error or ASN1P_UNDEF. */
#define YYERRCODE ASN1P_UNDEF


/* Enable debugging if requested.  */
#if ASN1P_DEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, ctx, yyscanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, asn1p_yctx_t *ctx, yyscan_t yyscanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (ctx);
  YY_USE (yyscanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, asn1p_yctx_t *ctx, yyscan_t yyscanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, ctx, yyscanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, asn1p_yctx_t *ctx, yyscan_t yyscanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], ctx, yyscanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, ctx, yyscanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !ASN1P_DEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !ASN1P_DEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, asn1p_yctx_t *ctx, yyscan_t yyscanner)
{
  YY_USE (yyvaluep);
  YY_USE (ctx);
  YY_USE (yyscanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (asn1p_yctx_t *ctx, yyscan_t yyscanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = ASN1P_EMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == ASN1P_EMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, yyscanner);
    }

  if (yychar <= ASN1P_EOF)
    {
      yychar = ASN1P_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == ASN1P_error)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = ASN1P_UNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = ASN1P_EMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* ParsedGrammar: "UTF-8 byte order mark" ModuleList  */
#line 435 "asn1p_y.y"
                            {
		*(void **)param = (yyvsp[0].a_grammar);
	}
#line 1985 "asn1p_y.c"
    break;

  case 3: /* ParsedGrammar: ModuleList  */
#line 438 "asn1p_y.y"
                     {
		*(void **)param = (yyvsp[0].a_grammar);
	}
#line 1993 "asn1p_y.c"
    break;

  case 4: /* ModuleList: ModuleDefinition  */
#line 444 "asn1p_y.y"
                         {
		(yyval.a_grammar) = asn1p_new();
		checkmem((yyval.a_grammar));
		TQ_ADD(&((yyval.a_grammar)->modules), (yyvsp[0].a_module), mod_next);
	}
#line 2003 "asn1p_y.c"
    break;

  case 5: /* ModuleList: ModuleList ModuleDefinition  */
#line 449 "asn1p_y.y"
                                      {
		(yyval.a_grammar) = (yyvsp[-1].a_grammar);
		TQ_ADD(&((yyval.a_grammar)->modules), (yyvsp[0].a_module), mod_next);
	}
#line 2012 "asn1p_y.c"
    break;

  case 6: /* $@1: %empty  */
#line 466 "asn1p_y.y"
                    { currentModule = asn1p_module_new(); }
#line 2018 "asn1p_y.c"
    break;

  case 7: /* ModuleDefinition: TypeRefName $@1 optObjectIdentifier TOK_DEFINITIONS optModuleDefinitionFlags TOK_PPEQ TOK_BEGIN optModuleBody TOK_END  */
#line 471 "asn1p_y.y"
                        {

		(yyval.a_module) = currentModule;

		if((yyvsp[-1].a_module)) {
			asn1p_module_t tmp = *((yyval.a_module));
			*((yyval.a_module)) = *((yyvsp[-1].a_module));
			*((yyvsp[-1].a_module)) = tmp;
			asn1p_module_free((yyvsp[-1].a_module));
		} else {
			/* There's a chance that a module is just plain empty */
		}

		(yyval.a_module)->ModuleName = (yyvsp[-8].tv_str);
		(yyval.a_module)->module_oid = (yyvsp[-6].a_oid);
		(yyval.a_module)->module_flags = (yyvsp[-4].a_module_flags);
	}
#line 2040 "asn1p_y.c"
    break;

  case 8: /* optObjectIdentifier: %empty  */
#line 495 "asn1p_y.y"
        { (yyval.a_oid) = 0; }
#line 2046 "asn1p_y.c"
    break;

  case 9: /* optObjectIdentifier: ObjectIdentifier  */
#line 496 "asn1p_y.y"
                           { (yyval.a_oid) = (yyvsp[0].a_oid); }
#line 2052 "asn1p_y.c"
    break;

  case 10: /* ObjectIdentifier: '{' ObjectIdentifierBody '}'  */
#line 500 "asn1p_y.y"
                                     {
		(yyval.a_oid) = (yyvsp[-1].a_oid);
	}
#line 2060 "asn1p_y.c"
    break;

  case 11: /* ObjectIdentifier: '{' '}'  */
#line 503 "asn1p_y.y"
                  {
		(yyval.a_oid) = 0;
	}
#line 2068 "asn1p_y.c"
    break;

  case 12: /* ObjectIdentifierBody: ObjectIdentifierElement  */
#line 509 "asn1p_y.y"
                                {
		(yyval.a_oid) = asn1p_oid_new();
		asn1p_oid_add_arc((yyval.a_oid), &(yyvsp[0].a_oid_arc));
		if((yyvsp[0].a_oid_arc).name)
			free((yyvsp[0].a_oid_arc).name);
	}
#line 2079 "asn1p_y.c"
    break;

  case 13: /* ObjectIdentifierBody: ObjectIdentifierBody ObjectIdentifierElement  */
#line 515 "asn1p_y.y"
                                                       {
		(yyval.a_oid) = (yyvsp[-1].a_oid);
		asn1p_oid_add_arc((yyval.a_oid), &(yyvsp[0].a_oid_arc));
		if((yyvsp[0].a_oid_arc).name)
			free((yyvsp[0].a_oid_arc).name);
	}
#line 2090 "asn1p_y.c"
    break;

  case 14: /* ObjectIdentifierElement: Identifier  */
#line 524 "asn1p_y.y"
                   {					/* iso */
		(yyval.a_oid_arc).name = (yyvsp[0].tv_str);
		(yyval.a_oid_arc).number = -1;
	}
#line 2099 "asn1p_y.c"
    break;

  case 15: /* ObjectIdentifierElement: Identifier '(' "number" ')'  */
#line 528 "asn1p_y.y"
                                        {		/* iso(1) */
		(yyval.a_oid_arc).name = (yyvsp[-3].tv_str);
		(yyval.a_oid_arc).number = (yyvsp[-1].a_int);
	}
#line 2108 "asn1p_y.c"
    break;

  case 16: /* ObjectIdentifierElement: "number"  */
#line 532 "asn1p_y.y"
                     {					/* 1 */
		(yyval.a_oid_arc).name = 0;
		(yyval.a_oid_arc).number = (yyvsp[0].a_int);
	}
#line 2117 "asn1p_y.c"
    break;

  case 17: /* optModuleDefinitionFlags: %empty  */
#line 542 "asn1p_y.y"
        { (yyval.a_module_flags) = MSF_NOFLAGS; }
#line 2123 "asn1p_y.c"
    break;

  case 18: /* optModuleDefinitionFlags: ModuleDefinitionFlags  */
#line 543 "asn1p_y.y"
                                {
		(yyval.a_module_flags) = (yyvsp[0].a_module_flags);
	}
#line 2131 "asn1p_y.c"
    break;

  case 19: /* ModuleDefinitionFlags: ModuleDefinitionFlag  */
#line 552 "asn1p_y.y"
                             {
		(yyval.a_module_flags) = (yyvsp[0].a_module_flags);
	}
#line 2139 "asn1p_y.c"
    break;

  case 20: /* ModuleDefinitionFlags: ModuleDefinitionFlags ModuleDefinitionFlag  */
#line 555 "asn1p_y.y"
                                                     {
		(yyval.a_module_flags) = (yyvsp[-1].a_module_flags) | (yyvsp[0].a_module_flags);
	}
#line 2147 "asn1p_y.c"
    break;

  case 21: /* ModuleDefinitionFlag: TOK_EXPLICIT TOK_TAGS  */
#line 564 "asn1p_y.y"
                              {
		(yyval.a_module_flags) = MSF_EXPLICIT_TAGS;
	}
#line 2155 "asn1p_y.c"
    break;

  case 22: /* ModuleDefinitionFlag: TOK_IMPLICIT TOK_TAGS  */
#line 567 "asn1p_y.y"
                                {
		(yyval.a_module_flags) = MSF_IMPLICIT_TAGS;
	}
#line 2163 "asn1p_y.c"
    break;

  case 23: /* ModuleDefinitionFlag: TOK_AUTOMATIC TOK_TAGS  */
#line 570 "asn1p_y.y"
                                 {
		(yyval.a_module_flags) = MSF_AUTOMATIC_TAGS;
	}
#line 2171 "asn1p_y.c"
    break;

  case 24: /* ModuleDefinitionFlag: TOK_EXTENSIBILITY TOK_IMPLIED  */
#line 573 "asn1p_y.y"
                                        {
		(yyval.a_module_flags) = MSF_EXTENSIBILITY_IMPLIED;
	}
#line 2179 "asn1p_y.c"
    break;

  case 25: /* ModuleDefinitionFlag: TOK_capitalreference TOK_INSTRUCTIONS  */
#line 577 "asn1p_y.y"
                                                {
		/* X.680Amd1 specifies TAG and XER */
		if(strcmp((yyvsp[-1].tv_str), "TAG") == 0) {
		 	(yyval.a_module_flags) = MSF_TAG_INSTRUCTIONS;
		} else if(strcmp((yyvsp[-1].tv_str), "XER") == 0) {
		 	(yyval.a_module_flags) = MSF_XER_INSTRUCTIONS;
		} else {
			fprintf(stderr,
				"WARNING: %s INSTRUCTIONS at %s:%d: "
				"Unrecognized encoding reference\n",
				(yyvsp[-1].tv_str), ASN_FILENAME, asn1p_get_lineno(yyscanner));
		 	(yyval.a_module_flags) = MSF_unk_INSTRUCTIONS;
		}
		free((yyvsp[-1].tv_str));
	}
#line 2199 "asn1p_y.c"
    break;

  case 26: /* optModuleBody: %empty  */
#line 598 "asn1p_y.y"
        { (yyval.a_module) = 0; }
#line 2205 "asn1p_y.c"
    break;

  case 27: /* optModuleBody: ModuleBody  */
#line 599 "asn1p_y.y"
                     {
		(yyval.a_module) = (yyvsp[0].a_module);
	}
#line 2213 "asn1p_y.c"
    break;

  case 28: /* ModuleBody: optExports optImports AssignmentList  */
#line 608 "asn1p_y.y"
                                             {
		(yyval.a_module) = asn1p_module_new();
		AL_IMPORT((yyval.a_module), exports, (yyvsp[-2].a_module), xp_next);
		AL_IMPORT((yyval.a_module), imports, (yyvsp[-1].a_module), xp_next);
		asn1p_module_move_members((yyval.a_module), (yyvsp[0].a_module));

		asn1p_module_free((yyvsp[-2].a_module));
		asn1p_module_free((yyvsp[-1].a_module));
		asn1p_module_free((yyvsp[0].a_module));
	}
#line 2228 "asn1p_y.c"
    break;

  case 29: /* AssignmentList: Assignment  */
#line 621 "asn1p_y.y"
                   {
		(yyval.a_module) = (yyvsp[0].a_module);
	}
#line 2236 "asn1p_y.c"
    break;

  case 30: /* AssignmentList: AssignmentList Assignment  */
#line 624 "asn1p_y.y"
                                    {
		if((yyvsp[-1].a_module)) {
			(yyval.a_module) = (yyvsp[-1].a_module);
		} else {
			(yyval.a_module) = (yyvsp[0].a_module);
			break;
		}
        asn1p_module_move_members((yyval.a_module), (yyvsp[0].a_module));
		asn1p_module_free((yyvsp[0].a_module));
	}
#line 2251 "asn1p_y.c"
    break;

  case 31: /* Assignment: DataTypeReference  */
#line 641 "asn1p_y.y"
                          {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		assert((yyvsp[0].a_expr)->expr_type != A1TC_INVALID);
		assert((yyvsp[0].a_expr)->meta_type != AMT_INVALID);
		asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
	}
#line 2263 "asn1p_y.c"
    break;

  case 32: /* Assignment: ValueAssignment  */
#line 648 "asn1p_y.y"
                          {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		assert((yyvsp[0].a_expr)->expr_type != A1TC_INVALID);
		assert((yyvsp[0].a_expr)->meta_type != AMT_INVALID);
		asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
	}
#line 2275 "asn1p_y.c"
    break;

  case 33: /* Assignment: ValueSetTypeAssignment  */
#line 662 "asn1p_y.y"
                                 {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		assert((yyvsp[0].a_expr)->expr_type != A1TC_INVALID);
		assert((yyvsp[0].a_expr)->meta_type != AMT_INVALID);
		asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
	}
#line 2287 "asn1p_y.c"
    break;

  case 34: /* $@2: %empty  */
#line 670 "asn1p_y.y"
                { asn1p_lexer_hack_push_encoding_control(yyscanner); }
#line 2293 "asn1p_y.c"
    break;

  case 35: /* Assignment: TOK_ENCODING_CONTROL TOK_capitalreference $@2  */
#line 671 "asn1p_y.y"
                        {
		fprintf(stderr,
			"WARNING: ENCODING-CONTROL %s "
			"specification at %s:%d ignored\n",
			(yyvsp[-1].tv_str), ASN_FILENAME, asn1p_get_lineno(yyscanner));
		free((yyvsp[-1].tv_str));
		(yyval.a_module) = 0;
	}
#line 2306 "asn1p_y.c"
    break;

  case 36: /* Assignment: BasicString  */
#line 683 "asn1p_y.y"
                      {
		return yyerror(param, yyscanner,
			"Attempt to redefine a standard basic string type, "
			"please comment out or remove this type redefinition.");
	}
#line 2316 "asn1p_y.c"
    break;

  case 37: /* optImports: %empty  */
#line 696 "asn1p_y.y"
        { (yyval.a_module) = 0; }
#line 2322 "asn1p_y.c"
    break;

  case 39: /* ImportsDefinition: TOK_IMPORTS optImportsBundleSet ';'  */
#line 700 "asn1p_y.y"
                                            {
		if(!saved_aid && 0)
			return yyerror(param, yyscanner, "Unterminated IMPORTS FROM, "
					"expected semicolon ';'");
		saved_aid = 0;
		(yyval.a_module) = (yyvsp[-1].a_module);
	}
#line 2334 "asn1p_y.c"
    break;

  case 40: /* ImportsDefinition: TOK_IMPORTS TOK_FROM  */
#line 710 "asn1p_y.y"
                                         {
		return yyerror(param, yyscanner, "Empty IMPORTS list");
	}
#line 2342 "asn1p_y.c"
    break;

  case 41: /* optImportsBundleSet: %empty  */
#line 716 "asn1p_y.y"
        { (yyval.a_module) = asn1p_module_new(); }
#line 2348 "asn1p_y.c"
    break;

  case 43: /* ImportsBundleSet: ImportsBundle  */
#line 720 "asn1p_y.y"
                      {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		TQ_ADD(&((yyval.a_module)->imports), (yyvsp[0].a_xports), xp_next);
	}
#line 2358 "asn1p_y.c"
    break;

  case 44: /* ImportsBundleSet: ImportsBundleSet ImportsBundle  */
#line 725 "asn1p_y.y"
                                         {
		(yyval.a_module) = (yyvsp[-1].a_module);
		TQ_ADD(&((yyval.a_module)->imports), (yyvsp[0].a_xports), xp_next);
	}
#line 2367 "asn1p_y.c"
    break;

  case 45: /* AssignedIdentifier: %empty  */
#line 732 "asn1p_y.y"
        { memset(&(yyval.a_aid), 0, sizeof((yyval.a_aid))); }
#line 2373 "asn1p_y.c"
    break;

  case 46: /* AssignedIdentifier: ObjectIdentifier  */
#line 733 "asn1p_y.y"
                           { (yyval.a_aid).oid = (yyvsp[0].a_oid); }
#line 2379 "asn1p_y.c"
    break;

  case 47: /* ImportsBundle: ImportsList TOK_FROM TypeRefName AssignedIdentifier  */
#line 737 "asn1p_y.y"
                                                            {
		(yyval.a_xports) = (yyvsp[-3].a_xports);
		(yyval.a_xports)->fromModuleName = (yyvsp[-1].tv_str);
		(yyval.a_xports)->identifier = (yyvsp[0].a_aid);
		/* This stupid thing is used for look-back hack. */
		saved_aid = (yyval.a_xports)->identifier.oid ? 0 : &((yyval.a_xports)->identifier);
		checkmem((yyval.a_xports));
	}
#line 2392 "asn1p_y.c"
    break;

  case 48: /* ImportsList: ImportsElement  */
#line 748 "asn1p_y.y"
                       {
		(yyval.a_xports) = asn1p_xports_new();
		checkmem((yyval.a_xports));
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 2402 "asn1p_y.c"
    break;

  case 49: /* ImportsList: ImportsList ',' ImportsElement  */
#line 753 "asn1p_y.y"
                                         {
		(yyval.a_xports) = (yyvsp[-2].a_xports);
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 2411 "asn1p_y.c"
    break;

  case 50: /* ImportsElement: TypeRefName  */
#line 760 "asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
	}
#line 2422 "asn1p_y.c"
    break;

  case 51: /* ImportsElement: TypeRefName '{' '}'  */
#line 766 "asn1p_y.y"
                              {		/* Completely equivalent to above */
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
	}
#line 2433 "asn1p_y.c"
    break;

  case 52: /* ImportsElement: Identifier  */
#line 772 "asn1p_y.y"
                     {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
	}
#line 2444 "asn1p_y.c"
    break;

  case 53: /* optExports: %empty  */
#line 782 "asn1p_y.y"
        { (yyval.a_module) = 0; }
#line 2450 "asn1p_y.c"
    break;

  case 54: /* optExports: ExportsDefinition  */
#line 783 "asn1p_y.y"
                            {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		if((yyvsp[0].a_xports)) {
			TQ_ADD(&((yyval.a_module)->exports), (yyvsp[0].a_xports), xp_next);
		} else {
			/* "EXPORTS ALL;" */
		}
	}
#line 2464 "asn1p_y.c"
    break;

  case 55: /* ExportsDefinition: TOK_EXPORTS ExportsBody ';'  */
#line 795 "asn1p_y.y"
                                    {
		(yyval.a_xports) = (yyvsp[-1].a_xports);
	}
#line 2472 "asn1p_y.c"
    break;

  case 56: /* ExportsDefinition: TOK_EXPORTS TOK_ALL ';'  */
#line 798 "asn1p_y.y"
                                  {
		(yyval.a_xports) = 0;
	}
#line 2480 "asn1p_y.c"
    break;

  case 57: /* ExportsDefinition: TOK_EXPORTS ';'  */
#line 801 "asn1p_y.y"
                          {
		/* Empty EXPORTS clause effectively prohibits export. */
		(yyval.a_xports) = asn1p_xports_new();
		checkmem((yyval.a_xports));
	}
#line 2490 "asn1p_y.c"
    break;

  case 58: /* ExportsBody: ExportsElement  */
#line 809 "asn1p_y.y"
                       {
		(yyval.a_xports) = asn1p_xports_new();
		assert((yyval.a_xports));
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 2500 "asn1p_y.c"
    break;

  case 59: /* ExportsBody: ExportsBody ',' ExportsElement  */
#line 814 "asn1p_y.y"
                                         {
		(yyval.a_xports) = (yyvsp[-2].a_xports);
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 2509 "asn1p_y.c"
    break;

  case 60: /* ExportsElement: TypeRefName  */
#line 821 "asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_EXPORTVAR;
	}
#line 2520 "asn1p_y.c"
    break;

  case 61: /* ExportsElement: TypeRefName '{' '}'  */
#line 827 "asn1p_y.y"
                              {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->expr_type = A1TC_EXPORTVAR;
	}
#line 2531 "asn1p_y.c"
    break;

  case 62: /* ExportsElement: Identifier  */
#line 833 "asn1p_y.y"
                     {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_EXPORTVAR;
	}
#line 2542 "asn1p_y.c"
    break;

  case 63: /* ValueSet: '{' ElementSetSpecs '}'  */
#line 842 "asn1p_y.y"
                                  { (yyval.a_constr) = (yyvsp[-1].a_constr); }
#line 2548 "asn1p_y.c"
    break;

  case 64: /* ValueSetTypeAssignment: TypeRefName Type TOK_PPEQ ValueSet  */
#line 845 "asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		assert((yyval.a_expr)->Identifier == 0);
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->meta_type = AMT_VALUESET;
		(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
	}
#line 2560 "asn1p_y.c"
    break;

  case 65: /* DefinedType: ComplexTypeReference  */
#line 865 "asn1p_y.y"
                             {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = (yyvsp[0].a_ref);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_TYPEREF;
	}
#line 2572 "asn1p_y.c"
    break;

  case 66: /* DefinedType: ComplexTypeReference '{' ActualParameterList '}'  */
#line 875 "asn1p_y.y"
                                                           {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = (yyvsp[-3].a_ref);
		(yyval.a_expr)->rhs_pspecs = (yyvsp[-1].a_expr);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_TYPEREF;
	}
#line 2585 "asn1p_y.c"
    break;

  case 67: /* DataTypeReference: TypeRefName TOK_PPEQ Type  */
#line 895 "asn1p_y.y"
                                  {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		assert((yyval.a_expr)->expr_type);
		assert((yyval.a_expr)->meta_type);
	}
#line 2596 "asn1p_y.c"
    break;

  case 68: /* DataTypeReference: TypeRefName TOK_PPEQ ObjectClass  */
#line 901 "asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		assert((yyval.a_expr)->expr_type == A1TC_CLASSDEF);
		assert((yyval.a_expr)->meta_type == AMT_OBJECTCLASS);
	}
#line 2607 "asn1p_y.c"
    break;

  case 69: /* DataTypeReference: TypeRefName '{' ParameterArgumentList '}' TOK_PPEQ Type  */
#line 917 "asn1p_y.y"
                                                                  {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-5].tv_str);
		(yyval.a_expr)->lhs_params = (yyvsp[-3].a_plist);
	}
#line 2617 "asn1p_y.c"
    break;

  case 70: /* DataTypeReference: TypeRefName '{' ParameterArgumentList '}' TOK_PPEQ ObjectClass  */
#line 923 "asn1p_y.y"
                                                                         {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-5].tv_str);
		(yyval.a_expr)->lhs_params = (yyvsp[-3].a_plist);
	}
#line 2627 "asn1p_y.c"
    break;

  case 71: /* ParameterArgumentList: ParameterArgumentName  */
#line 931 "asn1p_y.y"
                              {
		int ret;
		(yyval.a_plist) = asn1p_paramlist_new(asn1p_get_lineno(yyscanner));
		checkmem((yyval.a_plist));
		ret = asn1p_paramlist_add_param((yyval.a_plist), (yyvsp[0].a_parg).governor, (yyvsp[0].a_parg).argument);
		checkmem(ret == 0);
		asn1p_ref_free((yyvsp[0].a_parg).governor);
		free((yyvsp[0].a_parg).argument);
	}
#line 2641 "asn1p_y.c"
    break;

  case 72: /* ParameterArgumentList: ParameterArgumentList ',' ParameterArgumentName  */
#line 940 "asn1p_y.y"
                                                          {
		int ret;
		(yyval.a_plist) = (yyvsp[-2].a_plist);
		ret = asn1p_paramlist_add_param((yyval.a_plist), (yyvsp[0].a_parg).governor, (yyvsp[0].a_parg).argument);
		checkmem(ret == 0);
		asn1p_ref_free((yyvsp[0].a_parg).governor);
		free((yyvsp[0].a_parg).argument);
	}
#line 2654 "asn1p_y.c"
    break;

  case 73: /* ParameterArgumentName: TypeRefName  */
#line 951 "asn1p_y.y"
                    {
		(yyval.a_parg).governor = NULL;
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
	}
#line 2663 "asn1p_y.c"
    break;

  case 74: /* ParameterArgumentName: TypeRefName ':' Identifier  */
#line 955 "asn1p_y.y"
                                     {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor, (yyvsp[-2].tv_str), 0);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
		free((yyvsp[-2].tv_str));
	}
#line 2676 "asn1p_y.c"
    break;

  case 75: /* ParameterArgumentName: TypeRefName ':' TypeRefName  */
#line 963 "asn1p_y.y"
                                      {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor, (yyvsp[-2].tv_str), 0);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
		free((yyvsp[-2].tv_str));
	}
#line 2689 "asn1p_y.c"
    break;

  case 76: /* ParameterArgumentName: BasicTypeId ':' Identifier  */
#line 971 "asn1p_y.y"
                                     {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor,
			ASN_EXPR_TYPE2STR((yyvsp[-2].a_type)), 1);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
	}
#line 2702 "asn1p_y.c"
    break;

  case 77: /* ParameterArgumentName: BasicTypeId ':' TypeRefName  */
#line 979 "asn1p_y.y"
                                      {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor,
			ASN_EXPR_TYPE2STR((yyvsp[-2].a_type)), 1);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
	}
#line 2715 "asn1p_y.c"
    break;

  case 78: /* ActualParameterList: ActualParameter  */
#line 990 "asn1p_y.y"
                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2725 "asn1p_y.c"
    break;

  case 79: /* ActualParameterList: ActualParameterList ',' ActualParameter  */
#line 995 "asn1p_y.y"
                                                  {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2734 "asn1p_y.c"
    break;

  case 81: /* ActualParameter: SimpleValue  */
#line 1003 "asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("?");
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 2747 "asn1p_y.c"
    break;

  case 82: /* ActualParameter: DefinedValue  */
#line 1011 "asn1p_y.y"
                       {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("?");
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 2760 "asn1p_y.c"
    break;

  case 83: /* ActualParameter: ValueSet  */
#line 1019 "asn1p_y.y"
                   {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->expr_type = A1TC_VALUESET;
		(yyval.a_expr)->meta_type = AMT_VALUESET;
		(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
	}
#line 2771 "asn1p_y.c"
    break;

  case 84: /* optComponentTypeLists: %empty  */
#line 1042 "asn1p_y.y"
        { (yyval.a_expr) = NEW_EXPR(); }
#line 2777 "asn1p_y.c"
    break;

  case 85: /* optComponentTypeLists: ComponentTypeLists  */
#line 1043 "asn1p_y.y"
                             { (yyval.a_expr) = (yyvsp[0].a_expr); }
#line 2783 "asn1p_y.c"
    break;

  case 86: /* ComponentTypeLists: ComponentType  */
#line 1046 "asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2793 "asn1p_y.c"
    break;

  case 87: /* ComponentTypeLists: ComponentTypeLists ',' ComponentType  */
#line 1051 "asn1p_y.y"
                                               {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2802 "asn1p_y.c"
    break;

  case 88: /* ComponentTypeLists: ComponentTypeLists ',' TOK_VBracketLeft ComponentTypeLists TOK_VBracketRight  */
#line 1055 "asn1p_y.y"
                                                                                       {
		(yyval.a_expr) = (yyvsp[-4].a_expr);
		asn1p_expr_add_many((yyval.a_expr), (yyvsp[-1].a_expr));
		asn1p_expr_free((yyvsp[-1].a_expr));
	}
#line 2812 "asn1p_y.c"
    break;

  case 89: /* ComponentType: Identifier MaybeIndirectTaggedType optMarker  */
#line 1063 "asn1p_y.y"
                                                     {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->Identifier == 0);
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyvsp[0].a_marker).flags |= (yyval.a_expr)->marker.flags;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 2824 "asn1p_y.c"
    break;

  case 90: /* ComponentType: MaybeIndirectTaggedType optMarker  */
#line 1070 "asn1p_y.y"
                                            {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		(yyvsp[0].a_marker).flags |= (yyval.a_expr)->marker.flags;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
		_fixup_anonymous_identifier((yyval.a_expr), yyscanner);
	}
#line 2835 "asn1p_y.c"
    break;

  case 91: /* ComponentType: TOK_COMPONENTS TOK_OF MaybeIndirectTaggedType  */
#line 1076 "asn1p_y.y"
                                                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->meta_type = (yyvsp[0].a_expr)->meta_type;
		(yyval.a_expr)->expr_type = A1TC_COMPONENTS_OF;
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2847 "asn1p_y.c"
    break;

  case 92: /* ComponentType: ExtensionAndException  */
#line 1083 "asn1p_y.y"
                                {
		(yyval.a_expr) = (yyvsp[0].a_expr);
	}
#line 2855 "asn1p_y.c"
    break;

  case 93: /* AlternativeTypeLists: AlternativeType  */
#line 1089 "asn1p_y.y"
                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2865 "asn1p_y.c"
    break;

  case 94: /* AlternativeTypeLists: AlternativeTypeLists ',' AlternativeType  */
#line 1094 "asn1p_y.y"
                                                   {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2874 "asn1p_y.c"
    break;

  case 95: /* AlternativeType: Identifier MaybeIndirectTaggedType  */
#line 1101 "asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		assert((yyval.a_expr)->Identifier == 0);
		(yyval.a_expr)->Identifier = (yyvsp[-1].tv_str);
	}
#line 2884 "asn1p_y.c"
    break;

  case 96: /* AlternativeType: ExtensionAndException  */
#line 1106 "asn1p_y.y"
                                {
		(yyval.a_expr) = (yyvsp[0].a_expr);
	}
#line 2892 "asn1p_y.c"
    break;

  case 97: /* AlternativeType: MaybeIndirectTaggedType  */
#line 1109 "asn1p_y.y"
                                  {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		_fixup_anonymous_identifier((yyval.a_expr), yyscanner);
	}
#line 2901 "asn1p_y.c"
    break;

  case 98: /* ObjectClass: TOK_CLASS '{' FieldSpec '}' optWithSyntax  */
#line 1116 "asn1p_y.y"
                                                  {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		checkmem((yyval.a_expr));
		(yyval.a_expr)->with_syntax = (yyvsp[0].a_wsynt);
		assert((yyval.a_expr)->expr_type == A1TC_CLASSDEF);
		assert((yyval.a_expr)->meta_type == AMT_OBJECTCLASS);
	}
#line 2913 "asn1p_y.c"
    break;

  case 99: /* optUNIQUE: %empty  */
#line 1126 "asn1p_y.y"
        { (yyval.a_int) = 0; }
#line 2919 "asn1p_y.c"
    break;

  case 100: /* optUNIQUE: TOK_UNIQUE  */
#line 1127 "asn1p_y.y"
                     { (yyval.a_int) = 1; }
#line 2925 "asn1p_y.c"
    break;

  case 101: /* FieldSpec: ClassField  */
#line 1131 "asn1p_y.y"
                   {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_CLASSDEF;
		(yyval.a_expr)->meta_type = AMT_OBJECTCLASS;
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2937 "asn1p_y.c"
    break;

  case 102: /* FieldSpec: FieldSpec ',' ClassField  */
#line 1138 "asn1p_y.y"
                                   {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 2946 "asn1p_y.c"
    break;

  case 103: /* ClassField: TOK_typefieldreference optMarker  */
#line 1148 "asn1p_y.y"
                                         {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-1].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_TFS;	/* TypeFieldSpec */
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 2959 "asn1p_y.c"
    break;

  case 104: /* ClassField: TOK_valuefieldreference Type optUNIQUE optMarker  */
#line 1158 "asn1p_y.y"
                                                           {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_FTVFS;	/* FixedTypeValueFieldSpec */
		(yyval.a_expr)->unique = (yyvsp[-1].a_int);
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
		asn1p_expr_add((yyval.a_expr), (yyvsp[-2].a_expr));
	}
#line 2973 "asn1p_y.c"
    break;

  case 105: /* ClassField: TOK_valuefieldreference FieldName optMarker  */
#line 1169 "asn1p_y.y"
                                                      {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_VTVFS;
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 2986 "asn1p_y.c"
    break;

  case 106: /* ClassField: TOK_valuefieldreference DefinedObjectClass optMarker  */
#line 1179 "asn1p_y.y"
                                                               {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_OFS;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 3000 "asn1p_y.c"
    break;

  case 107: /* ClassField: TOK_typefieldreference FieldName optMarker  */
#line 1190 "asn1p_y.y"
                                                     {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_VTVSFS;
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 3013 "asn1p_y.c"
    break;

  case 108: /* ClassField: TOK_typefieldreference Type optMarker  */
#line 1200 "asn1p_y.y"
                                                {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_FTVSFS;
		asn1p_expr_add((yyval.a_expr), (yyvsp[-1].a_expr));
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 3027 "asn1p_y.c"
    break;

  case 109: /* ClassField: TOK_typefieldreference DefinedObjectClass optMarker  */
#line 1211 "asn1p_y.y"
                                                              {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_OSFS;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 3041 "asn1p_y.c"
    break;

  case 110: /* optWithSyntax: %empty  */
#line 1223 "asn1p_y.y"
        { (yyval.a_wsynt) = 0; }
#line 3047 "asn1p_y.c"
    break;

  case 111: /* optWithSyntax: WithSyntax  */
#line 1224 "asn1p_y.y"
                     {
		(yyval.a_wsynt) = (yyvsp[0].a_wsynt);
	}
#line 3055 "asn1p_y.c"
    break;

  case 112: /* $@3: %empty  */
#line 1231 "asn1p_y.y"
                { asn1p_lexer_hack_enable_with_syntax(yyscanner); }
#line 3061 "asn1p_y.c"
    break;

  case 113: /* WithSyntax: TOK_WITH TOK_SYNTAX '{' $@3 WithSyntaxList '}'  */
#line 1233 "asn1p_y.y"
                    {
		(yyval.a_wsynt) = (yyvsp[-1].a_wsynt);
	}
#line 3069 "asn1p_y.c"
    break;

  case 114: /* WithSyntaxList: WithSyntaxToken  */
#line 1239 "asn1p_y.y"
                        {
		(yyval.a_wsynt) = asn1p_wsyntx_new();
		TQ_ADD(&((yyval.a_wsynt)->chunks), (yyvsp[0].a_wchunk), next);
	}
#line 3078 "asn1p_y.c"
    break;

  case 115: /* WithSyntaxList: WithSyntaxList WithSyntaxToken  */
#line 1243 "asn1p_y.y"
                                         {
		(yyval.a_wsynt) = (yyvsp[-1].a_wsynt);
		TQ_ADD(&((yyval.a_wsynt)->chunks), (yyvsp[0].a_wchunk), next);
	}
#line 3087 "asn1p_y.c"
    break;

  case 116: /* WithSyntaxToken: TOK_whitespace  */
#line 1250 "asn1p_y.y"
                       {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromstring((yyvsp[0].tv_opaque).buf, 0);
		(yyval.a_wchunk)->type = WC_WHITESPACE;
	}
#line 3096 "asn1p_y.c"
    break;

  case 117: /* WithSyntaxToken: TOK_Literal  */
#line 1254 "asn1p_y.y"
                      {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromstring((yyvsp[0].tv_str), 0);
	}
#line 3104 "asn1p_y.c"
    break;

  case 118: /* WithSyntaxToken: PrimitiveFieldReference  */
#line 1257 "asn1p_y.y"
                                  {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromstring((yyvsp[0].a_refcomp).name, 0);
		(yyval.a_wchunk)->type = WC_FIELD;
	}
#line 3113 "asn1p_y.c"
    break;

  case 119: /* WithSyntaxToken: '[' WithSyntaxList ']'  */
#line 1261 "asn1p_y.y"
                                 {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromsyntax((yyvsp[-1].a_wsynt));
	}
#line 3121 "asn1p_y.c"
    break;

  case 120: /* ExtensionAndException: "..."  */
#line 1267 "asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3134 "asn1p_y.c"
    break;

  case 121: /* ExtensionAndException: "..." '!' DefinedValue  */
#line 1275 "asn1p_y.y"
                                         {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->value = (yyvsp[0].a_value);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3148 "asn1p_y.c"
    break;

  case 122: /* ExtensionAndException: "..." '!' SignedNumber  */
#line 1284 "asn1p_y.y"
                                         {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		(yyval.a_expr)->value = (yyvsp[0].a_value);
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3162 "asn1p_y.c"
    break;

  case 124: /* TaggedType: optTag UntaggedType  */
#line 1298 "asn1p_y.y"
                        {
        (yyval.a_expr) = (yyvsp[0].a_expr);
        (yyval.a_expr)->tag = (yyvsp[-1].a_tag);
    }
#line 3171 "asn1p_y.c"
    break;

  case 125: /* DefinedUntaggedType: DefinedType optManyConstraints  */
#line 1305 "asn1p_y.y"
                                       {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		/*
		 * Outer constraint for SEQUENCE OF and SET OF applies
		 * to the inner type.
		 */
		if((yyval.a_expr)->expr_type == ASN_CONSTR_SEQUENCE_OF
		|| (yyval.a_expr)->expr_type == ASN_CONSTR_SET_OF) {
			assert(!TQ_FIRST(&((yyval.a_expr)->members))->constraints);
			TQ_FIRST(&((yyval.a_expr)->members))->constraints = (yyvsp[0].a_constr);
		} else {
			if((yyval.a_expr)->constraints) {
				assert(!(yyvsp[0].a_constr));
				/* Check this : optManyConstraints is not used ?! */
				asn1p_constraint_free((yyvsp[0].a_constr));
			} else {
				(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
			}
		}
	}
#line 3196 "asn1p_y.c"
    break;

  case 126: /* UntaggedType: TypeDeclaration optManyConstraints  */
#line 1328 "asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		/*
		 * Outer constraint for SEQUENCE OF and SET OF applies
		 * to the inner type.
		 */
		if((yyval.a_expr)->expr_type == ASN_CONSTR_SEQUENCE_OF
		|| (yyval.a_expr)->expr_type == ASN_CONSTR_SET_OF) {
			assert(!TQ_FIRST(&((yyval.a_expr)->members))->constraints);
			TQ_FIRST(&((yyval.a_expr)->members))->constraints = (yyvsp[0].a_constr);
		} else {
			if((yyval.a_expr)->constraints) {
				assert(!(yyvsp[0].a_constr));
				/* Check this : optManyConstraints is not used ?! */
				asn1p_constraint_free((yyvsp[0].a_constr));
			} else {
				(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
			}
		}
	}
#line 3221 "asn1p_y.c"
    break;

  case 127: /* MaybeIndirectTaggedType: optTag MaybeIndirectTypeDeclaration optManyConstraints  */
#line 1351 "asn1p_y.y"
                                                           {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		(yyval.a_expr)->tag = (yyvsp[-2].a_tag);
		/*
		 * Outer constraint for SEQUENCE OF and SET OF applies
		 * to the inner type.
		 */
		if((yyval.a_expr)->expr_type == ASN_CONSTR_SEQUENCE_OF
		|| (yyval.a_expr)->expr_type == ASN_CONSTR_SET_OF) {
			assert(!TQ_FIRST(&((yyval.a_expr)->members))->constraints);
			TQ_FIRST(&((yyval.a_expr)->members))->constraints = (yyvsp[0].a_constr);
		} else {
			if((yyval.a_expr)->constraints) {
				assert(!(yyvsp[-1].a_expr));
				/* Check this : optManyConstraints is not used ?! */
				asn1p_constraint_free((yyvsp[0].a_constr));
			} else {
				(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
			}
		}
	}
#line 3247 "asn1p_y.c"
    break;

  case 128: /* NSTD_IndirectMarker: %empty  */
#line 1375 "asn1p_y.y"
        {
		(yyval.a_int) = asn1p_as_pointer ? EM_INDIRECT : 0;
		asn1p_as_pointer = 0;
	}
#line 3256 "asn1p_y.c"
    break;

  case 129: /* MaybeIndirectTypeDeclaration: NSTD_IndirectMarker TypeDeclaration  */
#line 1382 "asn1p_y.y"
                                        {
        (yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->marker.flags |= (yyvsp[-1].a_int);

		if(((yyval.a_expr)->marker.flags & EM_INDIRECT)
		&& ((yyval.a_expr)->marker.flags & EM_OPTIONAL) != EM_OPTIONAL) {
			fprintf(stderr,
				"INFO: Directive <ASN1C:RepresentAsPointer> "
				"applied to %s at %s:%d\n",
				ASN_EXPR_TYPE2STR((yyval.a_expr)->expr_type)
					?  ASN_EXPR_TYPE2STR((yyval.a_expr)->expr_type)
					: "member",
				ASN_FILENAME, (yyval.a_expr)->_lineno
			);
		}
    }
#line 3277 "asn1p_y.c"
    break;

  case 133: /* ConcreteTypeDeclaration: TOK_CHOICE '{' AlternativeTypeLists '}'  */
#line 1406 "asn1p_y.y"
                                                  {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->expr_type == A1TC_INVALID);
		(yyval.a_expr)->expr_type = ASN_CONSTR_CHOICE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3288 "asn1p_y.c"
    break;

  case 134: /* ConcreteTypeDeclaration: TOK_SEQUENCE '{' optComponentTypeLists '}'  */
#line 1412 "asn1p_y.y"
                                                     {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->expr_type == A1TC_INVALID);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SEQUENCE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3299 "asn1p_y.c"
    break;

  case 135: /* ConcreteTypeDeclaration: TOK_SET '{' optComponentTypeLists '}'  */
#line 1418 "asn1p_y.y"
                                                {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->expr_type == A1TC_INVALID);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SET;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3310 "asn1p_y.c"
    break;

  case 136: /* ConcreteTypeDeclaration: TOK_SEQUENCE optSizeOrConstraint TOK_OF optIdentifier optTag MaybeIndirectTypeDeclaration  */
#line 1424 "asn1p_y.y"
                                                                                                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->constraints = (yyvsp[-4].a_constr);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SEQUENCE_OF;
		(yyval.a_expr)->meta_type = AMT_TYPE;
		(yyvsp[0].a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyvsp[0].a_expr)->tag = (yyvsp[-1].a_tag);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 3325 "asn1p_y.c"
    break;

  case 137: /* ConcreteTypeDeclaration: TOK_SET optSizeOrConstraint TOK_OF optIdentifier optTag MaybeIndirectTypeDeclaration  */
#line 1434 "asn1p_y.y"
                                                                                               {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->constraints = (yyvsp[-4].a_constr);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SET_OF;
		(yyval.a_expr)->meta_type = AMT_TYPE;
		(yyvsp[0].a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyvsp[0].a_expr)->tag = (yyvsp[-1].a_tag);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 3340 "asn1p_y.c"
    break;

  case 138: /* ConcreteTypeDeclaration: TOK_ANY  */
#line 1444 "asn1p_y.y"
                                                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = ASN_TYPE_ANY;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3351 "asn1p_y.c"
    break;

  case 139: /* ConcreteTypeDeclaration: TOK_ANY TOK_DEFINED TOK_BY Identifier  */
#line 1450 "asn1p_y.y"
                                                        {
		int ret;
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		ret = asn1p_ref_add_component((yyval.a_expr)->reference,
			(yyvsp[0].tv_str), RLT_lowercase);
		checkmem(ret == 0);
		(yyval.a_expr)->expr_type = ASN_TYPE_ANY;
		(yyval.a_expr)->meta_type = AMT_TYPE;
		free((yyvsp[0].tv_str));
	}
#line 3368 "asn1p_y.c"
    break;

  case 140: /* ConcreteTypeDeclaration: TOK_INSTANCE TOK_OF ComplexTypeReference  */
#line 1462 "asn1p_y.y"
                                                   {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = (yyvsp[0].a_ref);
		(yyval.a_expr)->expr_type = A1TC_INSTANCE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3380 "asn1p_y.c"
    break;

  case 141: /* ComplexTypeReference: TOK_typereference  */
#line 1477 "asn1p_y.y"
                          {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		free((yyvsp[0].tv_str));
	}
#line 3393 "asn1p_y.c"
    break;

  case 142: /* ComplexTypeReference: TOK_capitalreference  */
#line 1485 "asn1p_y.y"
                               {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_CAPITALS);
		free((yyvsp[0].tv_str));
		checkmem(ret == 0);
	}
#line 3406 "asn1p_y.c"
    break;

  case 143: /* ComplexTypeReference: TOK_typereference '.' TypeRefName  */
#line 1493 "asn1p_y.y"
                                            {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[-2].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 3422 "asn1p_y.c"
    break;

  case 144: /* ComplexTypeReference: TOK_capitalreference '.' TypeRefName  */
#line 1504 "asn1p_y.y"
                                               {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[-2].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 3438 "asn1p_y.c"
    break;

  case 145: /* ComplexTypeReference: TOK_capitalreference '.' ComplexTypeReferenceAmpList  */
#line 1515 "asn1p_y.y"
                                                               {
		int ret;
		(yyval.a_ref) = (yyvsp[0].a_ref);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[-2].tv_str), RLT_CAPITALS);
		free((yyvsp[-2].tv_str));
		checkmem(ret == 0);
		/*
		 * Move the last element infront.
		 */
		{
			struct asn1p_ref_component_s tmp_comp;
			tmp_comp = (yyval.a_ref)->components[(yyval.a_ref)->comp_count-1];
			memmove(&(yyval.a_ref)->components[1],
				&(yyval.a_ref)->components[0],
				sizeof((yyval.a_ref)->components[0])
				* ((yyval.a_ref)->comp_count - 1));
			(yyval.a_ref)->components[0] = tmp_comp;
		}
	}
#line 3462 "asn1p_y.c"
    break;

  case 146: /* ComplexTypeReferenceAmpList: ComplexTypeReferenceElement  */
#line 1537 "asn1p_y.y"
                                    {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].a_refcomp).name, (yyvsp[0].a_refcomp).lex_type);
		free((yyvsp[0].a_refcomp).name);
		checkmem(ret == 0);
	}
#line 3475 "asn1p_y.c"
    break;

  case 147: /* ComplexTypeReferenceAmpList: ComplexTypeReferenceAmpList '.' ComplexTypeReferenceElement  */
#line 1545 "asn1p_y.y"
                                                                      {
		int ret;
		(yyval.a_ref) = (yyvsp[-2].a_ref);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].a_refcomp).name, (yyvsp[0].a_refcomp).lex_type);
		free((yyvsp[0].a_refcomp).name);
		checkmem(ret == 0);
	}
#line 3487 "asn1p_y.c"
    break;

  case 149: /* PrimitiveFieldReference: TOK_typefieldreference  */
#line 1558 "asn1p_y.y"
                               {
		(yyval.a_refcomp).lex_type = RLT_AmpUppercase;
		(yyval.a_refcomp).name = (yyvsp[0].tv_str);
	}
#line 3496 "asn1p_y.c"
    break;

  case 150: /* PrimitiveFieldReference: TOK_valuefieldreference  */
#line 1563 "asn1p_y.y"
                                  {
		(yyval.a_refcomp).lex_type = RLT_Amplowercase;
		(yyval.a_refcomp).name = (yyvsp[0].tv_str);
	}
#line 3505 "asn1p_y.c"
    break;

  case 151: /* FieldName: TOK_typefieldreference  */
#line 1572 "asn1p_y.y"
                               {
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_AmpUppercase);
		free((yyvsp[0].tv_str));
	}
#line 3515 "asn1p_y.c"
    break;

  case 152: /* FieldName: FieldName '.' TOK_typefieldreference  */
#line 1577 "asn1p_y.y"
                                               {
		(yyval.a_ref) = (yyval.a_ref);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_AmpUppercase);
		free((yyvsp[0].tv_str));
	}
#line 3525 "asn1p_y.c"
    break;

  case 153: /* FieldName: FieldName '.' TOK_valuefieldreference  */
#line 1582 "asn1p_y.y"
                                                {
		(yyval.a_ref) = (yyval.a_ref);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_Amplowercase);
		free((yyvsp[0].tv_str));
	}
#line 3535 "asn1p_y.c"
    break;

  case 154: /* DefinedObjectClass: TOK_capitalreference  */
#line 1590 "asn1p_y.y"
                             {
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_CAPITALS);
		free((yyvsp[0].tv_str));
	}
#line 3545 "asn1p_y.c"
    break;

  case 155: /* ValueAssignment: Identifier Type TOK_PPEQ Value  */
#line 1613 "asn1p_y.y"
                                       {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		assert((yyval.a_expr)->Identifier == NULL);
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 3557 "asn1p_y.c"
    break;

  case 158: /* $@4: %empty  */
#line 1625 "asn1p_y.y"
              { asn1p_lexer_hack_push_opaque_state(yyscanner); }
#line 3563 "asn1p_y.c"
    break;

  case 159: /* Value: '{' $@4 Opaque  */
#line 1625 "asn1p_y.y"
                                                                        {
		(yyval.a_value) = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_UNPARSED;
	}
#line 3573 "asn1p_y.c"
    break;

  case 160: /* SimpleValue: TOK_NULL  */
#line 1633 "asn1p_y.y"
                 {
		(yyval.a_value) = asn1p_value_fromint(0);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_NULL;
	}
#line 3583 "asn1p_y.c"
    break;

  case 161: /* SimpleValue: TOK_FALSE  */
#line 1638 "asn1p_y.y"
                    {
		(yyval.a_value) = asn1p_value_fromint(0);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_FALSE;
	}
#line 3593 "asn1p_y.c"
    break;

  case 162: /* SimpleValue: TOK_TRUE  */
#line 1643 "asn1p_y.y"
                   {
		(yyval.a_value) = asn1p_value_fromint(1);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_TRUE;
	}
#line 3603 "asn1p_y.c"
    break;

  case 168: /* DefinedValue: TypeRefName '.' Identifier  */
#line 1656 "asn1p_y.y"
                                     {
		asn1p_ref_t *ref;
		int ret;
		ref = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem(ref);
		ret = asn1p_ref_add_component(ref, (yyvsp[-2].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		ret = asn1p_ref_add_component(ref, (yyvsp[0].tv_str), RLT_lowercase);
		checkmem(ret == 0);
		(yyval.a_value) = asn1p_value_fromref(ref, 0);
		checkmem((yyval.a_value));
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 3622 "asn1p_y.c"
    break;

  case 169: /* RestrictedCharacterStringValue: TOK_cstring  */
#line 1674 "asn1p_y.y"
                    {
		(yyval.a_value) = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
		checkmem((yyval.a_value));
	}
#line 3631 "asn1p_y.c"
    break;

  case 170: /* RestrictedCharacterStringValue: TOK_tuple  */
#line 1678 "asn1p_y.y"
                    {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_TUPLE;
	}
#line 3641 "asn1p_y.c"
    break;

  case 171: /* RestrictedCharacterStringValue: TOK_quadruple  */
#line 1683 "asn1p_y.y"
                        {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_QUADRUPLE;
	}
#line 3651 "asn1p_y.c"
    break;

  case 172: /* Opaque: OpaqueFirstToken  */
#line 1691 "asn1p_y.y"
                     {
		(yyval.tv_opaque).len = (yyvsp[0].tv_opaque).len + 1;
		(yyval.tv_opaque).buf = malloc(1 + (yyval.tv_opaque).len + 1);
		checkmem((yyval.tv_opaque).buf);
		(yyval.tv_opaque).buf[0] = '{';
		memcpy((yyval.tv_opaque).buf + 1, (yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len);
		(yyval.tv_opaque).buf[(yyval.tv_opaque).len] = '\0';
		free((yyvsp[0].tv_opaque).buf);
    }
#line 3665 "asn1p_y.c"
    break;

  case 173: /* Opaque: Opaque TOK_opaque  */
#line 1700 "asn1p_y.y"
                            {
		int newsize = (yyvsp[-1].tv_opaque).len + (yyvsp[0].tv_opaque).len;
		char *p = malloc(newsize + 1);
		checkmem(p);
		memcpy(p         , (yyvsp[-1].tv_opaque).buf, (yyvsp[-1].tv_opaque).len);
		memcpy(p + (yyvsp[-1].tv_opaque).len, (yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len);
		p[newsize] = '\0';
		free((yyvsp[-1].tv_opaque).buf);
		free((yyvsp[0].tv_opaque).buf);
		(yyval.tv_opaque).buf = p;
		(yyval.tv_opaque).len = newsize;
	}
#line 3682 "asn1p_y.c"
    break;

  case 175: /* OpaqueFirstToken: Identifier  */
#line 1716 "asn1p_y.y"
                 {
        (yyval.tv_opaque).len = strlen((yyvsp[0].tv_str));
        (yyval.tv_opaque).buf = (yyvsp[0].tv_str);
    }
#line 3691 "asn1p_y.c"
    break;

  case 176: /* BasicTypeId: TOK_BOOLEAN  */
#line 1722 "asn1p_y.y"
                    { (yyval.a_type) = ASN_BASIC_BOOLEAN; }
#line 3697 "asn1p_y.c"
    break;

  case 177: /* BasicTypeId: TOK_NULL  */
#line 1723 "asn1p_y.y"
                   { (yyval.a_type) = ASN_BASIC_NULL; }
#line 3703 "asn1p_y.c"
    break;

  case 178: /* BasicTypeId: TOK_REAL  */
#line 1724 "asn1p_y.y"
                   { (yyval.a_type) = ASN_BASIC_REAL; }
#line 3709 "asn1p_y.c"
    break;

  case 179: /* BasicTypeId: TOK_OCTET TOK_STRING  */
#line 1725 "asn1p_y.y"
                               { (yyval.a_type) = ASN_BASIC_OCTET_STRING; }
#line 3715 "asn1p_y.c"
    break;

  case 180: /* BasicTypeId: TOK_OBJECT TOK_IDENTIFIER  */
#line 1726 "asn1p_y.y"
                                    { (yyval.a_type) = ASN_BASIC_OBJECT_IDENTIFIER; }
#line 3721 "asn1p_y.c"
    break;

  case 181: /* BasicTypeId: TOK_RELATIVE_OID  */
#line 1727 "asn1p_y.y"
                           { (yyval.a_type) = ASN_BASIC_RELATIVE_OID; }
#line 3727 "asn1p_y.c"
    break;

  case 182: /* BasicTypeId: TOK_EXTERNAL  */
#line 1728 "asn1p_y.y"
                       { (yyval.a_type) = ASN_BASIC_EXTERNAL; }
#line 3733 "asn1p_y.c"
    break;

  case 183: /* BasicTypeId: TOK_EMBEDDED TOK_PDV  */
#line 1729 "asn1p_y.y"
                               { (yyval.a_type) = ASN_BASIC_EMBEDDED_PDV; }
#line 3739 "asn1p_y.c"
    break;

  case 184: /* BasicTypeId: TOK_CHARACTER TOK_STRING  */
#line 1730 "asn1p_y.y"
                                   { (yyval.a_type) = ASN_BASIC_CHARACTER_STRING; }
#line 3745 "asn1p_y.c"
    break;

  case 185: /* BasicTypeId: TOK_UTCTime  */
#line 1731 "asn1p_y.y"
                      { (yyval.a_type) = ASN_BASIC_UTCTime; }
#line 3751 "asn1p_y.c"
    break;

  case 186: /* BasicTypeId: TOK_GeneralizedTime  */
#line 1732 "asn1p_y.y"
                              { (yyval.a_type) = ASN_BASIC_GeneralizedTime; }
#line 3757 "asn1p_y.c"
    break;

  case 189: /* BasicTypeId_UniverationCompatible: TOK_INTEGER  */
#line 1741 "asn1p_y.y"
                    { (yyval.a_type) = ASN_BASIC_INTEGER; }
#line 3763 "asn1p_y.c"
    break;

  case 190: /* BasicTypeId_UniverationCompatible: TOK_ENUMERATED  */
#line 1742 "asn1p_y.y"
                         { (yyval.a_type) = ASN_BASIC_ENUMERATED; }
#line 3769 "asn1p_y.c"
    break;

  case 191: /* BasicTypeId_UniverationCompatible: TOK_BIT TOK_STRING  */
#line 1743 "asn1p_y.y"
                             { (yyval.a_type) = ASN_BASIC_BIT_STRING; }
#line 3775 "asn1p_y.c"
    break;

  case 192: /* BuiltinType: BasicTypeId  */
#line 1747 "asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = (yyvsp[0].a_type);
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 3786 "asn1p_y.c"
    break;

  case 193: /* BuiltinType: TOK_INTEGER '{' NamedNumberList '}'  */
#line 1753 "asn1p_y.y"
                                          {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_INTEGER;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 3796 "asn1p_y.c"
    break;

  case 194: /* BuiltinType: TOK_ENUMERATED '{' Enumerations '}'  */
#line 1758 "asn1p_y.y"
                                          {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_ENUMERATED;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 3806 "asn1p_y.c"
    break;

  case 195: /* BuiltinType: TOK_BIT TOK_STRING '{' NamedBitList '}'  */
#line 1763 "asn1p_y.y"
                                              {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_BIT_STRING;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 3816 "asn1p_y.c"
    break;

  case 196: /* BuiltinType: TOK_ExtValue_BIT_STRING '{' IdentifierList '}'  */
#line 1768 "asn1p_y.y"
                                                     {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_BIT_STRING;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 3826 "asn1p_y.c"
    break;

  case 197: /* BuiltinType: TOK_ExtValue_BIT_STRING '{' '}'  */
#line 1773 "asn1p_y.y"
                                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
        (yyval.a_expr)->expr_type = ASN_BASIC_BIT_STRING;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 3837 "asn1p_y.c"
    break;

  case 198: /* BasicString: TOK_BMPString  */
#line 1782 "asn1p_y.y"
                      { (yyval.a_type) = ASN_STRING_BMPString; }
#line 3843 "asn1p_y.c"
    break;

  case 199: /* BasicString: TOK_GeneralString  */
#line 1783 "asn1p_y.y"
                            {
		(yyval.a_type) = ASN_STRING_GeneralString;
		fprintf(stderr, "WARNING: GeneralString is not fully supported\n");
	}
#line 3852 "asn1p_y.c"
    break;

  case 200: /* BasicString: TOK_GraphicString  */
#line 1787 "asn1p_y.y"
                            {
		(yyval.a_type) = ASN_STRING_GraphicString;
		fprintf(stderr, "WARNING: GraphicString is not fully supported\n");
	}
#line 3861 "asn1p_y.c"
    break;

  case 201: /* BasicString: TOK_IA5String  */
#line 1791 "asn1p_y.y"
                        { (yyval.a_type) = ASN_STRING_IA5String; }
#line 3867 "asn1p_y.c"
    break;

  case 202: /* BasicString: TOK_ISO646String  */
#line 1792 "asn1p_y.y"
                           { (yyval.a_type) = ASN_STRING_ISO646String; }
#line 3873 "asn1p_y.c"
    break;

  case 203: /* BasicString: TOK_NumericString  */
#line 1793 "asn1p_y.y"
                            { (yyval.a_type) = ASN_STRING_NumericString; }
#line 3879 "asn1p_y.c"
    break;

  case 204: /* BasicString: TOK_PrintableString  */
#line 1794 "asn1p_y.y"
                              { (yyval.a_type) = ASN_STRING_PrintableString; }
#line 3885 "asn1p_y.c"
    break;

  case 205: /* BasicString: TOK_T61String  */
#line 1795 "asn1p_y.y"
                        {
		(yyval.a_type) = ASN_STRING_T61String;
		fprintf(stderr, "WARNING: T61String is not fully supported\n");
	}
#line 3894 "asn1p_y.c"
    break;

  case 206: /* BasicString: TOK_TeletexString  */
#line 1799 "asn1p_y.y"
                            { (yyval.a_type) = ASN_STRING_TeletexString; }
#line 3900 "asn1p_y.c"
    break;

  case 207: /* BasicString: TOK_UniversalString  */
#line 1800 "asn1p_y.y"
                              { (yyval.a_type) = ASN_STRING_UniversalString; }
#line 3906 "asn1p_y.c"
    break;

  case 208: /* BasicString: TOK_UTF8String  */
#line 1801 "asn1p_y.y"
                         { (yyval.a_type) = ASN_STRING_UTF8String; }
#line 3912 "asn1p_y.c"
    break;

  case 209: /* BasicString: TOK_VideotexString  */
#line 1802 "asn1p_y.y"
                             {
		(yyval.a_type) = ASN_STRING_VideotexString;
		fprintf(stderr, "WARNING: VideotexString is not fully supported\n");
	}
#line 3921 "asn1p_y.c"
    break;

  case 210: /* BasicString: TOK_VisibleString  */
#line 1806 "asn1p_y.y"
                            { (yyval.a_type) = ASN_STRING_VisibleString; }
#line 3927 "asn1p_y.c"
    break;

  case 211: /* BasicString: TOK_ObjectDescriptor  */
#line 1807 "asn1p_y.y"
                               { (yyval.a_type) = ASN_STRING_ObjectDescriptor; }
#line 3933 "asn1p_y.c"
    break;

  case 216: /* optConstraint: %empty  */
#line 1819 "asn1p_y.y"
        { (yyval.a_constr) = 0; }
#line 3939 "asn1p_y.c"
    break;

  case 218: /* optManyConstraints: %empty  */
#line 1824 "asn1p_y.y"
        { (yyval.a_constr) = 0; }
#line 3945 "asn1p_y.c"
    break;

  case 220: /* optSizeOrConstraint: %empty  */
#line 1829 "asn1p_y.y"
        { (yyval.a_constr) = 0; }
#line 3951 "asn1p_y.c"
    break;

  case 223: /* Constraint: '(' ConstraintSpec ')'  */
#line 1835 "asn1p_y.y"
                           {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_SET, (yyvsp[-1].a_constr), 0);
    }
#line 3959 "asn1p_y.c"
    break;

  case 225: /* ManyConstraints: ManyConstraints Constraint  */
#line 1842 "asn1p_y.y"
                                     {
        if((yyvsp[0].a_constr)->type == ACT_CA_SET && (yyvsp[0].a_constr)->el_count == 1) {
            CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_SET, (yyvsp[-1].a_constr), (yyvsp[0].a_constr)->elements[0]);
        } else {
            CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_SET, (yyvsp[-1].a_constr), (yyvsp[0].a_constr));
        }
	}
#line 3971 "asn1p_y.c"
    break;

  case 229: /* ElementSetSpecs: "..."  */
#line 1856 "asn1p_y.y"
                       {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		(yyval.a_constr)->type = ACT_EL_EXT;
	}
#line 3980 "asn1p_y.c"
    break;

  case 231: /* ElementSetSpecs: ElementSetSpec ',' "..."  */
#line 1861 "asn1p_y.y"
                                      {
       asn1p_constraint_t *ct;
       ct = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
       ct->type = ACT_EL_EXT;
       CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, (yyvsp[-2].a_constr), ct);
   }
#line 3991 "asn1p_y.c"
    break;

  case 232: /* ElementSetSpecs: ElementSetSpec ',' "..." ',' ElementSetSpec  */
#line 1867 "asn1p_y.y"
                                                         {
       asn1p_constraint_t *ct;
       ct = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
       ct->type = ACT_EL_EXT;
       CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, (yyvsp[-4].a_constr), ct);
       ct = (yyval.a_constr);
       CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, ct, (yyvsp[0].a_constr));
   }
#line 4004 "asn1p_y.c"
    break;

  case 234: /* ElementSetSpec: TOK_ALL TOK_EXCEPT Elements  */
#line 1879 "asn1p_y.y"
                                      {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_AEX, (yyvsp[0].a_constr), 0);
	}
#line 4012 "asn1p_y.c"
    break;

  case 236: /* Unions: Unions UnionMark Intersections  */
#line 1886 "asn1p_y.y"
                                         {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_UNI, (yyvsp[-2].a_constr), (yyvsp[0].a_constr));
	}
#line 4020 "asn1p_y.c"
    break;

  case 238: /* Intersections: Intersections IntersectionMark IntersectionElements  */
#line 1893 "asn1p_y.y"
                                                               {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_INT, (yyvsp[-2].a_constr), (yyvsp[0].a_constr));
	}
#line 4028 "asn1p_y.c"
    break;

  case 240: /* IntersectionElements: Elements TOK_EXCEPT Elements  */
#line 1901 "asn1p_y.y"
                                       {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_EXC, (yyvsp[-2].a_constr), (yyvsp[0].a_constr));
	}
#line 4036 "asn1p_y.c"
    break;

  case 242: /* Elements: '(' ElementSetSpec ')'  */
#line 1908 "asn1p_y.y"
                             {
        int ret;
        (yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
        checkmem((yyval.a_constr));
        (yyval.a_constr)->type = ACT_CA_SET;
        ret = asn1p_constraint_insert((yyval.a_constr), (yyvsp[-1].a_constr));
        checkmem(ret == 0);
    }
#line 4049 "asn1p_y.c"
    break;

  case 243: /* SubtypeElements: SingleValue  */
#line 1919 "asn1p_y.y"
                    {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_EL_VALUE;
		(yyval.a_constr)->value = (yyvsp[0].a_value);
	}
#line 4060 "asn1p_y.c"
    break;

  case 244: /* SubtypeElements: ContainedSubtype  */
#line 1925 "asn1p_y.y"
                           {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_EL_TYPE;
		(yyval.a_constr)->containedSubtype = (yyvsp[0].a_value);
	}
#line 4071 "asn1p_y.c"
    break;

  case 250: /* PermittedAlphabet: TOK_FROM Constraint  */
#line 1941 "asn1p_y.y"
                            {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CT_FROM, (yyvsp[0].a_constr), 0);
	}
#line 4079 "asn1p_y.c"
    break;

  case 251: /* SizeConstraint: TOK_SIZE Constraint  */
#line 1946 "asn1p_y.y"
                            {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CT_SIZE, (yyvsp[0].a_constr), 0);
	}
#line 4087 "asn1p_y.c"
    break;

  case 252: /* PatternConstraint: TOK_PATTERN TOK_cstring  */
#line 1951 "asn1p_y.y"
                                {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		(yyval.a_constr)->type = ACT_CT_PATTERN;
		(yyval.a_constr)->value = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
	}
#line 4097 "asn1p_y.c"
    break;

  case 253: /* PatternConstraint: TOK_PATTERN Identifier  */
#line 1956 "asn1p_y.y"
                                 {
		asn1p_ref_t *ref;
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		(yyval.a_constr)->type = ACT_CT_PATTERN;
		ref = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		asn1p_ref_add_component(ref, (yyvsp[0].tv_str), RLT_lowercase);
		(yyval.a_constr)->value = asn1p_value_fromref(ref, 0);
		free((yyvsp[0].tv_str));
	}
#line 4111 "asn1p_y.c"
    break;

  case 254: /* ValueRange: LowerEndValue ConstraintRangeSpec UpperEndValue  */
#line 1968 "asn1p_y.y"
                                                    {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = (yyvsp[-1].a_ctype);
		(yyval.a_constr)->range_start = (yyvsp[-2].a_value);
		(yyval.a_constr)->range_stop = (yyvsp[0].a_value);
    }
#line 4123 "asn1p_y.c"
    break;

  case 256: /* LowerEndValue: TOK_MIN  */
#line 1978 "asn1p_y.y"
              {
		(yyval.a_value) = asn1p_value_fromint(-123);
		(yyval.a_value)->type = ATV_MIN;
    }
#line 4132 "asn1p_y.c"
    break;

  case 258: /* UpperEndValue: TOK_MAX  */
#line 1985 "asn1p_y.y"
              {
		(yyval.a_value) = asn1p_value_fromint(321);
		(yyval.a_value)->type = ATV_MAX;
    }
#line 4141 "asn1p_y.c"
    break;

  case 260: /* BitStringValue: TOK_bstring  */
#line 1993 "asn1p_y.y"
                    {
		(yyval.a_value) = _convert_bitstring2binary((yyvsp[0].tv_str), 'B');
		checkmem((yyval.a_value));
		free((yyvsp[0].tv_str));
	}
#line 4151 "asn1p_y.c"
    break;

  case 261: /* BitStringValue: TOK_hstring  */
#line 1998 "asn1p_y.y"
                      {
		(yyval.a_value) = _convert_bitstring2binary((yyvsp[0].tv_str), 'H');
		checkmem((yyval.a_value));
		free((yyvsp[0].tv_str));
	}
#line 4161 "asn1p_y.c"
    break;

  case 262: /* ContainedSubtype: TOK_INCLUDES Type  */
#line 2006 "asn1p_y.y"
                      {
		(yyval.a_value) = asn1p_value_fromtype((yyvsp[0].a_expr));
		checkmem((yyval.a_value));
		asn1p_expr_free((yyvsp[0].a_expr));
    }
#line 4171 "asn1p_y.c"
    break;

  case 263: /* ContainedSubtype: DefinedUntaggedType  */
#line 2012 "asn1p_y.y"
                          {
		(yyval.a_value) = asn1p_value_fromtype((yyvsp[0].a_expr));
		checkmem((yyval.a_value));
		asn1p_expr_free((yyvsp[0].a_expr));
    }
#line 4181 "asn1p_y.c"
    break;

  case 264: /* InnerTypeConstraints: TOK_WITH TOK_COMPONENT SingleTypeConstraint  */
#line 2024 "asn1p_y.y"
                                                    {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CT_WCOMP, (yyvsp[0].a_constr), 0);
	}
#line 4189 "asn1p_y.c"
    break;

  case 265: /* InnerTypeConstraints: TOK_WITH TOK_COMPONENTS MultipleTypeConstraints  */
#line 2027 "asn1p_y.y"
                                                          {
        assert((yyvsp[0].a_constr)->type == ACT_CA_CSV);
        (yyvsp[0].a_constr)->type = ACT_CT_WCOMPS;
        (yyval.a_constr) = (yyvsp[0].a_constr);
	}
#line 4199 "asn1p_y.c"
    break;

  case 269: /* FullSpecification: '{' TypeConstraints '}'  */
#line 2035 "asn1p_y.y"
                                           { (yyval.a_constr) = (yyvsp[-1].a_constr); }
#line 4205 "asn1p_y.c"
    break;

  case 270: /* PartialSpecification: '{' "..." ',' TypeConstraints '}'  */
#line 2037 "asn1p_y.y"
                                              {
        assert((yyvsp[-1].a_constr)->type == ACT_CA_CSV);
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
        (yyval.a_constr)->type = ACT_CA_CSV;
		asn1p_constraint_t *ct = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_constr));
		ct->type = ACT_EL_EXT;
        asn1p_constraint_insert((yyval.a_constr), ct);
        for(unsigned i = 0; i < (yyvsp[-1].a_constr)->el_count; i++) {
            asn1p_constraint_insert((yyval.a_constr), (yyvsp[-1].a_constr)->elements[i]);
        }
    }
#line 4222 "asn1p_y.c"
    break;

  case 271: /* TypeConstraints: NamedConstraint  */
#line 2050 "asn1p_y.y"
                    {
        (yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
        (yyval.a_constr)->type = ACT_CA_CSV;
        asn1p_constraint_insert((yyval.a_constr), (yyvsp[0].a_constr));
    }
#line 4232 "asn1p_y.c"
    break;

  case 272: /* TypeConstraints: TypeConstraints ',' NamedConstraint  */
#line 2055 "asn1p_y.y"
                                          {
        (yyval.a_constr) = (yyvsp[-2].a_constr);
        asn1p_constraint_insert((yyval.a_constr), (yyvsp[0].a_constr));
	}
#line 4241 "asn1p_y.c"
    break;

  case 273: /* NamedConstraint: IdentifierAsValue optConstraint optPresenceConstraint  */
#line 2061 "asn1p_y.y"
                                                              {
        (yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
        checkmem((yyval.a_constr));
        (yyval.a_constr)->type = ACT_EL_VALUE;
        (yyval.a_constr)->value = (yyvsp[-2].a_value);
        if((yyvsp[-1].a_constr)) asn1p_constraint_insert((yyval.a_constr), (yyvsp[-1].a_constr));
        (yyval.a_constr)->presence = (yyvsp[0].a_pres);
    }
#line 4254 "asn1p_y.c"
    break;

  case 274: /* optPresenceConstraint: %empty  */
#line 2075 "asn1p_y.y"
        { (yyval.a_pres) = ACPRES_DEFAULT; }
#line 4260 "asn1p_y.c"
    break;

  case 275: /* optPresenceConstraint: PresenceConstraint  */
#line 2076 "asn1p_y.y"
                             { (yyval.a_pres) = (yyvsp[0].a_pres); }
#line 4266 "asn1p_y.c"
    break;

  case 276: /* PresenceConstraint: TOK_PRESENT  */
#line 2080 "asn1p_y.y"
                    {
		(yyval.a_pres) = ACPRES_PRESENT;
	}
#line 4274 "asn1p_y.c"
    break;

  case 277: /* PresenceConstraint: TOK_ABSENT  */
#line 2083 "asn1p_y.y"
                     {
		(yyval.a_pres) = ACPRES_ABSENT;
	}
#line 4282 "asn1p_y.c"
    break;

  case 278: /* PresenceConstraint: TOK_OPTIONAL  */
#line 2086 "asn1p_y.y"
                       {
		(yyval.a_pres) = ACPRES_OPTIONAL;
	}
#line 4290 "asn1p_y.c"
    break;

  case 282: /* $@5: %empty  */
#line 2101 "asn1p_y.y"
                { asn1p_lexer_hack_push_opaque_state(yyscanner); }
#line 4296 "asn1p_y.c"
    break;

  case 283: /* UserDefinedConstraint: TOK_CONSTRAINED TOK_BY '{' $@5 Opaque  */
#line 2101 "asn1p_y.y"
                                                                                    {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_CT_CTDBY;
		(yyval.a_constr)->value = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
		checkmem((yyval.a_constr)->value);
		(yyval.a_constr)->value->type = ATV_UNPARSED;
	}
#line 4309 "asn1p_y.c"
    break;

  case 284: /* ContentsConstraint: TOK_CONTAINING Type  */
#line 2112 "asn1p_y.y"
                            {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		(yyval.a_constr)->type = ACT_CT_CTNG;
		(yyval.a_constr)->value = asn1p_value_fromtype((yyvsp[0].a_expr));
		asn1p_expr_free((yyvsp[0].a_expr));
	}
#line 4320 "asn1p_y.c"
    break;

  case 285: /* ConstraintRangeSpec: ".."  */
#line 2121 "asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_RANGE; }
#line 4326 "asn1p_y.c"
    break;

  case 286: /* ConstraintRangeSpec: ".." '<'  */
#line 2122 "asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_RLRANGE; }
#line 4332 "asn1p_y.c"
    break;

  case 287: /* ConstraintRangeSpec: '<' ".."  */
#line 2123 "asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_LLRANGE; }
#line 4338 "asn1p_y.c"
    break;

  case 288: /* ConstraintRangeSpec: '<' ".." '<'  */
#line 2124 "asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_ULRANGE; }
#line 4344 "asn1p_y.c"
    break;

  case 289: /* TableConstraint: SimpleTableConstraint  */
#line 2127 "asn1p_y.y"
                              {
		(yyval.a_constr) = (yyvsp[0].a_constr);
	}
#line 4352 "asn1p_y.c"
    break;

  case 290: /* TableConstraint: ComponentRelationConstraint  */
#line 2130 "asn1p_y.y"
                                      {
		(yyval.a_constr) = (yyvsp[0].a_constr);
	}
#line 4360 "asn1p_y.c"
    break;

  case 291: /* SimpleTableConstraint: '{' TypeRefName '}'  */
#line 2139 "asn1p_y.y"
                            {
		asn1p_ref_t *ref = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		asn1p_constraint_t *ct;
		int ret;
		ret = asn1p_ref_add_component(ref, (yyvsp[-1].tv_str), 0);
		checkmem(ret == 0);
		ct = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_constr));
		ct->type = ACT_EL_VALUE;
		ct->value = asn1p_value_fromref(ref, 0);
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CRC, ct, 0);
		free((yyvsp[-1].tv_str));
	}
#line 4378 "asn1p_y.c"
    break;

  case 292: /* ComponentRelationConstraint: SimpleTableConstraint '{' AtNotationList '}'  */
#line 2155 "asn1p_y.y"
                                                     {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CRC, (yyvsp[-3].a_constr), (yyvsp[-1].a_constr));
	}
#line 4386 "asn1p_y.c"
    break;

  case 293: /* AtNotationList: AtNotationElement  */
#line 2161 "asn1p_y.y"
                          {
		(yyval.a_constr) = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_EL_VALUE;
		(yyval.a_constr)->value = asn1p_value_fromref((yyvsp[0].a_ref), 0);
	}
#line 4397 "asn1p_y.c"
    break;

  case 294: /* AtNotationList: AtNotationList ',' AtNotationElement  */
#line 2167 "asn1p_y.y"
                                               {
		asn1p_constraint_t *ct;
		ct = asn1p_constraint_new(asn1p_get_lineno(yyscanner), currentModule);
		checkmem(ct);
		ct->type = ACT_EL_VALUE;
		ct->value = asn1p_value_fromref((yyvsp[0].a_ref), 0);
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, (yyvsp[-2].a_constr), ct);
	}
#line 4410 "asn1p_y.c"
    break;

  case 295: /* AtNotationElement: '@' ComponentIdList  */
#line 2181 "asn1p_y.y"
                            {
		char *p = malloc(strlen((yyvsp[0].tv_str)) + 2);
		int ret;
		*p = '@';
		strcpy(p + 1, (yyvsp[0].tv_str));
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		ret = asn1p_ref_add_component((yyval.a_ref), p, 0);
		checkmem(ret == 0);
		free(p);
		free((yyvsp[0].tv_str));
	}
#line 4426 "asn1p_y.c"
    break;

  case 296: /* AtNotationElement: '@' '.' ComponentIdList  */
#line 2192 "asn1p_y.y"
                                  {
		char *p = malloc(strlen((yyvsp[0].tv_str)) + 3);
		int ret;
		p[0] = '@';
		p[1] = '.';
		strcpy(p + 2, (yyvsp[0].tv_str));
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		ret = asn1p_ref_add_component((yyval.a_ref), p, 0);
		checkmem(ret == 0);
		free(p);
		free((yyvsp[0].tv_str));
	}
#line 4443 "asn1p_y.c"
    break;

  case 297: /* ComponentIdList: Identifier  */
#line 2208 "asn1p_y.y"
                   {
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 4451 "asn1p_y.c"
    break;

  case 298: /* ComponentIdList: ComponentIdList '.' Identifier  */
#line 2211 "asn1p_y.y"
                                         {
		int l1 = strlen((yyvsp[-2].tv_str));
		int l3 = strlen((yyvsp[0].tv_str));
		(yyval.tv_str) = malloc(l1 + 1 + l3 + 1);
		memcpy((yyval.tv_str), (yyvsp[-2].tv_str), l1);
		(yyval.tv_str)[l1] = '.';
		memcpy((yyval.tv_str) + l1 + 1, (yyvsp[0].tv_str), l3);
		(yyval.tv_str)[l1 + 1 + l3] = '\0';
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 4467 "asn1p_y.c"
    break;

  case 299: /* optMarker: %empty  */
#line 2231 "asn1p_y.y"
        {
		(yyval.a_marker).flags = EM_NOMARK;
		(yyval.a_marker).default_value = 0;
	}
#line 4476 "asn1p_y.c"
    break;

  case 300: /* optMarker: Marker  */
#line 2235 "asn1p_y.y"
                 { (yyval.a_marker) = (yyvsp[0].a_marker); }
#line 4482 "asn1p_y.c"
    break;

  case 301: /* Marker: TOK_OPTIONAL  */
#line 2239 "asn1p_y.y"
                     {
		(yyval.a_marker).flags = EM_OPTIONAL | EM_INDIRECT;
		(yyval.a_marker).default_value = 0;
	}
#line 4491 "asn1p_y.c"
    break;

  case 302: /* Marker: TOK_DEFAULT Value  */
#line 2243 "asn1p_y.y"
                            {
		(yyval.a_marker).flags = EM_DEFAULT;
		(yyval.a_marker).default_value = (yyvsp[0].a_value);
	}
#line 4500 "asn1p_y.c"
    break;

  case 303: /* IdentifierList: IdentifierElement  */
#line 2250 "asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
    }
#line 4510 "asn1p_y.c"
    break;

  case 304: /* IdentifierList: IdentifierList ',' IdentifierElement  */
#line 2255 "asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
    }
#line 4519 "asn1p_y.c"
    break;

  case 305: /* IdentifierElement: Identifier  */
#line 2261 "asn1p_y.y"
               {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
    }
#line 4531 "asn1p_y.c"
    break;

  case 306: /* NamedNumberList: NamedNumber  */
#line 2270 "asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4541 "asn1p_y.c"
    break;

  case 307: /* NamedNumberList: NamedNumberList ',' NamedNumber  */
#line 2275 "asn1p_y.y"
                                          {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4550 "asn1p_y.c"
    break;

  case 308: /* NamedNumber: Identifier '(' SignedNumber ')'  */
#line 2282 "asn1p_y.y"
                                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 4563 "asn1p_y.c"
    break;

  case 309: /* NamedNumber: Identifier '(' DefinedValue ')'  */
#line 2290 "asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 4576 "asn1p_y.c"
    break;

  case 310: /* NamedBitList: NamedBit  */
#line 2300 "asn1p_y.y"
                 {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4586 "asn1p_y.c"
    break;

  case 311: /* NamedBitList: NamedBitList ',' NamedBit  */
#line 2305 "asn1p_y.y"
                                    {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4595 "asn1p_y.c"
    break;

  case 312: /* NamedBit: Identifier '(' "number" ')'  */
#line 2312 "asn1p_y.y"
                                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = asn1p_value_fromint((yyvsp[-1].a_int));
	}
#line 4608 "asn1p_y.c"
    break;

  case 313: /* NamedBit: Identifier '(' DefinedValue ')'  */
#line 2320 "asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 4621 "asn1p_y.c"
    break;

  case 314: /* Enumerations: UniverationList  */
#line 2330 "asn1p_y.y"
                    {
		(yyval.a_expr) = (yyvsp[0].a_expr);
        asn1p_expr_t *first_memb = TQ_FIRST(&((yyval.a_expr)->members));
        if(first_memb) {
            if(first_memb->expr_type == A1TC_EXTENSIBLE) {
                return yyerror(param, yyscanner,
                    "The ENUMERATION cannot start with extension (...).");
            }
        } else {
            return yyerror(param, yyscanner,
                "The ENUMERATION list cannot be empty.");
        }
    }
#line 4639 "asn1p_y.c"
    break;

  case 315: /* UniverationList: UniverationElement  */
#line 2345 "asn1p_y.y"
                           {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4649 "asn1p_y.c"
    break;

  case 316: /* UniverationList: UniverationList ',' UniverationElement  */
#line 2350 "asn1p_y.y"
                                                 {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4658 "asn1p_y.c"
    break;

  case 317: /* UniverationElement: Identifier  */
#line 2357 "asn1p_y.y"
                   {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
	}
#line 4670 "asn1p_y.c"
    break;

  case 318: /* UniverationElement: Identifier '(' SignedNumber ')'  */
#line 2364 "asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 4683 "asn1p_y.c"
    break;

  case 319: /* UniverationElement: Identifier '(' DefinedValue ')'  */
#line 2372 "asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 4696 "asn1p_y.c"
    break;

  case 320: /* UniverationElement: SignedNumber  */
#line 2380 "asn1p_y.y"
                       {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 4708 "asn1p_y.c"
    break;

  case 321: /* UniverationElement: "..."  */
#line 2387 "asn1p_y.y"
                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_VALUE;
	}
#line 4721 "asn1p_y.c"
    break;

  case 322: /* SignedNumber: "number"  */
#line 2398 "asn1p_y.y"
                   {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
	}
#line 4730 "asn1p_y.c"
    break;

  case 323: /* SignedNumber: "negative number"  */
#line 2402 "asn1p_y.y"
                              {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
	}
#line 4739 "asn1p_y.c"
    break;

  case 324: /* RealValue: TOK_realnumber  */
#line 2409 "asn1p_y.y"
                       {
		(yyval.a_value) = asn1p_value_fromdouble((yyvsp[0].a_dbl));
		checkmem((yyval.a_value));
	}
#line 4748 "asn1p_y.c"
    break;

  case 325: /* optTag: %empty  */
#line 2440 "asn1p_y.y"
        { memset(&(yyval.a_tag), 0, sizeof((yyval.a_tag))); }
#line 4754 "asn1p_y.c"
    break;

  case 326: /* optTag: Tag  */
#line 2441 "asn1p_y.y"
              { (yyval.a_tag) = (yyvsp[0].a_tag); }
#line 4760 "asn1p_y.c"
    break;

  case 327: /* Tag: TagTypeValue TagPlicit  */
#line 2445 "asn1p_y.y"
                               {
		(yyval.a_tag) = (yyvsp[-1].a_tag);
		(yyval.a_tag).tag_mode = (yyvsp[0].a_tag).tag_mode;
	}
#line 4769 "asn1p_y.c"
    break;

  case 328: /* TagTypeValue: '[' TagClass "number" ']'  */
#line 2452 "asn1p_y.y"
                                    {
		(yyval.a_tag) = (yyvsp[-2].a_tag);
		(yyval.a_tag).tag_value = (yyvsp[-1].a_int);
	}
#line 4778 "asn1p_y.c"
    break;

  case 329: /* TagClass: %empty  */
#line 2458 "asn1p_y.y"
        { (yyval.a_tag).tag_class = TC_CONTEXT_SPECIFIC; }
#line 4784 "asn1p_y.c"
    break;

  case 330: /* TagClass: TOK_UNIVERSAL  */
#line 2459 "asn1p_y.y"
                        { (yyval.a_tag).tag_class = TC_UNIVERSAL; }
#line 4790 "asn1p_y.c"
    break;

  case 331: /* TagClass: TOK_APPLICATION  */
#line 2460 "asn1p_y.y"
                          { (yyval.a_tag).tag_class = TC_APPLICATION; }
#line 4796 "asn1p_y.c"
    break;

  case 332: /* TagClass: TOK_PRIVATE  */
#line 2461 "asn1p_y.y"
                      { (yyval.a_tag).tag_class = TC_PRIVATE; }
#line 4802 "asn1p_y.c"
    break;

  case 333: /* TagPlicit: %empty  */
#line 2465 "asn1p_y.y"
        { (yyval.a_tag).tag_mode = TM_DEFAULT; }
#line 4808 "asn1p_y.c"
    break;

  case 334: /* TagPlicit: TOK_IMPLICIT  */
#line 2466 "asn1p_y.y"
                       { (yyval.a_tag).tag_mode = TM_IMPLICIT; }
#line 4814 "asn1p_y.c"
    break;

  case 335: /* TagPlicit: TOK_EXPLICIT  */
#line 2467 "asn1p_y.y"
                       { (yyval.a_tag).tag_mode = TM_EXPLICIT; }
#line 4820 "asn1p_y.c"
    break;

  case 336: /* TypeRefName: TOK_typereference  */
#line 2471 "asn1p_y.y"
                          {
		checkmem((yyvsp[0].tv_str));
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 4829 "asn1p_y.c"
    break;

  case 337: /* TypeRefName: TOK_capitalreference  */
#line 2475 "asn1p_y.y"
                               {
		checkmem((yyvsp[0].tv_str));
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 4838 "asn1p_y.c"
    break;

  case 338: /* optIdentifier: %empty  */
#line 2483 "asn1p_y.y"
        { (yyval.tv_str) = 0; }
#line 4844 "asn1p_y.c"
    break;

  case 339: /* optIdentifier: Identifier  */
#line 2484 "asn1p_y.y"
                     {
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 4852 "asn1p_y.c"
    break;

  case 340: /* Identifier: "identifier"  */
#line 2490 "asn1p_y.y"
                       {
		checkmem((yyvsp[0].tv_str));
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 4861 "asn1p_y.c"
    break;

  case 341: /* IdentifierAsReference: Identifier  */
#line 2497 "asn1p_y.y"
               {
		(yyval.a_ref) = asn1p_ref_new(asn1p_get_lineno(yyscanner), currentModule);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_lowercase);
		free((yyvsp[0].tv_str));
    }
#line 4871 "asn1p_y.c"
    break;

  case 342: /* IdentifierAsValue: IdentifierAsReference  */
#line 2504 "asn1p_y.y"
                          {
		(yyval.a_value) = asn1p_value_fromref((yyvsp[0].a_ref), 0);
    }
#line 4879 "asn1p_y.c"
    break;


#line 4883 "asn1p_y.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == ASN1P_EMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (ctx, yyscanner, YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= ASN1P_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == ASN1P_EOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, ctx, yyscanner);
          yychar = ASN1P_EMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, ctx, yyscanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (ctx, yyscanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != ASN1P_EMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, ctx, yyscanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, ctx, yyscanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 2508 "asn1p_y.y"



/*
 * Convert Xstring ('0101'B or '5'H) to the binary vector.
 */
static asn1p_value_t *
_convert_bitstring2binary(char *str, int base) {
	asn1p_value_t *val;
	int slen;
	int memlen;
	int baselen;
	int bits;
	uint8_t *binary_vector;
	uint8_t *bv_ptr;
	uint8_t cur_val;

	assert(str);
	assert(str[0] == '\'');

	switch(base) {
	case 'B':
		baselen = 1;
		break;
	case 'H':
		baselen = 4;
		break;
	default:
		assert(base == 'B' || base == 'H');
		errno = EINVAL;
		return NULL;
	}

	slen = strlen(str);
	assert(str[slen - 1] == base);
	assert(str[slen - 2] == '\'');

	memlen = slen / (8 / baselen);	/* Conservative estimate */

	bv_ptr = binary_vector = malloc(memlen + 1);
	if(bv_ptr == NULL)
		/* ENOMEM */
		return NULL;

	cur_val = 0;
	bits = 0;
	while(*(++str) != '\'') {
		switch(baselen) {
		case 1:
			switch(*str) {
			case '1':
				cur_val |= 1 << (7 - (bits % 8));
			case '0':
				break;
			default:
				assert(!"_y UNREACH1");
			case ' ': case '\r': case '\n':
				continue;
			}
			break;
		case 4:
			switch(*str) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				cur_val |= (*str - '0') << (4 - (bits % 8));
				break;
			case 'A': case 'B': case 'C':
			case 'D': case 'E': case 'F':
				cur_val |= ((*str - 'A') + 10)
					<< (4 - (bits % 8));
				break;
			default:
				assert(!"_y UNREACH2");
			case ' ': case '\r': case '\n':
				continue;
			}
			break;
		}

		bits += baselen;
		if((bits % 8) == 0) {
			*bv_ptr++ = cur_val;
			cur_val = 0;
		}
	}

	*bv_ptr = cur_val;
	assert((bv_ptr - binary_vector) <= memlen);

	val = asn1p_value_frombits(binary_vector, bits, 0);
	if(val == NULL) {
		free(binary_vector);
	}

	return val;
}

/*
 * For unnamed types (used in old X.208 compliant modules)
 * generate some sort of interim names, to not to force human being to fix
 * the specification's compliance to modern ASN.1 standards.
 */
static void
_fixup_anonymous_identifier(asn1p_expr_t *expr, yyscan_t yyscanner) {
	char *p;
	assert(expr->Identifier == 0);

	/*
	 * Try to figure out the type name
	 * without going too much into details
	 */
	expr->Identifier = ASN_EXPR_TYPE2STR(expr->expr_type);
	if(expr->reference && expr->reference->comp_count > 0)
		expr->Identifier = expr->reference->components[0].name;

	fprintf(stderr,
		"WARNING: Line %d: expected lower-case member identifier, "
		"found an unnamed %s.\n"
		"WARNING: Obsolete X.208 syntax detected, "
		"please give the member a name.\n",
		asn1p_get_lineno(yyscanner), expr->Identifier ? expr->Identifier : "type");

	if(!expr->Identifier)
		expr->Identifier = "unnamed";
	expr->Identifier = strdup(expr->Identifier);
	assert(expr->Identifier);
	/* Make a lowercase identifier from the type name */
	for(p = expr->Identifier; *p; p++) {
		switch(*p) {
		case 'A' ... 'Z': *p += 32; break;
		case ' ': *p = '_'; break;
		case '-': *p = '_'; break;
		}
	}
	fprintf(stderr, "NOTE: Assigning temporary identifier \"%s\". "
			"Name clash may occur later.\n",
		expr->Identifier);
}

static int
yyerror(void **param, yyscan_t yyscanner, const char *msg) {
	fprintf(stderr,
		"ASN.1 grammar parse error "
		"near %s:%d (token \"%s\"): %s\n",
		ASN_FILENAME, asn1p_get_lineno(yyscanner), asn1p_get_text(yyscanner), msg);
	return -1;
}

void asn1p_yctx_init(asn1p_yctx_t *ctx) {

}

void asn1p_yctx_free(asn1p_yctx_t *ctx) {

}
