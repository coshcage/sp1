/*
 * Name:        sp1.c
 * Description: Stack parser 1.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0926241234B0926241234L00589
 * License:     GPLv3.
 */
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sp1.h"
#include "svstack.h"

/* File level function declarations. */
static int    cbftvsLexerPuppet(void * pitem, size_t param);
static size_t Lexer(P_QUEUE_L pq, wchar_t wc);
static int    cbftvsLexerDestroyPuppet(void * pitem, size_t param);
static int    cbfcmpWChar_T(const void * px, const void * py);
static int    cbftvsClearSyntaxTreeNode(void * pitem, size_t param);
static int    cbftvsClearStack(void * pitem, size_t param);
static BOOL   Pop1Operator(P_STACK_L pstkOperand, P_STACK_L pstkOperator);
static void   PrintTRM(P_TRM pt);

/* Function name: sp1LexCompile
 * Description:   Compile an array of TRMs to lex queue.
 * Parameter:
 *   parrlex An array of TRMs.
 * Return value:  A lex queue.
 */
P_QUEUE_L sp1LexCompile(P_ARRAY_Z parrlex)
{
	P_QUEUE_L pq = queCreateL();

	if (NULL != pq)
	{
		size_t i;
		for (i = 0; i < strLevelArrayZ(parrlex); ++i)
		{
			P_TRM pt;
			DFASEQ dq = { 0 };
			pt = (P_TRM)strLocateItemArrayZ(parrlex, sizeof(TRM), i);
			dq.num = i + 1;
			if (L'0' == *pt->re)
			{
				dq.pdfa = NULL;
			}
			else if (NULL == (dq.pdfa = CompileRegex2DFA(pt->re)))
			{
				dq.num = 0;
				queDeleteL(pq);
				return 0;
			}
			queInsertL(pq, &dq, sizeof(DFASEQ));
		}
	}

	return pq;
}

/* Attention:	  This Is An Internal Function. No Interface for Library Users.
 * Function name: cbftvsLexerPuppet
 * Description:   Callback for lex matching.
 * Parameters:
 *      pitem Pointer to each node of a queue.
 *      param Pointer to a size_t[2] array.
 * Return value:  CBF_CONTINUE only.
 */
static int cbftvsLexerPuppet(void * pitem, size_t param)
{
	size_t i;
	P_DFASEQ pdfaq = (P_DFASEQ)((P_NODE_S)pitem)->pdata;

	if (0 == pdfaq->curstate)
		pdfaq->curstate = 1;

	pdfaq->curstate = NextState(pdfaq->pdfa, pdfaq->curstate, (wchar_t)0[(size_t *)param]);
	strGetValueMatrix(&i, pdfaq->pdfa, pdfaq->curstate, 0, sizeof(size_t));
	if (i & SIGN)
	{
		1[(size_t *)param] = pdfaq->num;
		pdfaq->curstate = 1;
	}

	return CBF_CONTINUE;
}

/* Attention:	  This Is An Internal Function. No Interface for Library Users.
 * Function name: Lexer
 * Description:   Lexical analyzer.
 * Parameters:
 *         pq Pointer to a queue of DFAs.
 *         wc Inputted wide character.
 * Return value:  Line number of regular expression in L file.
 */
static size_t Lexer(P_QUEUE_L pq, wchar_t wc)
{
	size_t a[2];

	a[0] = wc;
	a[1] = 0;

	strTraverseLinkedListSC_N(pq->pfront, NULL, cbftvsLexerPuppet, (size_t)a);

	return a[1];
}

/* Attention:	 This Is An Internal Function. No Interface for Library Users.
 * Function name: cbftvsLexerDestroyPuppet
 * Description:   Free DFAs.
 * Parameters:
 *      pitem Pointer to each node of a queue.
 *      param N/A.
 * Return value:  CBF_CONTINUE only.
 */
static int cbftvsLexerDestroyPuppet(void * pitem, size_t param)
{
	P_DFASEQ pdfaq = (P_DFASEQ)((P_NODE_S)pitem)->pdata;

	DWC4100(param);
	if (pdfaq->pdfa)
		DestroyDFA(pdfaq->pdfa);

	return CBF_CONTINUE;
}

