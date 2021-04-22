#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum var_type {
    VOID_TYPE,
    CHAR_TYPE,
    INT_TYPE, 
    FLOAT_TYPE,
    STRING_TYPE,
    FUNC_TYPE
};

typedef enum var_type var_type;

typedef struct SymbolTable {

    char* name;
    int size;
    int addr_offset;
    int scope;
    int arr_depth;
    int arr_size;
    struct FunctionDefinition* func;
    var_type type;
    struct SymbolTable* next;

} SymbolTable;


typedef struct Parameters {
    char* name;
    int size;
    var_type type;
    struct Parameters* next;
} Parameters;


typedef struct FunctionDefinition {

    char* name;
    var_type return_type;
    int curr_offset;
    struct Parameters* params;
    struct SymbolTable* sym_table;  // Contains all variables in the scope of the function
 
} FunctionDefinition;

typedef struct VariableDeclaration {

    SymbolTable* table_entry;
    char* initialiser_code;
    struct VariableDeclaration* next;

} VariableDeclaration;


typedef struct ConstantValues {

    int const_int;
	float const_float;
	char const_char;
    char* string_literal;
    var_type type;
    char* data_section_name;
    struct ConstantValues* next;

} ConstantValues;


typedef struct Expression {

    char* code;
    var_type result_type;
    int result_offset;

} Expression;


extern FunctionDefinition* curr_function;
extern SymbolTable* global_sym_table;
extern ConstantValues* data_section;


ConstantValues* find_const_val(ConstantValues* s);
ConstantValues* insert_const_val(ConstantValues* s);

Parameters* create_parameter(char* name, int type);

FunctionDefinition* create_function(char* name, Parameters* params);

VariableDeclaration* create_new_variable(char* name, int scope);

void assign_address(VariableDeclaration* var);

SymbolTable* find_symbol(char* name, int scope);
SymbolTable* find_symbol_curr_scope(char* name, int scope);


