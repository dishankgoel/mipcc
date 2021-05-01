#include "symbol_table.h"

char* increment_operator(Expression* exp);
char* decrement_operator(Expression* exp);
char* minus_operator(Expression* exp);
char* not_operator(Expression* exp);

char* change_type(Expression* exp, var_type new_type);
void initialise_variable(VariableDeclaration* var, Expression* exp);

var_type implicit_type_conversion(var_type type1, var_type type2);

char* binary_op(Expression* exp1, Expression* exp2, int op);

char* assignment_op(Expression* exp1,  Expression* exp2);

char* if_construct(Expression* condition, char* body);
char* if_else_construct(Expression* condition, char* true_body, char* false_body);
char* while_construct(Expression* condition, char* body);
char* do_while_construct(Expression* condition, char* body);
char* for_construct(Expression* init, Expression* condition, Expression* exec, char* body);