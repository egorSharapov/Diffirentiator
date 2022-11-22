#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <math.h>
#include "diff_tree.hpp"

bool is_unary (OPERATORS op_value);
bool is_equal (double number_1, double number_2);

const char *convert_op     (OPERATORS op_value);
const char *convert_tex_op (OPERATORS op_value);

#endif