#include "symbol_table.h"
#include "error_reporting.h"

Parameters* create_parameter(char* name, int type) {

    Parameters* new_param = malloc(sizeof(Parameters));
    if(new_param == NULL) { print_error("Could not allocate parameter"); }
    new_param->name = strdup(name);
    new_param->size = 4;
    new_param->type = type;
    new_param->next = NULL;
    return new_param;

}

FunctionDefinition* create_function(char* name, Parameters* params) {

    SymbolTable* found = find_symbol_curr_scope(name, 0);
    if(found != NULL) {
        print_error("Function has already been declared");
    }
    FunctionDefinition* new_func = malloc(sizeof(FunctionDefinition));
    if(new_func == NULL) { print_error("Could not create function"); }
    new_func->name = strdup(name);
    new_func->params = params;
    new_func->curr_offset = 0;
    // Add all the parameters in the Symbol Table
    new_func->sym_table = NULL;
    int parameter_offset = 4;   // Original $fp is stored at offset 0
    Parameters* ptr = params;
    while (ptr != NULL) {
        SymbolTable* new_symbol = malloc(sizeof(SymbolTable));
        if(new_symbol == NULL) { print_error("Could not insert in Symbol table"); }
        new_symbol->name = strdup(ptr->name);
        new_symbol->size = ptr->size;
        new_symbol->scope = 1;  // The parameters are visible throughout the function scope
        new_symbol->type = ptr->type;
        new_symbol->addr_offset = parameter_offset;

        // Inserting in the synbol table
        new_symbol->next = new_func->sym_table;
        new_func->sym_table = new_symbol;

        parameter_offset += ptr->size;
        ptr = ptr->next;
    }

    SymbolTable* function_entry = malloc(sizeof(SymbolTable));
    if(function_entry == NULL) { print_error("Could not insert in Symbol table"); }
    function_entry->type = FUNC_TYPE;
    function_entry->name = strdup(name);
    function_entry->func = new_func;

    function_entry->next = global_sym_table;
    global_sym_table = function_entry;

    return new_func;

}

SymbolTable* find_symbol(char* name, int scope) {

    SymbolTable* ptr = curr_function->sym_table;
    SymbolTable* ans = NULL;
    int max_scope = 0;
    while(ptr != NULL) {
        // The variable should be in scope above the current scope
        // Also, this will give the variable with highest scope i.e. most local
        if(strcmp(ptr->name, name) == 0 && ptr->scope <= scope && ptr->scope >= max_scope) { 
            ans = ptr;
            max_scope = ptr->scope;
        }
        ptr = ptr->next;
    }
    if(ans == NULL) {
        // Look in the GlobalSymTable
        ptr = global_sym_table;
        while (ptr != NULL) {
            if(strcmp(ptr->name, name) == 0) {
                ans = ptr;
                break;
            }
            ptr = ptr->next;
        }
    }
    return ans;

}

SymbolTable* find_symbol_curr_scope(char* name, int scope) {
    SymbolTable* ptr;
    if(scope == 0) {
        ptr = global_sym_table;
    } else {
        ptr = curr_function->sym_table;
    }
    SymbolTable* ans = NULL;
    while(ptr != NULL) {
        if(strcmp(ptr->name, name) == 0 && ptr->scope == scope) {
            ans = ptr;
            break;
        }
        ptr = ptr->next;
    }
    return ans;
}


VariableDeclaration* create_new_variable(char* name, int scope) {

    SymbolTable* found = find_symbol_curr_scope(name, scope);
    if(found != NULL) {
        print_error("Variable has already been declared");
    }
    VariableDeclaration* new_var = malloc(sizeof(VariableDeclaration));
    if(new_var == NULL) { print_error("Could not allocate variable"); }
    SymbolTable* new_symbol = malloc(sizeof(SymbolTable));
    if(new_symbol == NULL) { print_error("Could not insert in Symbol table"); }
    new_symbol->name = strdup(name);
    new_symbol->scope = scope; 
    new_symbol->arr_size = 1;
    new_symbol->arr_depth = 0;

    if(scope != 0) {
        // Adding entry in the symbol table
        new_symbol->next = curr_function->sym_table;
        curr_function->sym_table = new_symbol;
    } else {
        new_symbol->next = global_sym_table;
        global_sym_table = new_symbol;
    }
    
    new_var->table_entry = new_symbol;
    new_var->initialiser_code = "";
    new_var->next = NULL;
    
    return new_var;

}

void assign_address(VariableDeclaration* var) {
    if(var->table_entry->arr_depth > 0) {
        curr_function->curr_offset = curr_function->curr_offset - var->table_entry->arr_size*4;    // Allocate 4 bytes of array size on the stack
    } else {
        curr_function->curr_offset = curr_function->curr_offset - 4; 
    }
    var->table_entry->addr_offset = curr_function->curr_offset;
}


ConstantValues* find_const_val(ConstantValues* s) {
    ConstantValues* ptr = data_section;
    while (ptr != NULL) {
        if(s->type == ptr->type) {
            switch (s->type) {
                case INT_TYPE:
                    if(s->const_int == ptr->const_int) {
                        return ptr;
                    }
                    break;
                case FLOAT_TYPE:
                    if(s->const_float == ptr->const_float) {
                        return ptr;
                    }
                    break;
                case CHAR_TYPE:
                    if(s->const_char == ptr->const_char) {
                        return ptr;
                    }
                    break;
                case STRING_TYPE:
                    if(strcmp(s->string_literal, ptr->string_literal) == 0) {
                        return ptr;
                    }
                    break;
            }
        }
        ptr = ptr->next;
    }
    return ptr;

}

int current_entry = 0;

ConstantValues* insert_const_val(ConstantValues* s) {

    // Name of entry in .data section
    s->data_section_name = malloc(sizeof(100));
    sprintf(s->data_section_name, "ConstVal%d", current_entry);
    current_entry++;
    s->next = data_section;
    data_section = s;
    return s;

}


Expression* create_expression() {

}
