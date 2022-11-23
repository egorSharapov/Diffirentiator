#include "file_analize.hpp"
#include "diffirentiator.hpp"
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




    tex_file = fopen (tex_file_name, "wb");
    if (!tex_file)  
        printf ("tex open error");
    
    read_t_file (&text, "out\\tree_input.txt", &tree);

    printf ("succes read\n");

    Print_tex_title (tex_file);

    //вычисление n-ной производной функции
    Taking_nth_derivative (&text, &tree, &diff_tree);

    //вычисление функции в точке
    Calculate_function_in_point (&text, &tree);

    //вычисление производной функции в точке
    Calculate_derivative_in_point (&text, &diff_tree);

    //вычисление ряда тейлора 
    Taylor_series_calculation (&text, &tree);

    //построение графика 
    Graph_plotter (&text, &tree, graph_file_name);

    fprintf (tex_file, "\\end{document}");
    fclose (tex_file);

    system ("pdflatex --output-directory=out out/tex_input.tex > nul");

    printf ("latex compile succes");

    return 0;
}