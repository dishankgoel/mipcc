#include "symbol_table.h"

Parameters* create_parameter(char* name, int type, int arr_depth) {

    Parameters* new_param = malloc(sizeof(Parameters));
    if(new_param == NULL) { print_error("Could not allocate parameter"); }
    new_param->name = strdup(name);
    new_param->size = 4;
    new_param->type = type;
    new_param->arr_depth = arr_depth;
    new_param->param_count = 1;
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
    new_func->curr_offset = -4; // $ra will be stored at -4($fp)
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
        new_symbol->arr_depth = ptr->arr_depth;

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
    new_symbol->total_arr_size = 1;
    new_symbol->arr_size[0] = 0;
    new_symbol->arr_depth = 0;
    new_symbol->const_init = NULL;

    if(scope != 0) {
        // Adding entry in the local symbol table
        new_symbol->next = curr_function->sym_table;
        curr_function->sym_table = new_symbol;
    } else {
        new_symbol->next = global_sym_table;
        global_sym_table = new_symbol;
    }
    
    new_var->table_entry = new_symbol;
    new_var->initialiser_code = malloc(CODE_SIZE);
    new_var->next = NULL;
    
    return new_var;

}

void assign_types(VariableDeclaration* vars, var_type type) {
    vars->table_entry->type = type;
}

int allocate_on_stack(int size) {
    curr_function->curr_offset = curr_function->curr_offset - size;
    return curr_function->curr_offset;
}

void assign_address(VariableDeclaration* var) {
    // Assign local address only when variable is not global
    if(var->table_entry->scope != 0) {
        if(var->table_entry->arr_depth > 0) {
            // Allocate 4 bytes of each array element on the stack
            int addr_array = allocate_on_stack(var->table_entry->total_arr_size*4);   
            // Pointer to the array
            int addr_pointer = allocate_on_stack(4);
            sprintf(var->initialiser_code, "\tla $t0, %d($fp)\n\tsw $t0, %d($fp)\n", addr_array, addr_pointer);
            var->table_entry->addr_offset = addr_pointer;

        } else {
            int addr;
            addr = allocate_on_stack(4);
            var->table_entry->addr_offset = addr;
        }
    }
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
    s->data_section_name = malloc(CODE_SIZE*sizeof(char));
    sprintf(s->data_section_name, "_ConstVal%d", current_entry);
    current_entry++;
    s->next = data_section;
    data_section = s;
    return s;

}

char* prepare_data_section() {
    char* temp = malloc(CODE_SIZE);
    sprintf(temp, "\t\t.data\n");
    // Insert constant data
    ConstantValues* ptr = data_section;
    while (ptr != NULL) {
        char* temp1 = malloc(CODE_SIZE);
        if(ptr->type == INT_TYPE) {
            sprintf(temp1, "%s: .word %d\n", ptr->data_section_name, ptr->const_int);
        } else if(ptr->type == FLOAT_TYPE) {
            sprintf(temp1, "%s: .float %f\n", ptr->data_section_name, ptr->const_float);
        } else if(ptr->type == STRING_TYPE) {
            sprintf(temp1, "%s: .asciiz %s\n", ptr->data_section_name, ptr->string_literal);
        }
        strcat(temp, temp1);
        ptr = ptr->next;
    }
    // Insert global variables
    SymbolTable* sym = global_sym_table;
    while (sym != NULL) {
        if(sym->type != FUNC_TYPE) {
            char* temp1 = malloc(30);
            if(sym->const_init != NULL) {
                if(sym->type == INT_TYPE) {
                    if(sym->const_init->type == INT_TYPE) {
                        sprintf(temp1, "%s: .word %d\n", sym->name, sym->const_init->const_int);    
                    } else if(sym->const_init->type == FLOAT_TYPE) {
                        sprintf(temp1, "%s: .word %d\n", sym->name, (int)sym->const_init->const_float);    
                    } else {
                        print_error("Error in converting global variable type");
                    }
                } else if(sym->type == FLOAT_TYPE) {
                    if(sym->const_init->type == FLOAT_TYPE) {
                        sprintf(temp1, "%s: .float %f\n", sym->name, sym->const_init->const_float);    
                    } else if(sym->const_init->type == INT_TYPE) {
                        sprintf(temp1, "%s: .float %f\n", sym->name, (float)sym->const_init->const_int);    
                    } else {
                        print_error("Error in converting global variable type");
                    }
                } else if(sym->type == STRING_TYPE) {
                    if(sym->const_init->type != STRING_TYPE) {
                        print_error("Error in converting global variable type");
                    } else {
                        sprintf(temp1, "%s: .asciiz %s\n", sym->name, sym->const_init->string_literal);    
                    }
                }
            } else {
                sprintf(temp1, "%s: .space 4\n", sym->name);
            }
            strcat(temp, temp1);
        }
        sym = sym->next;
    }
    return temp;

}


