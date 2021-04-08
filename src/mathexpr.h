/******************************************************************************
**  MODULE:        MATHEXPR.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   header file for the math expression parser in mathexpr.c.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.1.00
**  LAST UPDATE:   2/8/11
******************************************************************************/

//  Node in a tokenized math expression list
struct ExprNode
{
    int    opcode;                // operator code
    int    ivar;                  // variable index
    double fvalue;                // numerical value
    struct ExprNode *prev;        // previous node
    struct ExprNode *next;        // next node
};
typedef struct ExprNode MathExpr;

//  Creates a tokenized math expression from a string
MathExpr* mathexpr_create(char* s, int (*getVar) (char *));

//  Evaluates a tokenized math expression
double mathexpr_eval(MathExpr* expr, double (*getVal) (int));

//  Deletes a tokenized math expression
void  mathexpr_delete(MathExpr* expr);

// Returns reconstructed string version of a tokenized expression              //1.1.00
char * mathexpr_getStr(MathExpr* expr, char* exprStr,
                       char * (*getVariableStr) (int, char *));
