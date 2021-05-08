#include "operators.h"

char* increment_operator(Expression* exp) {
    char* code = malloc(CODE_SIZE);
    strcat(code, exp->code);
    if(!(exp->result_type == INT_TYPE || exp->result_type == FLOAT_TYPE)) {
        print_error("Cannot increment this type");
    }
    if(exp->sym_entry->arr_depth > 0) {
        print_error("Array variable cannot be incremented");
    }
    char* temp = malloc(CODE_SIZE);
    if(exp->result_type == INT_TYPE) {
        sprintf(temp, "\tlw $t0, %s\n\taddi $t0, $t0, 1\n\tsw $t0, %s\n", exp->result_location, exp->result_location);

    } else {
        sprintf(temp, "\tlwc1 $f0, %s\n\tadd.s $f0, $f0, 1\n\tswc1 $f0, %s\n", exp->result_location, exp->result_location);
    }
    strcat(code, temp);
    return code;

}

char* decrement_operator(Expression* exp) {
    char* code = malloc(CODE_SIZE);
    strcat(code, exp->code);
    if(!(exp->result_type == INT_TYPE || exp->result_type == FLOAT_TYPE)) {
        print_error("Cannot decrement this type");
    }
    if(exp->sym_entry->arr_depth > 0) {
        print_error("Array variable cannot be incremented");
    }
    char* temp = malloc(CODE_SIZE);
    if(exp->result_type == INT_TYPE) {
        sprintf(temp, "\tlw $t0, %s\n\taddi $t0, $t0, -1\n\tsw $t0, %s\n", exp->result_location, exp->result_location);

    } else {
        sprintf(temp, "\tlwc1 $f0, %s\n\tadd.s $f0, $f0, -1\n\tswc1 $f0, %s\n", exp->result_location, exp->result_location);
    }
    strcat(code, temp);
    return code;
}

char* minus_operator(Expression* exp) {
    char* code = malloc(CODE_SIZE);
    strcat(code, exp->code);
    if(!(exp->result_type == INT_TYPE || exp->result_type == FLOAT_TYPE)) {
        print_error("Cannot negate this type");
    }
    char* temp = malloc(CODE_SIZE);

    int addr = allocate_on_stack(4);
    char* location = get_location_from_offset(addr);
    if(exp->result_type == INT_TYPE) {
        if(exp->curr_depth > 0) {
            sprintf(temp, "\tlw $t0, %s\n\tlw $t0, 0($t0)\n\tsub $t0, $zero, $t0\n\tsw $t0, %s\n", exp->result_location, location);
        } else {
            sprintf(temp, "\tlw $t0, %s\n\tsub $t0, $zero, $t0\n\tsw $t0, %s\n", exp->result_location, location);
        }
    } else {
        if(exp->curr_depth > 0) {
            sprintf(temp, "\tlw $t0, %s\n\tlwc1 $f0, 0($t0)\n\tneg.s $f0, $f0\n\tswc1 $f0, %s\n", exp->result_location, location);
        } else {
            sprintf(temp, "\tlwc1 $f0, %s\n\tneg.s $f0, $f0\n\tswc1 $f0, %s\n", exp->result_location, location);
        }
    }
    strcat(code, temp);
    return code;

}

char* not_operator(Expression* exp) {
    char* code = malloc(CODE_SIZE);
    strcat(code, exp->code);
    if(exp->result_type != INT_TYPE) {
        print_error("Cannot perform ! on types other than INT");
    }
    char* temp = malloc(CODE_SIZE);
    int addr = allocate_on_stack(4);
    char* location = get_location_from_offset(addr);
    if(exp->curr_depth > 0) {
        sprintf(temp, "\tlw $t0, %s\n\tlw $t0, 0($t0)\n\tnot $t0, $t0\n\tsw $t0, %s\n", exp->result_location, location);
    } else {
        sprintf(temp, "\tlw $t0, %s\n\tnot $t0, $t0\n\tsw $t0, %s\n", exp->result_location, location);
    }
    strcat(code, temp);
    return code;
}


