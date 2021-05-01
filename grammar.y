%{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include "operators.h"

	extern char* yytext;
	extern int column;

	int  yylex(void);
	void yyerror (char  *a);

	FunctionDefinition* curr_function = NULL;
	SymbolTable* global_sym_table = NULL;
	int curr_scope = 0;
	int curr_type;
	int curr_if_label = 0;
	int curr_loop_label = 0;
	char* final_code;


%}

%union {
	char* str;
	int type;
	int arr_depth;
	struct ConstantValues* const_val;
	struct SymbolTable* table_entry;
	struct Parameters* parameters;
	struct FunctionDefinition* func_entry;
	struct VariableDeclaration* var_entry;
	struct Expression* expr;
}

%token <str> IDENTIFIER 
%token <const_val> CONSTANT STRING_LITERAL 
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN

%token CHAR INT FLOAT VOID

%token IF ELSE WHILE DO FOR CONTINUE BREAK RETURN

%type <type> type_specifier
%type <arr_depth> array_index
%type <func_entry> function_declarator
%type <parameters> parameter_list parameter_declaration argument_expression_list
%type <var_entry> direct_declarator declarator init_declarator init_declarator_list declaration

%type <expr> primary_expression postfix_expression unary_expression cast_expression 
%type <expr> multiplicative_expression additive_expression shift_expression relational_expression equality_expression
%type <expr> and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <expr> assignment_expression expression expression_statement

%type <str> statement compound_statement selection_statement iteration_statement jump_statement
%type <str> block_item block_item_list
%type <str> function_definition external_declaration translation_unit

%start start_state

%nonassoc "then"
%nonassoc ELSE


%%

primary_expression: 
      IDENTIFIER	{	struct SymbolTable* ptr = find_symbol($1, curr_scope);
	  					if(ptr == NULL) {
							print_error("No Such Vairable declared");	  
						}
						if(ptr->type == INT_TYPE || ptr->type == FLOAT_TYPE) {
							if(ptr->scope != 0) {
								char* addr = get_location_from_offset(ptr->addr_offset);
								$$ = create_expression(addr, ptr->type, ptr);
							} else {
								$$ = create_expression(ptr->name, ptr->type, ptr);
							}
						} else {
							$$ = create_expression(ptr->name, ptr->type, ptr);
						}
	  				}
	| CONSTANT	{	
					char* addr = strdup($1->data_section_name);
					$$ = create_expression(addr, $1->type, NULL);
					$$->const_val = $1;	
				}
	| STRING_LITERAL	{	char* addr = strdup($1->data_section_name);   
							$$ = create_expression(addr, $1->type, NULL);
						}
	| '(' expression ')'	{ $$ = $2; }
	;

postfix_expression: 
      primary_expression	{ $$ = $1; }
	| postfix_expression '[' expression ']'	{	$$ = prepare_array($1, $3);	}
	| primary_expression '(' ')'	{	struct Expression* func_call = malloc(sizeof(struct Expression));
										func_call->code = prepare_calling($1, NULL);
										if(strcmp($1->sym_entry->func->name, "print") != 0 && strcmp($1->sym_entry->func->name, "scan") != 0) {
											func_call->result_type = $1->sym_entry->func->return_type;
											// The value was just stored at the top of stack
											func_call->result_location = get_location_from_offset(curr_function->curr_offset);
										}
										func_call->sym_entry = $1->sym_entry;
										$$ = func_call;
									}
	| primary_expression '(' argument_expression_list ')'	{	struct Expression* func_call = malloc(sizeof(struct Expression));
																func_call->code = prepare_calling($1, $3);
																if(strcmp($1->sym_entry->func->name, "print") != 0 && strcmp($1->sym_entry->func->name, "scan") != 0) {
																	func_call->result_type = $1->sym_entry->func->return_type;
																	// The value was just stored at the top of stack
																	func_call->result_location = get_location_from_offset(curr_function->curr_offset);
																}
																func_call->sym_entry = $1->sym_entry;
																$$ = func_call;
															}
	| primary_expression INC_OP	{	$$ = $1;
									$$->code = increment_operator($1);
								}
	| primary_expression DEC_OP	{ 	$$ = $1; 
									$$->code = decrement_operator($1);
								}
	;

