#include "file_analize.hpp"
#include "diffirentiator.hpp"
#include "windows.h"
#include "utilities.hpp"

const char *tex_file_name = "out\\tex_input.tex";
FILE *tex_file = NULL;

int main ()
{
    system ("chcp 65001");

    Root tree = {};
    Root diff_tree = {};
    Root first_diff_tree = {};

    tex_file = fopen (tex_file_name, "wb");
    if (!tex_file)  
        printf ("tex open error");
    
    read_t_file ("out\\tree_input.txt", &tree);
    save_tree ("out\\tree_output.txt", &tree);

    printf ("succes read\n");


    Print_tex_title (tex_file);
    int diff_number = 0;
    printf ("Введите степень производной: ");
    scanf ("%d", &diff_number);

    fprintf (tex_file, "\n\\section{Взятие %d-ой производной.}\n\n", diff_number);
    
    fprintf (tex_file, "$ f(x) = ");
    Print_tex (tex_file, 0, tree.first_node);
    fprintf (tex_file, "$\n\n");

    bool show_steps = true;

    if (diff_number > 2)
        show_steps = false;
    
    while (diff_number--)
        diff_tree.first_node = Diff (tree.first_node, show_steps);
    
    fprintf (tex_file, "Получаем:\n\n");
    fprintf (tex_file, "$");
    Print_tex (tex_file, 1, tree.first_node);
    fprintf (tex_file, " = ");
    Print_tex (tex_file, 0, diff_tree.first_node);
    fprintf (tex_file, "$\n");

    fprintf (tex_file, "\n\\section{Упрощение.}\n\n");

    Graph_print_tree (&diff_tree);
    fprintf (tex_file, "$");
    Print_tex (tex_file, 0, diff_tree.first_node);
    fprintf (tex_file, " = ");
    diff_tree.first_node = Simpler (diff_tree.first_node);
    Print_tex (tex_file, 0, diff_tree.first_node);
    fprintf (tex_file, "$");


    //вычисление функции в точке
    fprintf (tex_file, "\n\\section{Вычисление значения функции в точке.}\n\n");

    double x_value = 0;
    printf ("Введите точку, в которой требуется вычислить значение функции: ");
    scanf ("%lg", &x_value);

    fprintf (tex_file, "$ f(%lg) = %lg $", x_value, Calculate_func (tree.first_node, x_value));

    //вычисление производной функции в точке
    fprintf (tex_file, "\n\\section{Вычисление производной функции в точке.}\n\n");

    x_value = 0;
    printf ("Введите точку, в которой требуется вычислить значение производной: ");
    scanf ("%lg", &x_value);

    fprintf (tex_file, "$ f(%lg) = %lg $", x_value, Calculate_func (diff_tree.first_node, x_value));


    // Taylor_series_calculation ();

    //вычисление ряда тейлора 
    Root taylor_tree = {};
    taylor_tree.first_node = tree.first_node;

    fprintf (tex_file, "\n\\section{Разложение данной функции в ряд Тейлора.}\n\n");
    int taylor_number = 1;
    printf ("Введите число членов в ряду Тейлора: ");
    scanf ("%d", &taylor_number);
    fprintf (tex_file, "$");
    Print_tex (tex_file, 0, taylor_tree.first_node);
    fprintf (tex_file, " = ");

    int index = 0;

    fprintf (tex_file, " %lg", Calculate_func (taylor_tree.first_node, 0));
    taylor_tree.first_node = Diff (taylor_tree.first_node, false);

    for (index = 1; index < taylor_number + 1; index++)
    {
        double value = Calculate_func (taylor_tree.first_node, 0);

        if (!is_equal (value, 0))
        {
            if (value < 0)
            {
                fprintf (tex_file, " - \\frac{%lg}{%d}", -value, Factorial (index));
                fprintf (tex_file, "\\cdot x^%d", index);
            }
            else
            {
                fprintf (tex_file, " + \\frac{%lg}{%d}", value, Factorial (index));
                fprintf (tex_file, "\\cdot x^%d", index); 
            }
        }
        taylor_tree.first_node = Diff (taylor_tree.first_node, false); 
        taylor_tree.first_node = Simpler (taylor_tree.first_node);
    }

    fprintf (tex_file, " + o(x^%d) $", taylor_number);


    //построение графика 
    fprintf (tex_file, "\n\\section{График функции и касательной к ней.}\n\n");
    printf ("Введите точку, в которой требуется найти касательную: ");
    x_value = 0;
    scanf ("%lg", &x_value);

    first_diff_tree.first_node = Diff (tree.first_node, false);
    first_diff_tree.first_node = Simpler (first_diff_tree.first_node);

    double f_x = Calculate_func (tree.first_node, x_value);
    double df_x = Calculate_func (first_diff_tree.first_node, x_value);

    FILE *graph_file = fopen ("out\\graph.gpi", "wb");

    if (!graph_file)
        printf ("graph file open error");

    fprintf (tex_file,  "\n\\begin{figure}[ht]\n"
                        "\\center\n"
                        "\\includegraphics[scale=0.65]{graph.png}\n"
                        "\\end{figure}\n");
    
    fprintf (graph_file,    "set terminal png size 800, 600\n"
                            "set output \"out/graph.png\"\n"

                            "set xlabel \"X\"\n"
                            "set ylabel \"Y\"\n"
                            "set grid\n"
                            "set yrange [-5:5]\n"
                            "set xrange[-5:5]\n"
                            "plot ");

    save_node (graph_file, tree.first_node, NULL);
    fprintf (graph_file, ", %lg * (x - %lg) + %lg", df_x, x_value, f_x);
    

    fclose (graph_file);
    system ("gnuplot out\\graph.gpi");

    fprintf (tex_file, "\\end{document}");
    fclose (tex_file);
    system ("pdflatex --output-directory=out out/tex_input.tex > nul");

    printf ("latex compile succes");

    return 0;
}