char* change_type(Expression* exp, var_type new_type) {
    if(!(new_type == INT_TYPE || new_type == FLOAT_TYPE)) {
        print_error("Cannot perform type conversion to suitable type");
    }
    char* code = malloc(CODE_SIZE);
    char* temp = malloc(CODE_SIZE);

    int addr = allocate_on_stack(4);
    char* location = get_location_from_offset(addr);
    if(new_type == INT_TYPE && exp->result_type == FLOAT_TYPE) {
        if(exp->curr_depth > 0) {
            sprintf(temp, "\tlw $t0, %s\n\tlwc1 $f0, 0($t0)\n\tcvt.w.s $f0, $f0\n\tmfc1 $t0, $f0\n\tsw $t0, %s\n", exp->result_location, location);
        } else {
            sprintf(temp, "\tlwc1 $f0, %s\n\tcvt.w.s $f0, $f0\n\tmfc1 $t0, $f0\n\tsw $t0, %s\n", exp->result_location, location);
        }
    } else if(new_type == FLOAT_TYPE && exp->result_type == INT_TYPE) {
        if(exp->curr_depth > 0) {
            sprintf(temp, "\tlw $t0, %s\n\tlw $t0, 0($t0)\n\tmtc1 $t0, $f0\n\tcvt.s.w $f0, $f0\n\tswc1 $f0, %s\n", exp->result_location, location);
        } else {
            sprintf(temp, "\tlw $t0, %s\n\tmtc1 $t0, $f0\n\tcvt.s.w $f0, $f0\n\tswc1 $f0, %s\n", exp->result_location, location);
        }
    }
    strcat(code, temp);
    exp->curr_depth = 0; 
    return code;

}

void initialise_variable(VariableDeclaration* var, Expression* exp) {
    char* code = malloc(CODE_SIZE);
    strcat(code, exp->code);
    char* temp = malloc(CODE_SIZE);
    char* location = malloc(30);
    if(var->table_entry->arr_depth > 0) {
        print_error("Cannot initiate array variable");
    }
    if(var->table_entry->scope == 0) {
        location = strdup(var->table_entry->name);
        if(exp->const_val == NULL) {
            print_error("Global variable must be initialized with constant value");
        }
        var->table_entry->const_init = exp->const_val;
        var->initialiser_code = "";
    } else {
        sprintf(location, "%d($fp)", var->table_entry->addr_offset);
        if(var->table_entry->type == INT_TYPE) {
            if(exp->result_type == INT_TYPE) {
                sprintf(temp, "\tlw $t0, %s\n\tsw $t0, %s\n", exp->result_location, location);
            } else if(exp->result_type == FLOAT_TYPE) {
                sprintf(temp, "\tlwc1 $f0, %s\n\tcvt.w.s $f0, $f0\n\tmfc1 $t0, $f0\n\tsw $t0, %s\n", exp->result_location, location);
            } else {
                print_error("Cannot assign this type to int variable");
            }
        } else if(var->table_entry->type == FLOAT_TYPE) {
            if(exp->result_type == INT_TYPE) {
                sprintf(temp, "\tlw $t0, %s\n\tmtc1 $t0, $f0\n\tcvt.s.w $f0, $f0\n\tswc1 $f0, %s\n", exp->result_location, location);
            } else if(exp->result_type == FLOAT_TYPE) {
                sprintf(temp, "\tlwc1 $f0, %s\n\tswc1 $f0, %s\n", exp->result_location, location);
            } else {
                print_error("Cannot assign this type to float variable");
            }
        } else if(var->table_entry->type == STRING_TYPE) {
            if(exp->result_type != STRING_TYPE) {
                print_error("Cannot assign this type to string variable");
            }   
            sprintf(temp, "\tla $t0, %s\n\tsw $t0, %s\n", exp->result_location, location);
        } else {
            print_error("Type not supported");
        }
        strcat(code, temp);
        var->initialiser_code = code;
    }
}


var_type implicit_type_conversion(var_type type1, var_type type2) {
    if(!((type1 == INT_TYPE || type1 == FLOAT_TYPE) && (type2 == INT_TYPE || type2 == FLOAT_TYPE))) {
        print_error("Cannot interpret the type for the operation");
    }
    if(type1 == INT_TYPE && type2 == INT_TYPE) {
        return INT_TYPE;
    } else {
        return FLOAT_TYPE;
    }
}


