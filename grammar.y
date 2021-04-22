%{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include "symbol_table.h"
	#include "error_reporting.h"

	extern char* yytext;
	extern int column;

	int  yylex(void);
	void yyerror (char  *a);

	FunctionDefinition* curr_function = NULL;
	SymbolTable* global_sym_table = NULL;
	int curr_scope = 0;

%}

%union {
	char* str;
	int type;
	struct ConstantValues* const_val;
	struct SymbolTable* table_entry;
	struct Parameters* parameters;
	struct FunctionDefinition* func_entry;
	struct VariableDeclaration* var_entry;
	struct Expression* exp;
}

%token <str> IDENTIFIER 
%token <const_val> CONSTANT STRING_LITERAL 
%token SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN

%token CHAR INT FLOAT VOID

%token IF ELSE WHILE DO FOR CONTINUE BREAK RETURN

%type <type> type_specifier
%type <func_entry> function_declarator
%type <parameters> parameter_list parameter_declaration
%type <var_entry> direct_declarator declarator init_declarator init_declarator_list declaration
%type <exp> primary_expression postfix_expression argument_expression_list unary_expression cast_expression 
%type <exp> multiplicative_expression additive_expression shift_expression relational_expression equality_expression
%type <exp> and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <exp> conditional_expression assignment_expression expression expression_statement


%start translation_unit

%nonassoc "then"
%nonassoc ELSE


%%

primary_expression: 
      IDENTIFIER	{	struct SymbolTable* ptr = find_symbol($1, curr_scope);
	  					if(ptr == NULL) {
							print_error("No Such Vairable declared");	  
						}
						  

	  				}
	| CONSTANT	{  }
	| STRING_LITERAL	{  }
	| '(' expression ')'
	;

postfix_expression: 
      primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

argument_expression_list: 
      assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression: 
      postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_specifier ')'
	;

unary_operator: 
      '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression: 
      unary_expression
	| '(' type_specifier ')' cast_expression
	;

multiplicative_expression: 
      cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression: 
      multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression: 
      additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression: 
      shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression: 
      relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression: 
      equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression: 
      and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression: 
      exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression: 
      inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression: 
      logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression: 
      logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression: 
      conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator: 
      '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression: 
      assignment_expression
	;

declaration: 
	  type_specifier init_declarator_list ';'	{ /* Assign types to all the declarators */ $$ = $2; }
	;

init_declarator_list: 
      init_declarator { $$ = $1; }
	| init_declarator_list ',' init_declarator { $3->next = $1; $1 = $3; $$ = $1; }
	;

init_declarator: 
      declarator	{ assign_address($1); $$ = $1; }
	| declarator '=' initializer
	;

type_specifier: 
      VOID	{ $$ = VOID_TYPE; }
	| CHAR	{ $$ = CHAR_TYPE; }
	| INT	{ $$ = INT_TYPE; }
	| FLOAT	{ $$ = FLOAT_TYPE; }
	;

declarator: 
	  direct_declarator	{ $$ = $1; }
	;

direct_declarator: 
      IDENTIFIER	{  $$ = create_new_variable($1, curr_scope);  }
	| '(' declarator ')'	{ $$ = $2; }
	| direct_declarator '[' CONSTANT ']' {	
											$$->table_entry->arr_depth = $1->table_entry->arr_depth + 1; 
											if($3->type != INT_TYPE) { 
												print_error("Array size should be integer"); 
											}
											$$->table_entry->arr_size = $1->table_entry->arr_size*$3->const_int;    
										 }
	| direct_declarator '[' ']'	{ $1->table_entry->arr_depth++; }
	;

function_declarator:
	  IDENTIFIER '(' parameter_list ')'	{ $$ = create_function($1, $3); }
	| IDENTIFIER '(' ')'	{	$$ = create_function($1, NULL); }
	;

parameter_list: 
      parameter_declaration	{	$$ = $1; }
	| parameter_list ',' parameter_declaration	{	$3->next = $1; $1 = $3; $$ = $1; }
	;

parameter_declaration: 
      type_specifier IDENTIFIER	{	$$ = create_parameter($2, $1); }
	;

initializer: 
	  '{' initializer_list '}'
	| '{' initializer_list ',' '}'
    |  assignment_expression
	;

initializer_list: 
      initializer
	| initializer_list ',' initializer
	;

statement:
	  compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

block_item_list: 
	  block_item
	| block_item_list block_item
	;

block_item: 
	  declaration
	| statement
	;

compound_statement: 
      '{' '}'
	| '{' { curr_scope++; } block_item_list '}'	{ curr_scope--; }
	;

expression_statement: 
      ';'
	| expression ';'
	;

selection_statement: 
      IF '(' expression ')' statement               %prec "then"
	| IF '(' expression ')' statement ELSE statement
	;

iteration_statement: 
      WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement: 
	  CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit: 
      external_declaration
	| translation_unit external_declaration
	;

external_declaration: 
      function_definition
	| declaration
	;

function_definition: 
	   type_specifier function_declarator {	$2->return_type = $1; curr_function = $2; curr_scope = 0; } compound_statement
	;


%%


int main() {
	// #ifdef YYDEBUG
	// 	yydebug = 1;
	// #endif
	// create_scope(); // Creating the global scope
    yyparse();
}

void yyerror(char *s) {
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);
}

