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
    return (op_value == OP_SIN or op_value == OP_COS or op_value >= 8);
}



const char *convert_tex_op (OPERATORS op_value)
{
    switch (op_value)
    {
    case OP_ADD:  return " + ";
    case OP_SUB:  return " - ";
    case OP_MUL:  return " \\cdot ";
    case OP_DIV:  return " \\over ";
    case OP_SIN:  return "\\sin";
    case OP_COS:  return "\\cos";
    case OP_PWR:  return "^";
    case OP_LN:   return "\\ln";
    case OP_TAN:  return "\\tg";
    case OP_COT : return "\\ctg";
    case OP_NON: return "non operator";
    default:
        break;
    }
    return "convert error";
}


const char *convert_op (OPERATORS op_value)
{
    switch (op_value)
    {
    case OP_ADD: return " + ";
    case OP_SUB: return " - ";
    case OP_MUL: return " * ";
    case OP_DIV: return " / ";
    case OP_SIN: return "sin";
    case OP_COS: return "cos";
    case OP_PWR: return "**";
    case OP_LN:  return "log";
    case OP_TAN: return "tan";
    case OP_COT: return "\\ tan";
    case OP_NON: return "non operator";
    default:
        break;
    }

    return "convert error";
}



void *_Safety_calloc (size_t size_of_memory, int line)
{
    void *object = calloc (size_of_memory, 1);

    if (!object)
        printf ("calloc error in line %d\n", line);

    return object;
}


int Factorial (int number)
{
    if (number > 0)
        return (number*Factorial (number - 1));
    
    return 1;
}