Expression* create_expression(char* addr, var_type type, SymbolTable* sym_entry) {

    Expression* new_expr = malloc(sizeof(Expression));
    if(new_expr == NULL) { print_error("Cannot create expression"); }
    new_expr->code = malloc(CODE_SIZE);
    new_expr->result_location = strdup(addr);
    new_expr->sym_entry = sym_entry;
    new_expr->curr_depth = 0;
    new_expr->result_type = type;
    new_expr->const_val = NULL;
    return new_expr;

}

int get_offset_from_location(char* location) {

    char* str_offset = strtok(location, "(");
    int offset = atoi(str_offset);
    return offset;

}

char* get_location_from_offset(int addr) {
    char* location = malloc(CODE_SIZE);
    sprintf(location, "%d($fp)", addr);
    return location;
}

Expression* prepare_array(Expression* var, Expression* index) {
    SymbolTable* array_entry = var->sym_entry;
    if(var->curr_depth > array_entry->arr_depth - 1) {
        print_error("Invalid indexing of array");
    }
    if(index->result_type != INT_TYPE) {
        print_error("Array index should be integer");
    }
    Expression* exp = malloc(sizeof(Expression));
    exp->sym_entry = var->sym_entry;
    exp->result_type = var->result_type;
    exp->curr_depth = var->curr_depth + 1;
    exp->code = malloc(CODE_SIZE);
    strcat(exp->code, var->code);
    strcat(exp->code, index->code);

    int new_offset = 1;
    for(int i = var->curr_depth + 1; i < array_entry->arr_depth; i++) {
        new_offset = new_offset*array_entry->arr_size[i];
    }
    new_offset *= 4;

    // Store the address of the element being accessed
    int addr = allocate_on_stack(4);
    char* location = get_location_from_offset(addr);
    char* temp = malloc(CODE_SIZE);
    if(var->curr_depth == 0) {
        sprintf(temp, "\tlw $t0, %s\n\tlw $t1, %s\n\tli $t2, %d\n\tmul $t1, $t1, $t2\n\tadd $t0, $t0, $t1\n\tsw $t0, %s\n", var->result_location, index->result_location, new_offset, location);
        exp->result_location = location;
    } else {
        // Calculate new address
        sprintf(temp, "\tlw $t0, %s\n\tlw $t1, %s\n\tli $t2, %d\n\tmul $t1, $t1, $t2\n\tadd $t0, $t0, $t1\n\tsw $t0, %s\n", var->result_location, index->result_location, new_offset, var->result_location);
        exp->result_location = var->result_location;
    }
    strcat(exp->code, temp);
    return exp;

}

