/*
 * Name:        sp1.c
 * Description: Stack parser 1 calculator.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0926241234C0926241234L00341
 * License:     GPLv3.
 */
#include <stdio.h>
#include <wchar.h>
#include <math.h>
#include "sp1.h"
#include "svstack.h"

TRM trm[] =
{
	{
		L"+",
		TT_OPERATOR,
		AT_NONE,
		1,
		1,
		2,
		0,
		0,
		0
	},
	{
		L"",
		TT_OPERATOR,
		AT_PREFIX,
		1,
		3,
		1,
		0,
		0,
		0
	},
	{
		L"-",
		TT_OPERATOR,
		AT_NONE,
		1,
		1,
		2,
		0,
		0,
		0
	},
	{
		L"",
		TT_OPERATOR,
		AT_PREFIX,
		1,
		3,
		1,
		0,
		0,
		0
	},
	{
		L"\\*",
		TT_OPERATOR,
		AT_NONE,
		1,
		2,
		2,
		0,
		0,
		0
	},
	{
		L"/",
		TT_OPERATOR,
		AT_NONE,
		1,
		2,
		2,
		0,
		0,
		0
	},
	{
		L"\\(",
		TT_LPAR,
		AT_NONE,
		1,
		0,
		0,
		0,
		0,
		0
	},
	{
		L"\\)",
		TT_RPAR,
		AT_NONE,
		1,
		0,
		0,
		0,
		0,
		0
	},
	{
		L",",
		TT_NONE,
		AT_NONE,
		1,
		0,
		0,
		0,
		0,
		0
	},
	{
		L"\\.",
		TT_OPERATOR,
		AT_NONE,
		1,
		5,
		2,
		0,
		0,
		0
	},
	{
		L"(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*",
		TT_OPERAND,
		AT_NUMBER,
		-1,
		0,
		0,
		0,
		0,
		0
	},
	{
		L"(_|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)(_|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9)*",
		TT_IDENTIFIER,
		AT_IDENTIFIER,
		-1,
		0,
		0,
		0,
		0,
		0
	},
	{
		L" |\t",
		TT_SPACE,
		AT_NONE,
		-1,
		0,
		0,
		0,
		0,
		0
	},
	{
		L";",
		TT_EOL,
		AT_NONE,
		1,
		0,
		0,
		0,
		0,
		0
	},
};

void pperror(size_t id, size_t ln, size_t col)
{
	switch (id)
	{
	case 0x1:
		wprintf(L"Miss operand! At line: %zd, column: %zd.\n", ln, col);
		break;
	case 0x2:
		wprintf(L"Miss operator! At line: %zd, column: %zd.\n", ln, col);
		break;
	case 0x3:
		wprintf(L"Identifier is too long! At line: %zd, column: %zd.\n", ln, col);
		break;
	case 0x4:
		wprintf(L"Miss parenthesis! At line: %zd, column: %zd.\n", ln, col);
		break;
	case 0x5:
		wprintf(L"Undefined identifier! At line: %zd, column: %zd.\n", ln, col);
		break;
	}
}

int cbftvsComputeSyntaxTree(void * pitem, size_t param)
{
	P_TNODE_BY pnode = (P_TNODE_BY)pitem;
	P_STACK_L pstk = (P_STACK_L)param;
	P_TRM pt = (P_TRM)pnode->pdata;
	wchar_t ** p = NULL;
	double d, d1, d2;
	size_t i, j;
	double (*pfn[1])(double);
	pfn[0] = sin;

	switch (pt->type)
	{
	case TT_OPERAND:
		if (AT_IDENTIFIER == pt->adtp)
		{
			pperror(0x5, pt->x, pt->y);
			return CBF_TERMINATE;
		}
		d = wcstod(pt->re, p);
		stkPushL(pstk, &d, sizeof(double));
		break;
	case TT_OPERATOR:
		if (AT_IDENTIFIER != pt->adtp)
		{
			switch (pt->re[0])
			{
			case L'+':
				if (AT_PREFIX != pt->adtp)
				{
					stkPopL(&d1, sizeof(double), pstk);
					stkPopL(&d2, sizeof(double), pstk);
					d = d1 + d2;
					stkPushL(pstk, &d, sizeof(double));
				}
				break;
			case L'-':
				if (AT_PREFIX != pt->adtp)
				{
					stkPopL(&d1, sizeof(double), pstk);
					stkPopL(&d2, sizeof(double), pstk);
					d = d1 - d2;

				}
				else
				{
					stkPopL(&d1, sizeof(double), pstk);
					d = -1 * d1;
				}
				stkPushL(pstk, &d, sizeof(double));
				break;
			case L'*':
				stkPopL(&d1, sizeof(double), pstk);
				stkPopL(&d2, sizeof(double), pstk);
				d = d1 * d2;
				stkPushL(pstk, &d, sizeof(double));
				break;
			case L'/':
				stkPopL(&d1, sizeof(double), pstk);
				stkPopL(&d2, sizeof(double), pstk);
				d = d2 / d1;
				stkPushL(pstk, &d, sizeof(double));
				break;
			case L'.':
				stkPopL(&d1, sizeof(double), pstk);
				stkPopL(&d2, sizeof(double), pstk);
				j = wcslen(((P_TRM)pnode->ppnode[RIGHT]->pdata)->re);
				for (i = 0; i < j; ++i)
				{
					d1 /= 10;
				}
				d = d2 + d1;
				stkPushL(pstk, &d, sizeof(double));
				break;
			}
		}
		else
		{
			if (!wcscmp(pt->re, L"sin"))
			{
				stkPopL(&d1, sizeof(double), pstk);
				d = pfn[pt->calbk = 0](d1);
				stkPushL(pstk, &d, sizeof(double));
			}
		}
		break;
	case TT_IDENTIFIER:
		pperror(0x5, pt->x, pt->y);
		return CBF_TERMINATE;
		break;
	}
	return CBF_CONTINUE;
}

double ComputeSyntaxTree(P_TNODE_BY pnode)
{
	P_STACK_L pstkNum = stkCreateL();

	if (CBF_TERMINATE != treTraverseBYPost(pnode, cbftvsComputeSyntaxTree, (size_t)pstkNum))
	{
		double d;
		stkPopL(&d, sizeof(double), pstkNum);
		return d;

	}

	stkDeleteL(pstkNum);
	return 0.0;
}

// #define WCSTR L"1 - - 1"

/* Program entry.
 */
int main()
{
	size_t x = 1, y = 1;
	wchar_t buff[BUFSIZ] = { 0 };
	P_TNODE_BY pnode;
	P_TRIE_A pt;
	ARRAY_Z parr;
	P_QUEUE_L pq;
	parr.num = 14;
	parr.pdata = (PUCHAR)trm;

	pt = treCreateTrieA();
	pq = sp1LexCompile(&parr);

	sp1RegisterFunction(pt, L"sin", 1);
	
	wscanf(L"%ls", buff);
	
	pnode = sp1ParseExpression(pq, &parr, pt, buff, pperror, &x, &y);
	if (NULL != pnode)
	{
		sp1PrintSyntaxTree(pnode, 0);
		wprintf(L"\n");
		wprintf(L"\n= %lf\n", ComputeSyntaxTree(pnode));

		sp1DestroySyntaxTree(pnode);
	}

	sp1LexDestroy(pq);
	treDeleteTrieA(pt, sizeof(wchar_t));

	return 0;
}
