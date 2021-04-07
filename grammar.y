%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN

%token CHAR INT LONG FLOAT DOUBLE VOID
%token STRUCT

%token IF ELSE WHILE DO FOR CONTINUE BREAK RETURN


%%



%%


#include <stdio.h>

extern char yytext[];
extern int column;

void yyerror(char *s) {
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);
}