/* Function name: sp1LexDestroy
 * Description:   Free the queue which is derived form LexCompile.
 * Parameter:
 *        pq Pointer to a queue of DFAs.
 * Return value:  N/A.
 */
void sp1LexDestroy(P_QUEUE_L pq)
{
	strTraverseLinkedListSC_N(pq->pfront, NULL, cbftvsLexerDestroyPuppet, 0);
	queDeleteL(pq);
}

/* Attention:	 This Is An Internal Function. No Interface for Library Users.
 * Function name: cbfcmpWChar_T
 * Description:   Compare two wchar_t.
 * Parameters:
 *         px Pointer to the first element.
 *         py Pointer to the second element.
 * Return value:  -1,0,1 respectively.
 */
static int cbfcmpWChar_T(const void * px, const void * py)
{
	if (*(wchar_t *)px > *(wchar_t *)py) return 1;
	if (*(wchar_t *)px < *(wchar_t *)py) return -1;
	return 0;
}

/* Attention:	 This Is An Internal Function. No Interface for Library Users.
 * Function name: cbftvsClearSyntaxTreeNode
 * Description:   Free syntax tree nodes.
 * Parameters:
 *      pitem Pointer to each node in the tree.
 *      param N/A.
 * Return value:  CBF_CONTINUE only.
 */
static int cbftvsClearSyntaxTreeNode(void * pitem, size_t param)
{
	DWC4100(param);
	if (NULL != ((P_TRM)P2P_TNODE_BY(pitem)->pdata)->re)
		free(((P_TRM)P2P_TNODE_BY(pitem)->pdata)->re);
	((P_TRM)P2P_TNODE_BY(pitem)->pdata)->re = NULL;
	return CBF_CONTINUE;
}

/* Function name: sp1DestroySyntaxTree
 * Description:   Free a syntax tree.
 * Parameter:
 *     pnode Pointer to the root node of a syntax tree.
 * Return value:  N/A.
 */
void sp1DestroySyntaxTree(P_TNODE_BY pnode)
{
	treMorrisTraverseBYPre(pnode, cbftvsClearSyntaxTreeNode, 0);
	treFreeBY(&pnode);
}

/* Attention:	  This Is An Internal Function. No Interface for Library Users.
 * Function name: cbftvsClearStack
 * Description:   Clean stacks.
 * Parameters:
 *      pitem Pointer to each node in the stack.
 *      param N/A.
 * Return value:  CBF_CONTINUE only.
 */
static int cbftvsClearStack(void * pitem, size_t param)
{
	P_TNODE_BY pnode = *(P_TNODE_BY *)((P_NODE_S)pitem)->pdata;
	DWC4100(param);
	treMorrisTraverseBYPre(pnode, cbftvsClearSyntaxTreeNode, 0);
	return CBF_CONTINUE;
}

/* Attention:	  This Is An Internal Function. No Interface for Library Users.
 * Function name: Pop1Operator
 * Description:   Pop 1 operator.
 * Parameters:
 * pstkOperand Pointer to the operand stack.
 * pstkOperator Pointer to the operator stack.
 * Return value:  TRUE or FALSE.
 */
