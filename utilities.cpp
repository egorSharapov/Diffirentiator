#include <assert.h>
#include <float.h>
#include "utilities.hpp"


bool is_equal (double number_1, double number_2)
{
    assert (isfinite(number_1));
    assert (isfinite(number_2));    
    
    return fabs (number_1 - number_2) < DBL_EPSILON;

}

bool is_unary (OPERATORS op_value)
{
    return (op_value == OP_SIN or op_value == OP_COS);
}



const char *convert_tex_op (OPERATORS op_value)
{
    if (op_value == OP_ADD)
        return " + ";

    if (op_value == OP_SUB)
        return " - ";

    if (op_value == OP_MUL)
        return " \\cdot ";
    
    if (op_value == OP_DIV)
        return " \\over ";

    if (op_value == OP_SIN)
        return "\\sin";
    
    if (op_value == OP_COS)
        return "\\cos";
    
    if (op_value == OP_PWR)
        return "^";
    
    return "non operator";
}


const char *convert_op (OPERATORS op_value)
{
    if (op_value == OP_ADD)
        return " + ";

    if (op_value == OP_SUB)
        return " - ";

    if (op_value == OP_MUL)
        return " * ";
    
    if (op_value == OP_DIV)
        return " \\ ";

    if (op_value == OP_SIN)
        return "sin";
    
    if (op_value == OP_COS)
        return "cos";
    
    if (op_value == OP_PWR)
        return "^";
    
    return "non operator";
}

