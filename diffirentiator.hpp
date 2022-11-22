#ifndef DIFFIRENTIATOR_HPP_INCLUDED
#define DIFFIRENTIATOR_HPP_INCLUDED

#include "diff_tree.hpp"

Tree_node *Diff (const Tree_node *node, bool is_dump_to_tex);
Tree_node *Copy (Tree_node *node);
Tree_node *Simpler (Tree_node *node);
Tree_node *Constant_simplification(Tree_node *node, bool *is_modifed);
double Calculate_func (Tree_node *node, double x_value);

int Factorial (int number);
#endif