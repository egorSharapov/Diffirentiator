#ifndef DIFFIRENTIATOR_HPP_INCLUDED
#define DIFFIRENTIATOR_HPP_INCLUDED

#include "diff_tree.hpp"


enum NUMBER_CMD_STRING
{
    derivative_n_str    = 1,
    func_calc_str       = 2,
    derivative_calc_str = 3,
    taylor_series_str   = 4,
    tangent_point_str   = 5,
    graph_range_str     = 6,
};


Tree_node *Diff    (const Tree_node *node, bool is_dump_to_tex, const char diff_var = 'x');
Tree_node *Copy    (Tree_node *node);
Tree_node *Simpler (Tree_node *node);

Tree_node *Constant_simplification (Tree_node *node, bool *is_modifed);

void Graph_plotter                 (Text *text, Root *tree, const char *graph_file_name, Constants *);
void Taking_nth_derivative         (Text *text, Root *tree, Root *diff_tree);
void Taylor_series_calculation     (Text *text, Root* tree, Constants *);
void Calculate_function_in_point   (Text *text, Root *tree, Constants *);
void Calculate_derivative_in_point (Text *text, Root *diff_tree, Constants *);
void Calculate_full_derivative     (Root *tree, Constants *f_constants);

int    Factorial      (int number);
double Calculate_func (Tree_node *node, double x_value, Constants *f_constants);
double Find_const_by_name (const char var_name, Constants *f_constants);
#endif