argument_expression_list: 
      assignment_expression	{	struct Parameters* temp = malloc(sizeof(struct Parameters));
		  						$$ = temp;
	  							$$->code = strdup($1->code);
								$$->initialise_location = strdup($1->result_location);
								$$->curr_depth = $1->curr_depth;
								if($1->sym_entry != NULL) {
									$$->arr_depth = $1->sym_entry->arr_depth;
								}
								$$->type = $1->result_type;
								$$->param_count = 1;  
							}
	| argument_expression_list ',' assignment_expression	{	struct Parameters* temp = malloc(sizeof(struct Parameters));
																temp->code = strdup($3->code);
																temp->initialise_location = strdup($3->result_location);
																temp->curr_depth = $3->curr_depth;
																if($3->sym_entry != NULL) {
																	temp->arr_depth = $3->sym_entry->arr_depth;
																}
																temp->type = $3->result_type;
																temp->param_count = $1->param_count + 1;
																
																temp->next = $1;
																$1 = temp;
																$$ = $1;
															}
	;

unary_expression: 
      postfix_expression	{ $$ = $1; }
	| INC_OP primary_expression	{	$$ = $2; $$->code = increment_operator($2);	}
	| DEC_OP primary_expression	{	$$ = $2; $$->code = decrement_operator($2); }
	;

cast_expression: 
      unary_expression	{	$$ = $1; }
	| '(' type_specifier ')' cast_expression	{	$$ = $4; 
													$$->code = strcat($4->code, change_type($4, $2));
													$$->result_type = $2;
													$$->result_location = get_location_from_offset(curr_function->curr_offset);
													$$->curr_depth = 0;
													$$->sym_entry = NULL;
												}
	| '-' unary_expression	{	$$ = $2; 
								$$->code = minus_operator($2);
								$$->result_type = $2->result_type;
								$$->result_location = get_location_from_offset(curr_function->curr_offset);
								$$->curr_depth = 0;
								$$->sym_entry = NULL;
							}
	| '!' unary_expression	{	$$ = $2; 
								$$->code = not_operator($2);
								$$->result_type = $2->result_type;
								$$->result_location = get_location_from_offset(curr_function->curr_offset);
								$$->curr_depth = 0;
								$$->sym_entry = NULL;
							}
	;

multiplicative_expression: 
      cast_expression	{	$$ = $1; }
	| multiplicative_expression '*' cast_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 0);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = implicit_type_conversion($1->result_type, $3->result_type);
														exp->curr_depth = 0;
														$$ = exp; 	
													}
	| multiplicative_expression '/' cast_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 1);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = implicit_type_conversion($1->result_type, $3->result_type);
														exp->curr_depth = 0;
														$$ = exp; 	
													}
	| multiplicative_expression '%' cast_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 2);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = implicit_type_conversion($1->result_type, $3->result_type);
														exp->curr_depth = 0; 
														$$ = exp;	
													}
	;

additive_expression: 
      multiplicative_expression
	| additive_expression '+' multiplicative_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 3);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = implicit_type_conversion($1->result_type, $3->result_type);
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	| additive_expression '-' multiplicative_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 4);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = implicit_type_conversion($1->result_type, $3->result_type);
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	;

shift_expression: 
      additive_expression
	| shift_expression LEFT_OP additive_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 5);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = implicit_type_conversion($1->result_type, $3->result_type);
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	| shift_expression RIGHT_OP additive_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 6);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = implicit_type_conversion($1->result_type, $3->result_type);
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	;

relational_expression: 
      shift_expression
	| relational_expression '<' shift_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 7);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = INT_TYPE;
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	| relational_expression '>' shift_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 8);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = INT_TYPE;
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	| relational_expression LE_OP shift_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 9);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = INT_TYPE;
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	| relational_expression GE_OP shift_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 10);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = INT_TYPE;
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	;

equality_expression: 
      relational_expression
	| equality_expression EQ_OP relational_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
															exp->code = binary_op($1, $3, 11);
															exp->result_location = get_location_from_offset(curr_function->curr_offset);
															exp->result_type = INT_TYPE;
															exp->curr_depth = 0; 
															$$ = exp; 	
														}
	| equality_expression NE_OP relational_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
															exp->code = binary_op($1, $3, 12);
															exp->result_location = get_location_from_offset(curr_function->curr_offset);
															exp->result_type = INT_TYPE;
															exp->curr_depth = 0; 
															$$ = exp; 	
														}
	;

