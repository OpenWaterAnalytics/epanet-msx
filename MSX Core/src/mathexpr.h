/******************************************************************************
**  MODULE:        MATHEXPR.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   header file for the math expression parser in mathexpr.c.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
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

// Opaque Pointer
typedef struct Project *MSXproject;

//  Creates a tokenized math expression from a string
MathExpr* mathexpr_create(MSXproject MSX, char* s, int (*getVar) (MSXproject, char *));

//  Evaluates a tokenized math expression
double mathexpr_eval(MSXproject MSX, MathExpr* expr, double (*getVal) (MSXproject, int));

//  Deletes a tokenized math expression
void  mathexpr_delete(MathExpr* expr);

// Returns reconstructed string version of a tokenized expression              //1.1.00
char * mathexpr_getStr(MathExpr* expr, char* exprStr,
                       char * (*getVariableStr) (int, char *));
