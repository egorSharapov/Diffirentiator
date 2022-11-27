#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <math.h>
#include "diff_tree.hpp"

#define Safety_calloc(size_of_memory) _Safety_calloc (size_of_memory, __LINE__)


int Factorial (int number);
void *_Safety_calloc (size_t size_of_memory, int line);
bool is_unary (OPERATORS op_value);
bool is_equal (double number_1, double number_2);

const char *convert_op     (OPERATORS op_value);
const char *convert_tex_op (OPERATORS op_value);

#endif