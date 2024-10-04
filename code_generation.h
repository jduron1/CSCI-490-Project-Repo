#ifndef CODE_GENERATION_H
#define CODE_GENERATION_H

#include <stdio.h>
#include "ast.h"
#include "semantics.h"
#include "symbol_table.h"

void generateSimpleCode(FILE*, ASTNode*);
void generateConstCode(FILE*, ASTNode*);
void generateRefCode(FILE*, ASTNode*);
void generateArithCode(FILE*, ASTNode*);
void generateIncrCode(FILE*, ASTNode*);
void generateBoolCode(FILE*, ASTNode*);
void generateRelCode(FILE*, ASTNode*);
void generateEquCode(FILE*, ASTNode*);
void generateDeclCode(FILE*, ASTNode*);
void generateStatementCode(FILE*, ASTNode*);
void generateIfCode(FILE*, ASTNode*);
void generateElseIfCode(FILE*, ASTNode*);
void generateForCode(FILE*, ASTNode*);
void generateWhileCode(FILE*, ASTNode*);
void generateAssignCode(FILE*, ASTNode*);
void generateArithAssignCode(FILE*, ASTNode*);
void generateFuncCallCode(FILE*, ASTNode*);
void generateCallArgsCode(FILE*, ASTNode*);
void generateFuncDeclCode(FILE*, ASTNode*);
void generateDeclArgsCode(FILE*, ASTNode*);
void generateReturnCode(FILE*, ASTNode*);
void generateParenCode(FILE*, ASTNode*);
void findNodeType(ASTNode*);

#endif