%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "parser.tab.h"
    #include "symbol_table.h"
    #include "semantics.h"
    #include "ast.h"
%}

%option noyywrap
%x MULTI_LINE

CARACTER            (?i:car)
BOOLEANO            (?i:bool)
ENTERO              (?i:ent)
REAL                (?i:real)
CADENA              (?i:cad)
VACIO               (?i:vacio)
FUNCION             (?i:func)
CIERTO              (?i:cierto)
FALSO               (?i:falso)
SI                  (?i:si)
SINO                (?i:sino)
POR                 (?i:por)
EN                  (?i:en)
MIENTRAS            (?i:mientras)
PARAR               (?i:parar)
CONTINUAR           (?i:continuar)
REGRESAR            (?i:regresar)

IDENTIFIER          [a-zA-Z][a-zA-Z0-9_]*

INTEGER             [0-9]+
FLOAT               [0-9]+(\.[0-9]+)

CHARACTER           \'[^\']\'
STRING              \"[^\"]*\"

SINGLE_LINE         \#\#.*

WHITESPACE          [\ \f\r\t\v]+

%%

{SINGLE_LINE}       { }

"#*"                { BEGIN(MULTI_LINE); }
<MULTI_LINE>"*#"    { BEGIN(INITIAL); }
<MULTI_LINE>[^*\n]+ { }
<MULTI_LINE>"*"     { }
<MULTI_LINE>\n      { yylineno++; }

{CARACTER}          { return CARACTER; }
{BOOLEANO}          { return BOOLEANO; }
{ENTERO}            { return ENTERO; }
{REAL}              { return REAL; }
{CADENA}            { return CADENA; }
{VACIO}             { return VACIO; }
{FUNCION}           { return FUNCION; }
{CIERTO}            { yylval.value.boolean = 1; return CIERTO; }
{FALSO}             { yylval.value.boolean = 0; return FALSO; }
{SI}                { return SI; }
{SINO}              { return SINO; }
{POR}               { return POR; }
{EN}                { return EN; }
{MIENTRAS}          { return MIENTRAS; }
{PARAR}             { return PARAR; }
{CONTINUAR}         { return CONTINUAR; }
{REGRESAR}          { return REGRESAR; }

"+"                 { yylval.value.integer = OP_ADD; return ADD; }
"-"                 { yylval.value.integer = OP_SUB; return SUB; }
"*"                 { yylval.value.integer = OP_MUL; return MUL; }
"/"                 { yylval.value.integer = OP_DIV; return DIV; }
"%"                 { yylval.value.integer = OP_MOD; return MOD; }
"^"                 { yylval.value.integer = OP_EXP; return EXP; }
"+="                { yylval.value.integer = OP_ADD_ASSIGN; return ADD_ASSIGN; }
"-="                { yylval.value.integer = OP_SUB_ASSIGN; return SUB_ASSIGN; }
"*="                { yylval.value.integer = OP_MUL_ASSIGN; return MUL_ASSIGN; }
"/="                { yylval.value.integer = OP_DIV_ASSIGN; return DIV_ASSIGN; }
"%="                { yylval.value.integer = OP_MOD_ASSIGN; return MOD_ASSIGN; }
"^="                { yylval.value.integer = OP_EXP_ASSIGN; return EXP_ASSIGN; }
"||"                { yylval.value.integer = OP_OR; return OR; }
"&&"                { yylval.value.integer = OP_AND; return AND; }
"!"                 { yylval.value.integer = OP_NOT; return NOT; }
"=="                { yylval.value.integer = OP_EQUAL; return EQ; }
"!="                { yylval.value.integer = OP_NOT_EQUAL; return NE; }
"<"                 { yylval.value.integer = OP_LESS; return LESS; }
"<="                { yylval.value.integer = OP_LE; return LE; }
">"                 { yylval.value.integer = OP_GREAT; return GREAT; }
">="                { yylval.value.integer = OP_GE; return GE; }
"->"                { return ARROW; }
"..."               { return ELLIPSIS; }

"("                 { return LPAREN; }
")"                 { return RPAREN; }
"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
"["                 { return LBRACKET; }
"]"                 { return RBRACKET; }
";"                 { return SEMICOLON; }
","                 { return COMMA; }
"="                 { return ASSIGN; }
"&"                 { return AMPERSAND; }

{IDENTIFIER}        { 
                        if (lookup(yytext) == NULL) {
                            insert(yytext, strlen(yytext), UNDEF, yylineno);
                        }

                        yylval.item = lookup(yytext);

                        return IDENTIFIER;
                    }

{INTEGER}           { yylval.value.integer = atoi(yytext); return INTEGER; }
{FLOAT}             { yylval.value.real = atof(yytext); return FLOAT; }
{CHARACTER}         { yylval.value.character = yytext[1]; return CHARACTER; }
{STRING}            { yylval.value.string = strdup(yytext); return STRING; }

\n                  { yylineno++; }
{WHITESPACE}        { }

.                   { fprintf(stderr, "Unrecognized character at line %d: %s\n", yylineno, yytext); exit(1); }

%%