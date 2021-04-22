%{
    #include "grammar.tab.h"
    #include "symbol_table.h"
    #include "error_reporting.h"

    void comment();
    void count();

    ConstantValues* data_section = NULL;

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
"else"			            { count(); return(ELSE); }
"float"			            { count(); return(FLOAT); }
"for"			            { count(); return(FOR); }
"if"			            { count(); return(IF); }
"int"			            { count(); return(INT); }
"return"		            { count(); return(RETURN); }
"sizeof"		            { count(); return(SIZEOF); }
"void"			            { count(); return(VOID); }
"while"			            { count(); return(WHILE); }

{IDEN}({IDEN}|{DIGIT})*     {   count();    
                                yylval.str = strdup(yytext);
                                return(IDENTIFIER); 
                            }

0[xX]{HEX}+                 {   count();
                                int hex_val;
                                sscanf(yytext, "%x", &hex_val);
                                ConstantValues* node = malloc(sizeof(ConstantValues));
                                node->data_section_name = NULL;
                                node->type = INT_TYPE;
                                node->const_int = hex_val;
                                ConstantValues* ptr = find_const_val(node);
                                if(ptr == NULL) {
                                    ptr = insert_const_val(node);
                                }
                                yylval.const_val = ptr;
                                return(CONSTANT); 
                            }
{DIGIT}+                    {   count();
                                int int_val;
                                sscanf(yytext, "%d", &int_val);
                                ConstantValues* node = malloc(sizeof(ConstantValues));
                                node->data_section_name = NULL;
                                node->type = INT_TYPE;
                                node->const_int = int_val;
                                ConstantValues* ptr = find_const_val(node);
                                if(ptr == NULL) {
                                    ptr = insert_const_val(node);
                                }
                                yylval.const_val = ptr;
                                return(CONSTANT);
                            }
\'([^'])\'                  {   count();
                                char char_val;
                                sscanf(yytext, "%c", &char_val);
                                ConstantValues* node = malloc(sizeof(ConstantValues));
                                node->data_section_name = NULL;
                                node->type = CHAR_TYPE;
                                node->const_char = char_val;
                                ConstantValues* ptr = find_const_val(node);
                                if(ptr == NULL) {
                                    ptr = insert_const_val(node);
                                }
                                yylval.const_val = ptr;
                                return(CONSTANT); 
                            }
{DIGIT}+{EXP}               {   count(); 
                                float float_val;
                                sscanf(yytext, "%e", &float_val);
                                ConstantValues* node = malloc(sizeof(ConstantValues));
                                node->data_section_name = NULL;
                                node->type = FLOAT_TYPE;
                                node->const_float = float_val;
                                ConstantValues* ptr = find_const_val(node);
                                if(ptr == NULL) {
                                    ptr = insert_const_val(node);
                                }
                                yylval.const_val = ptr;
                                return(CONSTANT); 
                            }
{DIGIT}*"."{DIGIT}+({EXP})? {   count(); 
                                float float_val;
                                sscanf(yytext, "%e", &float_val);
                                ConstantValues* node = malloc(sizeof(ConstantValues));
                                node->data_section_name = NULL;
                                node->type = FLOAT_TYPE;
                                node->const_float = float_val;
                                ConstantValues* ptr = find_const_val(node);
                                if(ptr == NULL) {
                                    ptr = insert_const_val(node);
                                }
                                yylval.const_val = ptr;
                                return(CONSTANT); 
                            }
{DIGIT}+"."{DIGIT}*({EXP})? {   count(); 
                                float float_val;
                                sscanf(yytext, "%e", &float_val);
                                ConstantValues* node = malloc(sizeof(ConstantValues));
                                node->data_section_name = NULL;
                                node->type = FLOAT_TYPE;
                                node->const_float = float_val;
                                ConstantValues* ptr = find_const_val(node);
                                if(ptr == NULL) {
                                    ptr = insert_const_val(node);
                                }
                                yylval.const_val = ptr;
                                return(CONSTANT); 
                            }

\"([^"])*\"                 {   count(); 
                                char* string_literal = strdup(yytext);
                                ConstantValues* node = malloc(sizeof(ConstantValues));
                                node->data_section_name = NULL;
                                node->type = STRING_TYPE;
                                node->string_literal = strdup(string_literal);
                                ConstantValues* ptr = find_const_val(node);
                                if(ptr == NULL) {
                                    ptr = insert_const_val(node);
                                }
                                yylval.const_val = ptr;
                                return(STRING_LITERAL); 
                            }

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
    int c;

    while ((c = input()) != 0) {
        if (c == '*') {
            while ((c = input()) == '*')
                ;
            if (c == '/') 
                return;

            if (c == 0)
                break;
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
    ECHO;
}