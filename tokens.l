%{
    #include <stdio.h>
    #include <string.h>

%}

DIGIT   [0-9]
IDEN    [a-zA-Z_]
HEX     [a-fA-F0-9]
EXP     [Ee][+-]?{DIGIT}+

%%

"/*"                        { comment(); }
"break"			            { count(); return(BREAK); }
"char"			            { count(); return(CHAR); }
"continue"		            { count(); return(CONTINUE); }
"do"			            { count(); return(DO); }
"double"		            { count(); return(DOUBLE); }
"else"			            { count(); return(ELSE); }
"float"			            { count(); return(FLOAT); }
"for"			            { count(); return(FOR); }
"if"			            { count(); return(IF); }
"int"			            { count(); return(INT); }
"long"			            { count(); return(LONG); }
"return"		            { count(); return(RETURN); }
"sizeof"		            { count(); return(SIZEOF); }
"struct"		            { count(); return(STRUCT); }
"void"			            { count(); return(VOID); }
"while"			            { count(); return(WHILE); }

{IDEN}({IDEN}|{DIGIT})*     { count(); return(IDENTIFIER); }

0[xX]{HEX}+                 { count(); return(CONSTANT); }
{DIGIT}+                    { count(); return(CONSTANT); }
\'(.|[^'])+\'               { count(); return(CONSTANT); }
{DIGIT}+{EXP}               { count(); return(CONSTANT); }
{DIGIT}*"."{DIGIT}+({EXP})? { count(); return(CONSTANT); }
{DIGIT}+"."{DIGIT}*({EXP})? { count(); return(CONSTANT); }

\"(.|[^"])*\"               { count(); return(STRING_LITERAL); }

">>="			{ count(); return(RIGHT_ASSIGN); }
"<<="			{ count(); return(LEFT_ASSIGN); }
"+="			{ count(); return(ADD_ASSIGN); }
"-="			{ count(); return(SUB_ASSIGN); }
"*="			{ count(); return(MUL_ASSIGN); }
"/="			{ count(); return(DIV_ASSIGN); }
"%="			{ count(); return(MOD_ASSIGN); }
"&="			{ count(); return(AND_ASSIGN); }
"^="			{ count(); return(XOR_ASSIGN); }
"|="			{ count(); return(OR_ASSIGN); }
">>"			{ count(); return(RIGHT_OP); }
"<<"			{ count(); return(LEFT_OP); }
"++"			{ count(); return(INC_OP); }
"--"			{ count(); return(DEC_OP); }
"->"			{ count(); return(PTR_OP); }
"&&"			{ count(); return(AND_OP); }
"||"			{ count(); return(OR_OP); }
"<="			{ count(); return(LE_OP); }
">="			{ count(); return(GE_OP); }
"=="			{ count(); return(EQ_OP); }
"!="			{ count(); return(NE_OP); }
";"			    { count(); return(';'); }
("{"|"<%")		{ count(); return('{'); }
("}"|"%>")		{ count(); return('}'); }
","			    { count(); return(','); }
":"			    { count(); return(':'); }
"="			    { count(); return('='); }
"("			    { count(); return('('); }
")"			    { count(); return(')'); }
("["|"<:")		{ count(); return('['); }
("]"|":>")		{ count(); return(']'); }
"."			    { count(); return('.'); }
"&"			    { count(); return('&'); }
"!"			    { count(); return('!'); }
"~"			    { count(); return('~'); }
"-"			    { count(); return('-'); }
"+"			    { count(); return('+'); }
"*"			    { count(); return('*'); }
"/"			    { count(); return('/'); }
"%"			    { count(); return('%'); }
"<"			    { count(); return('<'); }
">"			    { count(); return('>'); }
"^"			    { count(); return('^'); }
"|"			    { count(); return('|'); }
"?"			    { count(); return('?'); }

[ \t\v\n\f]		{ count(); }
.			    {  }


%%

void comment() {
    char curr, next;
    int parsed = 0;
    while (parsed == 0) {
        while ((curr = input()) != '*' && curr != 0) {
            putchar(curr);  
        }
        if ((next = input()) != '/' && curr != 0) {
            unput(curr);
            continue;
        } else if (curr != 0) {
            parsed = 1;
            putchar(next);
        }
    }

}

int column = 0;

void count() {
    for (int i = 0; i < strlen(yytext); i++) {
        if (yytext[i] == '\n') {
            column = 0;
        } else if (yytext[i] == '\t') {
            column += 8 - (column % 8);
        } else {
            column++;
        }
    }
}