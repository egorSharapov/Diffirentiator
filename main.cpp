#include "file_analize.hpp"
#include "diff_tex.hpp"
#include "windows.h"
#include "utilities.hpp"

const char *tex_file_name = "out\\tex_input.tex";
const char *graph_file_name = "out\\graph.gpi";

FILE *tex_file = NULL;

int main ()
{
    system ("chcp 65001");

    
    Text text = {};
    Root tree = {};
    Root diff_tree = {};
    Constants func_constants = {};

    tex_file = fopen (tex_file_name, "wb");
    if (!tex_file)  
        printf ("tex open error");
    
    read_t_file (&text, "out\\tree_input.txt", &tree, &func_constants);
    printf ("succes read file\n");

    Print_tex_title (tex_file, &tree, &func_constants);

    //вычисление n-ной производной функции
    Taking_nth_derivative (&text, &tree, &diff_tree);
    
    //вычисление функции в точке
    Calculate_function_in_point (&text, &tree, &func_constants);

    //вычисление производной функции в точке
    Calculate_derivative_in_point (&text, &diff_tree, &func_constants);

    //вычисление ряда тейлора 
    Taylor_series_calculation (&text, &tree, &func_constants);

    //взятие полной производной
    Calculate_full_derivative (&tree, &func_constants);

    //построение графика 
    Graph_plotter (&text, &tree, graph_file_name, &func_constants);

    Print_tex_end (tex_file);

    return 0;
}