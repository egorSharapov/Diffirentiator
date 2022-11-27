#ifndef DIFF_TEX_HPP_INCLUDED
#define DIFF_TEX_HPP_INCLUDED


#include "diff_tree.hpp"
#include "file_analize.hpp"

void Graph_plotter                 (Text *text, Root *tree, const char *graph_file_name, Constants *);
void Taking_nth_derivative         (Text *text, Root *tree, Root *diff_tree);
void Taylor_series_calculation     (Text *text, Root* tree, Constants *);
void Calculate_function_in_point   (Text *text, Root *tree, Constants *);
void Calculate_derivative_in_point (Text *text, Root *diff_tree, Constants *);
void Calculate_full_derivative     (Root *tree, Constants *f_constants);


#endif