static BOOL Pop1Operator(P_STACK_L pstkOperand, P_STACK_L pstkOperator)
{
	if (!stkIsEmptyL(pstkOperator))
	{
		size_t m, n;
		P_TNODE_BY pnodet, pnode;
		P_ARRAY_Z par;
		
		stkPopL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
		n = ((P_TRM)pnode->pdata)->pc;
		par = strCreateArrayZ(n, sizeof(P_TNODE_BY));
		for (m = 0; m < n; ++m)
		{
			*(P_TNODE_BY *)strLocateItemArrayZ(par, sizeof(P_TNODE_BY), m) = NULL;
			if (!stkIsEmptyL(pstkOperand))
			{
				stkPopL(&pnodet, sizeof(P_TNODE_BY), pstkOperand);
				*(P_TNODE_BY *)strLocateItemArrayZ(par, sizeof(P_TNODE_BY), m) = pnodet;
			}
			else
			{
				stkPushL(pstkOperator, &pnode, sizeof(P_TNODE_BY));
				strDeleteArrayZ(par);
				return FALSE;
			}
		}
		strReverseArrayZ(par, &pnodet, sizeof(P_TNODE_BY));
		switch(n)
		{
		case 1:
			pnode->ppnode[LEFT] = *(P_TNODE_BY *)strLocateItemArrayZ(par, sizeof(P_TNODE_BY), LEFT);
			break;
		case 2:
			memcpy(pnode->ppnode, par->pdata, sizeof(P_TNODE_BY) * 2);
			break;
		default:
			pnode->ppnode[RIGHT] = *(P_TNODE_BY *)strLocateItemArrayZ(par, sizeof(P_TNODE_BY), 0);
			for (pnodet = pnode, m = 0; m < n - 1; ++m)
			{
				pnodet->ppnode[RIGHT] = *(P_TNODE_BY *)strLocateItemArrayZ(par, sizeof(P_TNODE_BY), m + 1);
				pnodet = pnodet->ppnode[RIGHT];

			}
			break;
		}
		stkPushL(pstkOperand, &pnode, sizeof(P_TNODE_BY));
		strDeleteArrayZ(par);
		return TRUE;
	}
	return FALSE;
}

/* Function name: sp1ParseExpression
 * Description:   Parser.
 * Parameter:
 *        pq Lex queue.
 *   parrlex An array of TRMs.
 *     ptafn Pointer to a trie of function names.
 *     wcstr The expression string.
 *       err Pointer to a function that handles errors.
 *       pln Pointer to line number.
 *      pcol Pointer to column number.
 * Return value:  Syntax tree node.
 */