and_expression: 
      equality_expression
	| and_expression '&' equality_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
													exp->code = binary_op($1, $3, 13);
													exp->result_location = get_location_from_offset(curr_function->curr_offset);
													exp->result_type = INT_TYPE;
													exp->curr_depth = 0; 
													$$ = exp; 	
												}
	;

exclusive_or_expression: 
      and_expression
	| exclusive_or_expression '^' and_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = binary_op($1, $3, 14);
														exp->result_location = get_location_from_offset(curr_function->curr_offset);
														exp->result_type = INT_TYPE;
														exp->curr_depth = 0; 
														$$ = exp; 	
													}
	;

inclusive_or_expression: 
      exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
																exp->code = binary_op($1, $3, 15);
																exp->result_location = get_location_from_offset(curr_function->curr_offset);
																exp->result_type = INT_TYPE;
																exp->curr_depth = 0; 
																$$ = exp; 	
															}
	;

logical_and_expression: 
      inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
																exp->code = binary_op($1, $3, 13);
																exp->result_location = get_location_from_offset(curr_function->curr_offset);
																exp->result_type = INT_TYPE;
																exp->curr_depth = 0; 
																$$ = exp; 	
															}
	;

logical_or_expression: 
      logical_and_expression
	| logical_or_expression OR_OP logical_and_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
																exp->code = binary_op($1, $3, 15);
																exp->result_location = get_location_from_offset(curr_function->curr_offset);
																exp->result_type = INT_TYPE;
																exp->curr_depth = 0; 
																$$ = exp; 	
															}
	;

assignment_expression: 
      logical_or_expression
	| unary_expression '=' logical_or_expression	{	struct Expression* exp = malloc(sizeof(struct Expression));
														exp->code = assignment_op($1, $3);
														exp->result_location = $1->result_location;
														exp->result_type = $1->result_type;
														$$ = exp;
													}
	;

expression: 
      assignment_expression
	;

declaration: 
	  type_specifier init_declarator_list ';'	{ $$ = $2; }
	;

init_declarator_list: 
      init_declarator { $$ = $1; }
	| init_declarator_list ',' init_declarator { $3->next = $1; $1 = $3; $$ = $1; }
	;

init_declarator: 
      declarator	{ assign_types($1, curr_type); assign_address($1); $$ = $1; }
	| declarator '=' logical_or_expression	{	assign_types($1, curr_type); assign_address($1); initialise_variable($1, $3); $$ = $1;	}
	;

type_specifier: 
      VOID	{ $$ = VOID_TYPE; curr_type = $$; }
	| CHAR	{ $$ = CHAR_TYPE; curr_type = $$; }
	| INT	{ $$ = INT_TYPE; curr_type = $$; }
	| FLOAT	{ $$ = FLOAT_TYPE; curr_type = $$; }
	;

declarator: 
	  direct_declarator	{ $$ = $1; }
	;

direct_declarator: 
      IDENTIFIER	{  $$ = create_new_variable($1, curr_scope);  }
	| '(' declarator ')'	{ $$ = $2; }
	| direct_declarator '[' CONSTANT ']' {	$$ = $1;
											$$->table_entry->arr_depth = $1->table_entry->arr_depth + 1; 
											if($3->type != INT_TYPE) { 
												print_error("Array size should be integer"); 
											}
											$$->table_entry->total_arr_size = $1->table_entry->total_arr_size*$3->const_int;
											$$->table_entry->arr_size[$$->table_entry->arr_depth] = $3->const_int;    
										 }
	;

function_declarator:
	  IDENTIFIER '(' parameter_list ')'	{ $$ = create_function($1, $3); }
	| IDENTIFIER '(' ')'	{	$$ = create_function($1, NULL); }
	;

parameter_list: 
      parameter_declaration	{	$$ = $1; }
	| parameter_list ',' parameter_declaration	{	int c = $1->param_count + 1; $3->next = $1; $1 = $3; $$ = $1; $$->param_count = c; }
	;

parameter_declaration: 
      type_specifier IDENTIFIER	{	$$ = create_parameter($2, $1, 0); }
	| type_specifier IDENTIFIER array_index	{	$$ = create_parameter($2, $1, $3);	}
	;

array_index:
	  '[' ']'	{	$$ = 1;	}
	| array_index '[' ']'	{	$$ = $1 + 1;	}
	;