/*
Mapping of op vs actual operation
0: *
1: /
2: %
3: +
4: -
5: <<
6: >>
7: <
8: >
9: <=
10: >=
11: ==
12: !=
13: &, &&
14: ^
15: |, ||
*/

char* binary_op(Expression* exp1, Expression* exp2, int op) {
    if(exp1->sym_entry != NULL) {
        if(exp1->sym_entry->arr_depth > 0 && exp1->curr_depth == 0) {
            print_error("Array cannot be accessed without indexing");
        }
    }
    if(exp2->sym_entry != NULL) {
        if(exp2->sym_entry->arr_depth > 0 && exp2->curr_depth == 0) {
            print_error("Array cannot be accessed without indexing");
        }
    }
    char* code = malloc(CODE_SIZE);
    strcat(code, exp1->code);
    strcat(code, exp2->code);
    char* loc1 = strdup(exp1->result_location);
    char* loc2 = strdup(exp2->result_location);
    var_type new_type = implicit_type_conversion(exp1->result_type, exp2->result_type);
    char* temp = malloc(CODE_SIZE);
    if(new_type == FLOAT_TYPE) {
        if(exp1->result_type != FLOAT_TYPE) {
            temp = change_type(exp1, FLOAT_TYPE);
            loc1 = get_location_from_offset(curr_function->curr_offset);
        } else if(exp2->result_type != FLOAT_TYPE) {
            temp = change_type(exp2, FLOAT_TYPE);
            loc2 = get_location_from_offset(curr_function->curr_offset);
        }
        strcat(code, temp);
        // Allocating space for new value
        allocate_on_stack(4);
        char* new_loc = get_location_from_offset(curr_function->curr_offset);
        if(exp1->curr_depth > 0) {
            sprintf(temp, "\tlw $t0, %s\n\tlwc1 $f0, 0($t0)\n", loc1);
        } else {
            sprintf(temp, "\tlwc1 $f0, %s\n", loc1);
        }
        strcat(code, temp);
        if(exp2->curr_depth > 0) {
            sprintf(temp, "\tlw $t0, %s\n\tlwc1 $f1, 0($t0)\n", loc2);
        } else {
            sprintf(temp, "\tlwc1 $f1, %s\n", loc2);
        }
        strcat(code, temp);
        switch (op) {
        case 0:
            sprintf(temp, "\tmul.s $f0, $f0, $f1\n\tswc1 $f0, %s\n", new_loc);
            break;
        case 1:
            sprintf(temp, "\tdiv.s $f0, $f0, $f1\n\tswc1 $f0, %s\n", new_loc);
            break;
        case 3:
            sprintf(temp, "\tadd.s $f0, $f0, $f1\n\tswc1 $f0, %s\n", new_loc);
            break;
        case 4:
            sprintf(temp, "\tsub.s $f0, $f0, $f1\n\tswc1 $f0, %s\n", new_loc);
            break;
        case 7:
            sprintf(temp, "\tc.lt.s $f0, $f1\n\tcfc1 $t0, $25\n\tsw $t0, %s\n", new_loc);
            break;
        case 8:
            sprintf(temp, "\tc.lt.s $f1, $f0\n\tcfc1 $t0, $25\n\tsw $t0, %s\n", new_loc);
            break;
        case 9:
            sprintf(temp, "\tc.le.s $f0, $f1\n\tcfc1 $t0, $25\n\tsw $t0, %s\n", new_loc);
            break;
        case 10:
            sprintf(temp, "\tc.le.s $f1, $f0\n\tcfc1 $t0, $25\n\tsw $t0, %s\n", new_loc);
            break;
        case 11:
            sprintf(temp, "\tc.eq.s $f0, $f1\n\tcfc1 $t0, $25\n\tsw $t0, %s\n", new_loc);
            break;
        case 12:
            sprintf(temp, "\tc.ne.s $f0, $f1\n\tcfc1 $t0, $25\n\tsw $t0, %s\n", new_loc);
            break;
        default:
            print_error("Expression should be integer for this operand");
        }
        strcat(code, temp);
    } else {
        // Allocating space for new value
        allocate_on_stack(4);
        char* new_loc = get_location_from_offset(curr_function->curr_offset);
        if(exp1->curr_depth > 0) {
            sprintf(temp, "\tlw $t0, %s\n\tlw $t0, 0($t0)\n", loc1);
        } else {
            sprintf(temp, "\tlw $t0, %s\n", loc1);
        }
        strcat(code, temp);
        if(exp2->curr_depth > 0) {
            sprintf(temp, "\tlw $t1, %s\n\tlw $t1, 0($t1)\n", loc2);
        } else {
            sprintf(temp, "\tlw $t1, %s\n", loc2);
        }
        strcat(code, temp);
        switch (op) {
        case 0:
            sprintf(temp, "\tmul $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 1:
            sprintf(temp, "\tdiv $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 2:
            sprintf(temp, "\tdiv $t0, $t1\n\tmfhi $t2\n\tsw $t2, %s\n", new_loc);
            break;
        case 3:
            sprintf(temp, "\tadd $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 4:
            sprintf(temp, "\tsub $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 5:
            sprintf(temp, "\tsllv $t2, $t0, $t1\n\tsw $t2, %s\n", new_loc);
            break;
        case 6:
            sprintf(temp, "\tsrav $t2, $t0, $t1\n\tsw $t2, %s\n", new_loc);
            break;
        case 7:
            sprintf(temp, "\tslt $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 8:
            sprintf(temp, "\tsgt $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 9:
            sprintf(temp, "\tsle $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 10:
            sprintf(temp, "\tsge $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 11:
            sprintf(temp, "\tseq $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 12:
            sprintf(temp, "\tsne $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 13:
            sprintf(temp, "\tand $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 14:
            sprintf(temp, "\txor $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        case 15:
            sprintf(temp, "\tor $t0, $t0, $t1\n\tsw $t0, %s\n", new_loc);
            break;
        }
        strcat(code, temp);
    }
    return code;   
}

char* assignment_op(Expression* exp1,  Expression* exp2) {
    char* code = malloc(CODE_SIZE);
    strcat(code, exp1->code);
    strcat(code, exp2->code);

    char* temp = malloc(CODE_SIZE);
    if(exp1->sym_entry != NULL) {
        if(exp1->sym_entry->arr_depth > 0 && exp1->curr_depth == 0) {
            print_error("Array cannot be accessed without indexing");
        }
    }
    if(exp2->sym_entry != NULL) {
        if(exp2->sym_entry->arr_depth > 0 && exp2->curr_depth == 0) {
            print_error("Array cannot be accessed without indexing");
        }
    }
    if(exp1->curr_depth > 0) {
        sprintf(temp, "\tlw $t1, %s\n", exp1->result_location);
        exp1->result_location = "0($t1)";
        strcat(code, temp);
    }
    if(exp2->curr_depth > 0) {
        sprintf(temp, "\tlw $t2, %s\n", exp2->result_location);
        exp2->result_location = "0($t2)";
        strcat(code, temp);
    }
    if(exp1->result_type == INT_TYPE) {
        if(exp2->result_type == INT_TYPE) {
            sprintf(temp, "\tlw $t0, %s\n\tsw $t0, %s\n", exp2->result_location, exp1->result_location);
        } else if(exp2->result_type == FLOAT_TYPE) {
            sprintf(temp, "\tlwc1 $f0, %s\n\tcvt.w.s $f0, $f0\n\tmfc1 $t0, $f0\n\tsw $t0, %s\n", exp2->result_location, exp1->result_location);
        } else {
            print_error("Cannot assign this type to int variable");
        }
    } else if(exp1->result_type == FLOAT_TYPE) {
        if(exp2->result_type == INT_TYPE) {
            sprintf(temp, "\tlw $t0, %s\n\tmtc1 $t0, $f0\n\tcvt.s.w $f0, $f0\n\tswc1 $f0, %s\n", exp2->result_location, exp1->result_location);
        } else if(exp2->result_type == FLOAT_TYPE) {
            sprintf(temp, "\tlwc1 $f0, %s\n\tswc1 $f0, %s\n", exp2->result_location, exp1->result_location);
        } else {
            print_error("Cannot assign this type to float variable");
        }
    } else if(exp1->result_type == STRING_TYPE) {
        if(exp2->result_type != STRING_TYPE) {
            print_error("Cannot assign this type to string variable");
        }   
        sprintf(temp, "\tla $t0, %s\n\tsw $t0, %s\n", exp2->result_location, exp1->result_location);
    } else {
        print_error("Type not supported");
    }
    strcat(code, temp);
    return code;

}

char* if_construct(Expression* condition, char* body) {
    char* code = malloc(CODE_SIZE);
    sprintf(code, "__start_if_label%d:\n", curr_if_label);
    strcat(code, condition->code);
    char* temp = malloc(CODE_SIZE);
    sprintf(temp, "\tlw $t0, %s\n\tbeqz $t0, __end_if_label%d\n", condition->result_location, curr_if_label);
    strcat(code, temp);
    strcat(code, body);
    sprintf(temp, "__end_if_label%d:\n", curr_if_label);
    strcat(code, temp);
    curr_if_label++;
    return code;
}

char* if_else_construct(Expression* condition, char* true_body, char* false_body) {
    char* code = malloc(CODE_SIZE);
    sprintf(code, "__start_if_label%d:\n", curr_if_label);
    strcat(code, condition->code);
    char* temp = malloc(CODE_SIZE);
    sprintf(temp, "\tlw $t0, %s\n\tbeqz $t0, __else_if_label%d\n", condition->result_location, curr_if_label);
    strcat(code, temp);
    strcat(code, true_body);
    sprintf(temp, "\tb __end_if_label%d\n", curr_if_label);
    strcat(code, temp);
    sprintf(temp, "__else_if_label%d:\n", curr_if_label);
    strcat(code, temp);
    strcat(code, false_body);
    sprintf(temp, "__end_if_label%d:\n", curr_if_label);
    strcat(code, temp);
    curr_if_label++;
    return code;

}

char* while_construct(Expression* condition, char* body) {
    char* code = malloc(CODE_SIZE);
    sprintf(code, "__start_loop_label%d:\n", curr_loop_label);
    strcat(code, condition->code);
    char* temp = malloc(CODE_SIZE);
    sprintf(temp, "\tlw $t0, %s\n\tbeqz $t0, __end_loop_label%d\n", condition->result_location, curr_loop_label);
    strcat(code, temp);
    strcat(code, body);
    sprintf(temp, "\tb __start_loop_label%d\n", curr_loop_label);
    strcat(code, temp);
    sprintf(temp, "__end_loop_label%d:\n", curr_loop_label);
    strcat(code, temp);
    curr_loop_label++;
    return code;

}

char* do_while_construct(Expression* condition, char* body) {
    char* code = malloc(CODE_SIZE);
    sprintf(code, "__start_loop_label%d:\n", curr_loop_label);
    strcat(code, body);
    strcat(code, condition->code);
    char* temp = malloc(CODE_SIZE);
    sprintf(temp, "\tlw $t0, %s\n\tbeqz $t0, __end_loop_label%d\n", condition->result_location, curr_loop_label);
    strcat(code, temp);
    sprintf(temp, "\tb __start_loop_label%d\n", curr_loop_label);
    strcat(code, temp);
    sprintf(temp, "__end_loop_label%d:\n", curr_loop_label);
    strcat(code, temp);
    curr_loop_label++;
    return code;

}

char* for_construct(Expression* init, Expression* condition, Expression* exec, char* body) {

    char* code = malloc(CODE_SIZE);
    strcat(code, init->code);
    char* temp = malloc(CODE_SIZE);
    sprintf(temp, "__start_loop_label%d:\n", curr_loop_label);
    strcat(code, temp);
    strcat(code, condition->code);
    sprintf(temp, "\tlw $t0, %s\n\tbeqz $t0, __end_loop_label%d\n", condition->result_location, curr_loop_label);
    strcat(code, temp);
    strcat(code, body);
    if(exec != NULL) {
        strcat(code, exec->code);
    }
    sprintf(temp, "\tb __start_loop_label%d\n", curr_loop_label);
    strcat(code, temp);
    sprintf(temp, "__end_loop_label%d:\n", curr_loop_label);
    strcat(code, temp);
    curr_loop_label++;
    return code;

}