P_TNODE_BY sp1ParseExpression(P_QUEUE_L pq, P_ARRAY_Z parrlex, P_TRIE_A ptafn, wchar_t * wcstr, CBF_ERROR err, size_t * pln, size_t * pcol)
{
	P_TRM pt;
	ptrdiff_t il = 0;
	size_t s, prvs = 0, prvtp = TT_NONE;
	P_TNODE_BY pnode = NULL;
	P_STACK_L pstkOperator = NULL, pstkOperand = NULL;
	wchar_t buf[BUFSIZ] = {0}, * pbuf = buf, prvc = *wcstr;

	pstkOperator = stkCreateL();
	pstkOperand = stkCreateL();

	if (NULL == pstkOperator || NULL == pstkOperand)
	{
		if (NULL != pstkOperator)
			stkDeleteL(pstkOperator);
		if (NULL != pstkOperand)
			stkDeleteL(pstkOperand);
		return NULL;
	}

	while (L'\0' != prvc)
	{
		s = Lexer(pq, *wcstr);
		switch (*wcstr)
		{
		case L'\r':
		case L'\n':
			if (L'\r' != prvc && L'\n' != prvc)
			{
				++*pln;
				*pcol = 1;
			}
			break;
		default:
			++*pcol;
		}
		
		if (prvs <= strLevelArrayZ(parrlex) && prvs > 0)
			pt = (P_TRM)strLocateItemArrayZ(parrlex, sizeof(TRM), prvs - 1);
		else
			pt = NULL;
			
		if ((NULL != pt && (pt->il != -1 || il + 1 == pt->il)) || NULL == pt || prvs != s)
		{
			if (L'\0' != buf[0] && NULL != pt)
			{
				size_t * psiz;
				TRM trm = { 0 };
						
				trm = *pt;
				trm.x = *pln;
				trm.y = *pcol;
						
				switch (pt->type)
				{
				case TT_NONE:
				case TT_SPACE:
					trm.type = TT_NONE;
					break;
				case TT_EOL:
					goto Lbl_Finish;
					break;
				case TT_OPERAND:
				HandleOperand:
					trm.re = wcsdup(buf);
					trm.type = TT_OPERAND;
					pnode = strCreateNodeD(&trm, sizeof(TRM));
					stkPushL(pstkOperand, &pnode, sizeof(P_TNODE_BY));
					break;
				case TT_OPERATOR:
					if (TT_OPERAND != prvtp && TT_RPAR != prvtp) /* Change to prefix. */
					{
						++pt;
						if (!(pt - (P_TRM)parrlex->pdata < (ptrdiff_t)strLevelArrayZ(parrlex) && L'\0' == *pt->re)) /* Ensure that pt is in range. */
							--pt;
						trm = *pt;
						trm.x = *pln;
						trm.y = *pcol;
					}
				HandleOperator:
					if (!stkIsEmptyL(pstkOperator))
					{
						stkPeepL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
						if (((P_TRM)pnode->pdata)->level >= trm.level)
						{
							if (!Pop1Operator(pstkOperand, pstkOperator))
							{
								if (NULL != err)
									err(0x1, *pln, *pcol);
								goto Lbl_Error;
							}
						}
					}
					if (!stkIsEmptyL(pstkOperator))
					{
						stkPeepL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
						if (((P_TRM)pnode->pdata)->level >= trm.level)
							goto HandleOperator;
					}
					trm.re = wcsdup(buf);
					pnode = strCreateNodeD(&trm, sizeof(TRM));
					stkPushL(pstkOperator, &pnode, sizeof(P_TNODE_BY));
					break;
				case TT_LPAR:
					if (TT_RPAR == prvtp)
					{
						if (NULL != err)
							err(0x2, *pln, *pcol);
						goto Lbl_Error;
					}
					trm.re = NULL;
					pnode = strCreateNodeD(&trm, sizeof(TRM));
					stkPushL(pstkOperator, &pnode, sizeof(P_TNODE_BY));
					break;
				case TT_RPAR:
					if (TT_LPAR == prvtp)
					{
						if (NULL != err)
							err(0x1, *pln, *pcol);
						goto Lbl_Error;
					}
				HandleRPar:
					if (!stkIsEmptyL(pstkOperator))
					{
						stkPeepL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
						if (TT_LPAR != ((P_TRM)pnode->pdata)->type)
						{
							if (!Pop1Operator(pstkOperand, pstkOperator))
							{
								if (NULL != err)
									err(0x1, *pln, *pcol);
								goto Lbl_Error;
							}
						}
					}
					if (!stkIsEmptyL(pstkOperator))
					{
						stkPeepL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
						if (TT_LPAR != ((P_TRM)pnode->pdata)->type)
						{
							goto HandleRPar;
						}
					}
					if (stkIsEmptyL(pstkOperator))
					{
						if (NULL != err)
							err(0x4, *pln, *pcol);
						goto Lbl_Error;
					}
					stkPopL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
					break;
				case TT_IDENTIFIER:
					psiz = treSearchTrieA(ptafn, buf, wcslen(buf) - 1, sizeof(wchar_t), cbfcmpWChar_T);
					if (NULL != psiz) /* This is a function. */
					{
						trm.pc = *psiz;
						trm.level = SP1C_FN_LEVEL;
						trm.type = TT_OPERATOR;
						if (0 != trm.type)
							prvtp = trm.type;
						goto HandleOperator;
					}
					else
					{
						if (0 != trm.type)
							prvtp = trm.type;
						goto HandleOperand;
					}
					psiz = NULL;
					break;
				}
					
				if (0 != trm.type)
					prvtp = trm.type;
			}

			pbuf = buf;
			il = 0;
		}
		else
		{
			++il;
			if (pbuf - buf >= BUFSIZ)
			{
				if (NULL != err)
					err(0x3, *pln, *pcol);
				goto Lbl_Error;
			}
		}

		*pbuf = *wcstr;
		++pbuf;
		*pbuf = L'\0';
		
		prvs = s;
		prvc = *wcstr;
		++wcstr;
	}
Lbl_Finish:
	while (!stkIsEmptyL(pstkOperator))
	{
		stkPeepL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
		switch (((P_TRM)pnode->pdata)->type)
		{
		case TT_LPAR:
		case TT_RPAR:
			if (NULL != err)
				err(0x4, *pln, *pcol);
			goto Lbl_Error;
		}
		if (!Pop1Operator(pstkOperand, pstkOperator))
		{
			if (NULL != err)
			{
				P_TRM pt = (P_TRM)pnode->pdata;
				err(0x1, pt->x, pt->y);
			
			}
			goto Lbl_Error;
		}
	}
	if (!stkIsEmptyL(pstkOperand))
		stkPopL(&pnode, sizeof(P_TNODE_BY), pstkOperand);

	if (!stkIsEmptyL(pstkOperand) || !stkIsEmptyL(pstkOperator))
	{
		if (NULL != err)
			err(0x1, *pln, *pcol);
	}
Lbl_Error:
	do
	{
		if (!stkIsEmptyL(pstkOperand))
		{
			stkPopL(&pnode, sizeof(P_TNODE_BY), pstkOperand);
			sp1DestroySyntaxTree(pnode);
			pnode = NULL;
		}
		if (!stkIsEmptyL(pstkOperator))
		{
			stkPopL(&pnode, sizeof(P_TNODE_BY), pstkOperator);
			sp1DestroySyntaxTree(pnode);
			pnode = NULL;
		}
	}
	while (!stkIsEmptyL(pstkOperand) || !stkIsEmptyL(pstkOperator));
	
	strTraverseLinkedListSC_X(*pstkOperator, NULL, cbftvsClearStack, 0);
	strTraverseLinkedListSC_X(*pstkOperand, NULL, cbftvsClearStack, 0);
	stkDeleteL(pstkOperator);
	stkDeleteL(pstkOperand);
	return pnode;
}

