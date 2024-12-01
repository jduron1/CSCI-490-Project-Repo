#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include "ast.h"

void generateSimpleCode(FILE *, ASTNode *);
void generateConstCode(FILE *, ASTNode *);
void generateRefCode(FILE *, ASTNode *);
void generateArithCode(FILE *, ASTNode *);
void generateIncrCode(FILE *, ASTNode *);
void generateBoolCode(FILE *, ASTNode *);
void generateRelCode(FILE *, ASTNode *);
void generateEquCode(FILE *, ASTNode *);
void generateDeclCode(FILE *, ASTNode *);
void generateStatementCode(FILE *, ASTNode *);
void generateIfCode(FILE *, ASTNode *);
void generateElseIfCode(FILE *, ASTNode *);
void generateForCode(FILE *, ASTNode *);
void generateForEachCode(FILE *, ASTNode *);
void generateWhileCode(FILE *, ASTNode *);
void generateAssignCode(FILE *, ASTNode *);
void generateArithAssignCode(FILE *, ASTNode *);
void generateFuncCallCode(FILE *, ASTNode *);
void generateCallArgsCode(FILE *, ASTNode *);
void generateFuncDeclCode(FILE *, ASTNode *);
void generateDeclArgsCode(FILE *, ASTNode *);
void generateReturnCode(FILE *, ASTNode *);
void generateParenCode(FILE *, ASTNode *);
void findNodeType(FILE *, ASTNode *);

#endif // CODE_GEN_H