char* prepare_print(Parameters* params) {
    char* code = malloc(CODE_SIZE);
    char* reversed[50];
    int i = 0;
    Parameters* ptr = params;
    while (ptr != NULL) {
        // Initialize the parameter
        char* temp = malloc(CODE_SIZE);
        strcat(temp, ptr->code);
        char* temp1 = malloc(CODE_SIZE);
        if(ptr->arr_depth > 0) {
            sprintf(temp1, "\tlw $t0, %s\n", ptr->initialise_location);
            ptr->initialise_location = "0($t0)";
            strcat(temp, temp1);
        }
        if(ptr->type == INT_TYPE) {
            sprintf(temp1, "\tli $v0, 1\n\tlw $a0, %s\n\tsyscall\n", ptr->initialise_location);
        } else if(ptr->type == FLOAT_TYPE) {
            sprintf(temp1, "\tli $v0, 2\n\tlwc1 $f12, %s\n\tsyscall\n", ptr->initialise_location);
        } else if(ptr->type == STRING_TYPE) {
            sprintf(temp1, "\tli $v0, 4\n\tla $a0, %s\n\tsyscall\n", ptr->initialise_location);
        } else {
            print_error("This type cannot be printed");
        }
        strcat(temp, temp1);
        reversed[i] = temp;
        i++;
        ptr = ptr->next;
    }
    while(i > 0) {
        i--;
        strcat(code, reversed[i]);
    }
    return code;
    
}

char* prepare_scan(Parameters* params) {
    char* code = malloc(CODE_SIZE);
    char* reversed[50];
    int i = 0;
    Parameters* ptr = params;
    while (ptr != NULL) {
        char* temp = malloc(CODE_SIZE);
        strcat(temp, ptr->code);
        char* temp1 = malloc(CODE_SIZE);
        if(ptr->arr_depth > 0) {
            sprintf(temp1, "\tlw $t0, %s\n", ptr->initialise_location);
            ptr->initialise_location = "0($t0)";
            strcat(temp, temp1);
        }
        if(ptr->type == INT_TYPE) {
            sprintf(temp1, "\tli $v0, 5\n\tsyscall\n\tsw $v0, %s\n", ptr->initialise_location);
        } else if(ptr->type == FLOAT_TYPE) {
            sprintf(temp1, "\tli $v0, 6\n\tsyscall\n\tswc1 $f0, %s\n", ptr->initialise_location);
        } else if(ptr->type == STRING_TYPE) {
            // sprintf(temp, "\tli $v0, ")
        } else {
            print_error("This type cannot be scanned");
        }
        strcat(temp, temp1);
        reversed[i] = temp;
        i++;
        ptr = ptr->next;
    }
    while(i > 0) {
        i--;
        strcat(code, reversed[i]);
    }
    return code;
}