/* Attention:	  This Is An Internal Function. No Interface for Library Users.
 * Function name: PrintTRM
 * Description:   Print TRM structure.
 * Parameter:
 *        pt Pointer to a TRM structure.
 * Return value:  N/A.
 */
static void PrintTRM(P_TRM pt)
{
	wprintf(L"%ls(%ld,%ld)", pt->re, pt->type, pt->adtp);
}

/* Function name: sp1PrintSyntaxTree
 * Description:   Print a whole syntax tree.
 * Parameters:
 *      pnode The root node of the tree.
 *      space Spaces to indent tree nodes.
 * Return value:  N/A.
 */
void sp1PrintSyntaxTree(P_TNODE_BY pnode, size_t space)
{
	size_t i;
	if (NULL == pnode)
		return;

	space += TREE_NODE_SPACE_COUNT;

	/* Process right child first. */
	sp1PrintSyntaxTree(pnode->ppnode[RIGHT], space);

	/* Print current node after space count. */
	wprintf(L"\n");
	for (i = TREE_NODE_SPACE_COUNT; i < space; ++i)
		wprintf(L" ");

	PrintTRM((P_TRM)((P_TNODE_BY)pnode)->pdata);
	sp1PrintSyntaxTree(pnode->ppnode[LEFT], space);
}

/* Function name: sp1RegisterFunction
 * Description:   Register function name.
 * Parameter:
 *     ptafn Pointer to a trie.
 *      name Name wide string.
 *        pc Parameter counter.
 * Return value:  TRUE or FALSE.
 */
BOOL sp1RegisterFunction(P_TRIE_A ptafn, wchar_t * name, size_t pc)
{
	return treInsertTrieA(ptafn, name, wcslen(name) - 1, sizeof(wchar_t), pc, cbfcmpWChar_T);
}

/* Function name: sp1UnregisterFunction
 * Description:   Unregister function name.
 * Parameter:
 *     ptafn Pointer to a trie.
 *      name Name wide string.
 * Return value:  TRUE or FALSE.
 */
BOOL sp1UnregisterFunction(P_TRIE_A ptafn, wchar_t * name)
{
	return treRemoveTrieA(ptafn, name, wcslen(name) - 1, sizeof(wchar_t), cbfcmpWChar_T);
}

