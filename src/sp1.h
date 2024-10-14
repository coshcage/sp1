/*
 * Name:        sp1.h
 * Description: Stack parser 1 header.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0926241234A0926241234L00074
 * License:     GPLv3.
 */
#ifndef _SP1_H_
#define _SP1_H_

#include "svstring.h"
#include "svqueue.h"
#include "svtree.h"
#include "svregex.h"

typedef enum st_TRMTYPE
{
	TT_NONE,        /* No type. */
	TT_OPERAND,     /* Operand. */
	TT_OPERATOR,    /* Operator. */
	TT_LPAR,        /* Left parenthesis. */
	TT_RPAR,        /* Right parenthesis. */
	TT_IDENTIFIER,  /* Identifier. */
	TT_SPACE,       /* Spaces. */
	TT_EOL          /* End of line. */
} TRMTYPE;

typedef enum st_ADDTYPE
{
	AT_NONE = 0,       /* No type. */
	// AT_MIDDLE,     /* Infix. */
	AT_PREFIX = 1,     /* Prefix. */
	// AT_SUFFIX,     /* Suffix. */
	AT_NUMBER = 2,     /* Numbers. */
	AT_IDENTIFIER = 4,  /* Identifiers. */
	AT_ASSOCIATIVITY_RTL = 8
} ADDTYPE;

/* Error printing function prototype. */
typedef void (*CBF_ERROR)(size_t id, size_t ln, size_t col);

typedef struct st_TRM
{
	wchar_t * re; /* Regular expression or lexicon. */
	TRMTYPE type; /* Type. */
	ADDTYPE adtp; /* Additional type. */
	ptrdiff_t il; /* Length. */
	size_t level; /* Level. */
	ptrdiff_t pc; /* Parameter counter. */
	size_t calbk; /* Call back function. */
	size_t x, y;  /* Line and column number. */
} TRM, * P_TRM;

/* Structure for DFA sequence. */
typedef struct st_DFASEQ
{
	size_t  num;	  /* Returning number. */
	P_DFA   pdfa;     /* Pointer to DFA. */
	size_t  curstate; /* Current state. */
} DFASEQ, * P_DFASEQ;

/* Function level. */
#define SP1C_FN_LEVEL 4

/* Function declaration. */
P_QUEUE_L  sp1LexCompile(P_ARRAY_Z parrlex);
void       sp1LexDestroy(P_QUEUE_L pq);
void       sp1DestroySyntaxTree(P_TNODE_BY pnode);
P_TNODE_BY sp1ParseExpression(P_QUEUE_L pq, P_ARRAY_Z parrlex, P_TRIE_A ptafn, wchar_t * wcstr, CBF_ERROR err, size_t * pln, size_t * pcol);
void       sp1PrintSyntaxTree(P_TNODE_BY pnode, size_t space);
BOOL       sp1RegisterFunction(P_TRIE_A ptafn, wchar_t * name, size_t pc);
BOOL       sp1UnregisterFunction(P_TRIE_A ptafn, wchar_t * name);

#endif