char* prepare_calling(Expression* callee, Parameters* params) {

    char* code = malloc(CODE_SIZE);
    strcat(code, callee->code);
    if(callee->sym_entry->type != FUNC_TYPE) {
        print_error("This variable is not callable");
    }
    if(strcmp(callee->sym_entry->name, "print") == 0) {
        strcat(code, prepare_print(params));
    } else if(strcmp(callee->sym_entry->name, "scan") == 0) {
        strcat(code, prepare_scan(params));
    } else {
        if(callee->sym_entry->func->params != NULL) {
            if(params == NULL) {
                print_error("Count mismatch for parameters");
            }
            if(callee->sym_entry->func->params->param_count != params->param_count) {
                print_error("Count mismatch for parameters");
            }
            // push parameters on the stack
            Parameters* ptr = params;
            Parameters* callee_ptr = callee->sym_entry->func->params;
            while (ptr != NULL) {
                // calculate the parameter expression
                strcat(code, ptr->code);
                // store the result of evaluation on stack
                char* save_value = malloc(CODE_SIZE);
                strcat(code, "\taddu $sp, $sp, -4\n");
                char* location = strdup(ptr->initialise_location);
                if(callee_ptr->arr_depth > 0) { // The parameter is array type
                    if(callee_ptr->arr_depth != ptr->arr_depth) {
                        print_error("Array dimensions do not match to parameter");
                    }
                    if(ptr->curr_depth != 0) {
                        print_error("Invalid way of passing array");
                    }
                    sprintf(save_value, "\tlw $t0, %s\n\tsw $t0, 0($sp)\n", location);
                    strcat(code, save_value);
                } else {
                    if(ptr->curr_depth > 0) {    // We are passing an indexed array
                        sprintf(save_value, "\tlw $t0, %s\n", location);
                        location = "0($t0)";
                        strcat(code, save_value);
                    }
                    if(ptr->type == INT_TYPE) {
                        sprintf(save_value, "\tlw $t0, %s\n", location);
                        strcat(code, save_value);
                        strcat(code, "\tsw $t0, 0($sp)\n");
                    } else if(ptr->type == FLOAT_TYPE) {
                        sprintf(save_value, "\tlwc1 $f0, %s\n", location);
                        strcat(code, save_value);
                        strcat(code, "\tswc1 $f0, 0($sp)\n");
                    }
                }
                
                ptr = ptr->next;
                callee_ptr = callee_ptr->next;
            }
        } else if(params != NULL) {
            print_error("Function requires no parameters");
        }
        // Push the current base address
        strcat(code, "\taddu $sp, $sp, -4\n");
        strcat(code, "\tsw $fp, 0($sp)\n");
        // The new frame is the current stack pointer
        strcat(code, "\tmove $fp, $sp\n");
        char* temp = malloc(CODE_SIZE);
        sprintf(temp, "\tjal %s\n", callee->sym_entry->name);
        strcat(code, temp);
        // allocate space for evaluating return value
        int addr = allocate_on_stack(4);
        if(callee->sym_entry->func->return_type == FLOAT_TYPE) {
            sprintf(temp, "\tswc1 $f1, %d($fp)\n", addr);
        } else {
            sprintf(temp, "\tsw $v0, %d($fp)\n", addr);
        }
        strcat(code, temp);
    }
    return code;
}

char* function_return(Expression* exp, var_type return_type) {
    char* code = malloc(CODE_SIZE);
    if(exp != NULL) {
        strcat(code, exp->code);
        char* temp = malloc(CODE_SIZE);
        if(return_type == INT_TYPE) {
            if(exp->result_type == INT_TYPE) {
                sprintf(temp, "\tlw $v0, %s\n", exp->result_location);
            } else if(exp->result_type == FLOAT_TYPE) {
                sprintf(temp, "\tlwc1 $f0, %s\n\tcvt.w.s $f0, $f0\n\tmfc1 $v0, $f0\n", exp->result_location);
            } else {
                print_error("Cannot implicitely convert this type to return type");
            }
        } else if(return_type == FLOAT_TYPE) {
            if(exp->result_type == FLOAT_TYPE) {
                sprintf(temp, "\tlwc1 $f1, %s\n", exp->result_location);
            } else if(exp->result_type == INT_TYPE) {
                sprintf(temp, "\tlw $t0, %s\n\tmtc1 $t0, $f1\n\tcvt.s.w $f1, $f1\n", exp->result_location);
            } else {
                print_error("Cannot implicitely convert this type to return type");
            }
        } else if(return_type == STRING_TYPE) {
            if(exp->result_type != STRING_TYPE) {
                print_error("Cannot implicitely convert this type to return type");
            }
            sprintf(temp, "\tlw $t0, %s\n", exp->result_location);
        } else {
            print_error("Return type not supported");
        }
        strcat(code, temp);
    }
    if(strcmp(curr_function->name, "main") != 0) {
        // Restore return address
        strcat(code, "\tlw $ra, -4($fp)\n");
        // Restore original base pointer
        strcat(code, "\tlw $fp, 0($fp)\n");
        // Restore stack pointer
        char* temp = malloc(CODE_SIZE);
        sprintf(temp, "\taddu $sp, $sp, %d\n", abs(curr_function->curr_offset) + 4*curr_function->params->param_count + 4);
        strcat(code, temp);
        sprintf(temp, "\tjr $ra\n");
        strcat(code, temp);
    } 
    return code; 
}
