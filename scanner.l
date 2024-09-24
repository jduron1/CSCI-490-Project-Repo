%{
    #include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
    #include "symbol_table.h"
    #include "semantics.h"
    #include "ast.h"
	#include "parser.tab.h"

    extern int yylineno;
    extern FILE *yyin;
    extern FILE *yyout;
%}

%option noyywrap
%x MULTI_LINE

/* Keyword definitions */

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
ESPERA              (?i:espera)
CONTINUA            (?i:continua)
REGRESA             (?i:regresa)

/* Identifier definition */
IDENTIFIER          [a-zA-Z][a-zA-Z0-9_]*

/* Number constant definition */
INTEGER             [0-9]+
DECIMAL             [0-9]+(\.[0-9]+)

/* String literal definitions */
CHARACTER           \'[ -~]\'|\'\\[nftrbv]\'
STRING              \"[ -~]*\"

/* Comment definitions */
SINGLE_LINE         \#[^*].*

/* Whitespace definition */
WHITESPACE          [\ \f\r\t\v]+

%%

{SINGLE_LINE}       { /* Ignore comments */ }

"#*"                { BEGIN(MULTI_LINE); }
<MULTI_LINE>"*#"    { BEGIN(INITIAL); }
<MULTI_LINE>[^*\n]+ { /* Ignore comments */ }
<MULTI_LINE>"*"     { /* Ignore comments */ }
<MULTI_LINE>\n      { yylineno++; }

{CARACTER}          { return CARACTER; }
{BOOLEANO}          { return BOOLEANO; }
{ENTERO}            { return ENTERO; }
{REAL}              { return REAL; }
{CADENA}            { return CADENA; }
{VACIO}             { return VACIO; }
{FUNCION}           { return FUNCION; }
{CIERTO}            { return CIERTO; }
{FALSO}             { return FALSO; }
{SI}                { return SI; }
{SINO}              { return SINO; }
{POR}               { return POR; }
{EN}                { return EN; }
{MIENTRAS}          { return MIENTRAS; }
{ESPERA}            { return ESPERA; }
{CONTINUA}          { return CONTINUA; }
{REGRESA}           { return REGRESA; }

"+"                 { return ADD; }
"-"                 { return SUB; }
"*"                 { return MUL; }
"/"                 { return DIV; }
"%"                 { return MOD; }
"^"                 { return EXP; }
"+="                { return ADD_ASSIGN; }
"-="                { return SUB_ASSIGN; }
"*="                { return MUL_ASSIGN; }
"/="                { return DIV_ASSIGN; }
"%="                { return MOD_ASSIGN; }
"^="                { return EXP_ASSIGN; }
"||"                { return OR; }
"&&"                { return AND; }
"!"                 { return NOT; }
"=="|"!="           { return EQ; }
"<"|"<="|">"|">="   { return REL; }
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
                        insert(yytext, strlen(yytext), UNDEF, yylineno);
                        yylval.item = lookup(yytext);
                        return IDENTIFIER;
                    }

{INTEGER}           { yylval.var.integer = atoi(yytext); return INTEGER; }
{DECIMAL}           { yylval.var.real = atof(yytext); return DECIMAL; }
{CHARACTER}         { yylval.var.character = yytext[0]; return CHARACTER; }
{STRING}            { yylval.var.string = strdup(yytext); return STRING; }

\n                  { yylineno++; }
{WHITESPACE}        { /* Ignore whitespace */ }

.                   { fprintf(stderr, "Unrecognized character at line %d: %s\n", yylineno, yytext); exit(1); }

%%