statement:
	  compound_statement
	| expression_statement	{ $$ = $1->code; }
	| selection_statement
	| iteration_statement
	| jump_statement
	;

block_item_list: 
	  block_item	{	$$ = $1; }
	| block_item_list block_item	{	char* temp = malloc(CODE_SIZE); strcat(temp, $1); strcat(temp, $2); 	$$ = temp; }
	;

block_item: 
	  declaration	{ $$ = $1->initialiser_code; }
	| statement	{	$$ = $1; }
	;

compound_statement: 
      '{' '}'	{	$$ = ""; }
	| '{' { curr_scope++; } block_item_list '}'	{ curr_scope--; }	{	$$ = $3; }
	;

expression_statement: 
      ';'	{	struct Expression* exp = malloc(sizeof(struct Expression));	exp->code = ""; $$ = exp; }
	| expression ';'	{	$$ = $1; }
	;

selection_statement: 
      IF '(' expression ')' statement               %prec "then"	{	$$ = if_construct($3, $5);	}
	| IF '(' expression ')' statement ELSE statement	{	$$ = if_else_construct($3, $5, $7);	}
	;

iteration_statement: 
      WHILE '(' expression ')' statement	{	$$ = while_construct($3, $5);	}
	| DO statement WHILE '(' expression ')' ';'	{	$$ = do_while_construct($5, $2);	}
	| FOR '(' expression_statement expression_statement ')'	 statement	{	$$ = for_construct($3, $4, NULL, $6);	}
	| FOR '(' expression_statement expression_statement expression ')' statement	{	$$ = for_construct($3, $4, $5, $7);	}
	;

jump_statement: 
	  CONTINUE ';'	{	char* temp = malloc(100); sprintf(temp, "\tb __start_loop_label%d\n", curr_loop_label); $$ = temp;	}
	| BREAK ';'		{	char* temp = malloc(100); sprintf(temp, "\tb __end_loop_label%d\n", curr_loop_label); $$ = temp;	}
	| RETURN ';'	{	if(curr_function->return_type != VOID_TYPE) {
							print_error("Function must return a value");
						}
						$$ = function_return(NULL, 0);
					}
	| RETURN expression ';'	{	if(curr_function->return_type == VOID_TYPE) {
									print_error("Void function cannot return value");
								}
								$$ = function_return($2, curr_function->return_type);
							} 
	;

start_state:
	  translation_unit	{ final_code = strdup($1); }
	;

translation_unit: 
      external_declaration	{	$$ = strdup($1); }
	| translation_unit external_declaration	{	char* temp = malloc(CODE_SIZE); strcat(temp, $1); strcat(temp, $2); $$ = temp; 	}
	;

external_declaration: 
      function_definition	{ $$ = $1; }
	| declaration	{	$$ = $1->initialiser_code;	}
	;

function_definition: 
	   type_specifier function_declarator {	$2->return_type = $1; curr_function = $2; curr_scope = 0; } compound_statement	
	   	{	char* temp = malloc(CODE_SIZE); 
	   		sprintf(temp, "%s:\n", $2->name);
			char* temp1 = malloc(100);
			sprintf(temp1, "\taddu $sp, $sp, %d\n", $2->curr_offset);
			strcat(temp, temp1);
			if(strcmp($2->name, "main") != 0) {
				strcat(temp, "\tsw $ra, -4($fp)\n"); 
				strcat(temp, $4);
			} else {
				strcat(temp, "\tmove $fp, $sp\n");
				strcat(temp, $4);
				// Exit the program after main is executed
				strcat(temp, "\tli $v0, 10\n\tsyscall\n");
			}
			$$ = temp;	
		}
	;


%%


int main() {
	// #ifdef YYDEBUG
	// 	yydebug = 1;
	// #endif
	// Add print and scan as functions
	create_function("print", NULL);
	create_function("scan", NULL);

    yyparse();
	char* temp = malloc(CODE_SIZE);
	strcat(temp, prepare_data_section());
	strcat(temp, "\t\t.text\n\t\t.globl main\n");
	strcat(temp, final_code);
	final_code = temp;
	// printf("\n%s\n", final_code);
	FILE* final_file = fopen("mips.asm", "w");
	fprintf(final_file, "%s", final_code);
	fclose(final_file);
}

void yyerror(char *s) {
    fflush(stdout);
    print_error("Syntax Error");
}

