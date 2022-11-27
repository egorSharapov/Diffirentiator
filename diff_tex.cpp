#include <assert.h>
#include <string.h>
#include "diff_tex.hpp"
#include "diffirentiator.hpp"
#include "utilities.hpp"

extern FILE *tex_file;

static void Taylor_series_loop (Root *taylor_tree, int n_taylor, Constants *f_constants)
{
    assert (taylor_tree);

    int index = 0;
    for (index = 1; index < n_taylor + 1; index++)
    {
        double value = Calculate_func (taylor_tree->first_node, 0, f_constants);
        if (isnan (value))
            break;
        if (!is_equal (value, 0))
        {
            if (value < 0)
                fprintf (tex_file, " - \\frac{%lg}{%d}", -value, Factorial (index));
            else
                fprintf (tex_file, " + \\frac{%lg}{%d}", value, Factorial (index));
            fprintf (tex_file, "\\cdot x^%d", index);
        }
        taylor_tree->first_node = Diff (taylor_tree->first_node, false); 
        taylor_tree->first_node = Simpler (taylor_tree->first_node);
    }

    fprintf (tex_file, " + o(x^%d) $", n_taylor);
}


void Taylor_series_calculation (Text *text, Root* tree, Constants *f_constants)
{
    assert (text);
    assert (tree);

    Root taylor_tree = {};
    taylor_tree.first_node = tree->first_node;

    int n_taylor = 1;

    if (sscanf (text->meta_string[taylor_series_str].string_point, "Number of taylor series: %d", &n_taylor) != 1)
    {
        printf ("incorrect number of taylor series");
        
        printf ("Введите число членов в ряду Тейлора: ");
        scanf ("%d", &n_taylor);
    }

    fprintf (tex_file, "\n\\section{Разложение данной функции в ряд Тейлора.}\n\n");

    fprintf (tex_file, "$");
    Print_tex (tex_file, 0, taylor_tree.first_node);
    fprintf (tex_file, " = ");


    fprintf (tex_file, " %lg", Calculate_func (taylor_tree.first_node, 0, f_constants));

    taylor_tree.first_node = Diff (taylor_tree.first_node, false);
    taylor_tree.first_node = Simpler (taylor_tree.first_node);

    Taylor_series_loop (&taylor_tree, n_taylor, f_constants);

}


static void Configure_graph_file (FILE *graph_file)
{
    fprintf (tex_file,  "\n\\begin{figure}[ht]\n"
                        "\\center\n"
                        "\\includegraphics[scale=0.65]{graph.png}\n"
                        "\\end{figure}\n");
    
    fprintf (graph_file,    "set terminal png size 800, 600\n"
                            "set output \"out/graph.png\"\n"

                            "set xlabel \"x\"\n"
                            "set ylabel \"f(x)\"\n"
                            "set grid\n");
}

void Graph_plotter (Text *text, Root *tree, const char *graph_file_name, Constants *f_constants)
{
    assert (text);
    assert (tree);
    assert (graph_file_name);

    Root first_diff_tree = {};
    double tangent_point = 0;

    double x_min = -5,
           x_max = 5,
           y_min = -5,
           y_max = 5;

    FILE *graph_file = fopen (graph_file_name, "wb");

    if (!graph_file)
        printf ("graph file open error");

    if (sscanf (text->meta_string[tangent_point_str].string_point, "Tangent point: %lg", &tangent_point) != 1)
    {
        printf ("incorrect tangent point\n");
        printf ("Введите точку, в которой требуется найти касательную: ");
        scanf ("%lg", &tangent_point);
    }

    fprintf (tex_file, "\n\\section{Уравнение касательной функции в точке x = %lg.}\n\n", tangent_point);

    first_diff_tree.first_node = Diff (tree->first_node, false);
    first_diff_tree.first_node = Simpler (first_diff_tree.first_node);

    double f_x = Calculate_func (tree->first_node, tangent_point, f_constants);
    double df_x = Calculate_func (first_diff_tree.first_node, tangent_point, f_constants);

    if (f_x - df_x * tangent_point < 0)
        fprintf (tex_file, "g(x) = $%lg \\cdot x %lg $", df_x, f_x - df_x * tangent_point);
    else
        fprintf (tex_file, "g(x) = $%lg \\cdot  x + %lg $", df_x, f_x - df_x * tangent_point);

    fprintf (tex_file, "\n\\section{График функции и касательной к ней.}\n\n");


    if (sscanf (text->meta_string[graph_range_str].string_point, "Graph range: (%lg,    %lg),  (%lg,    %lg)", 
                                                                                &x_min, &x_max, &y_min, &y_max) != 4)
    {
        printf ("Введите границы построения графика в формате (x_min, x_max), (y_min, y_max):\n");
        scanf (" (%lg, %lg), (%lg, %lg)", &x_min, &x_max, &y_min, &y_max);
    }

    Configure_graph_file (graph_file);

    fprintf (graph_file, "set yrange [%lg:%lg]\n"
                         "set xrange[%lg:%lg]\n"
                         "plot ", y_min, y_max, x_min, x_max);

    save_node (graph_file, tree->first_node, NULL, f_constants);
    if (!isnan (df_x) and !isnan (f_x))
        fprintf (graph_file, ", %lg * (x - %lg) + %lg", df_x, tangent_point, f_x);
    fclose (graph_file);

    char *command = (char *) Safety_calloc (sizeof ("gnuplot ") + sizeof (graph_file_name));
    sprintf (command, "gnuplot %s", graph_file_name);
    system (command);

}


void Calculate_function_in_point (Text *text, Root *tree, Constants *f_constants)
{
    assert (text);
    assert (tree);

    double f_x_value = 0;

    if (sscanf (text->meta_string[func_calc_str].string_point, "Function calculation point: %lg", &f_x_value) != 1)
    {
        printf ("Введите точку, в которой требуется вычислить значение функции: ");
        scanf ("%lg", &f_x_value);
    }

    fprintf (tex_file, "\n\\section{Вычисление значения функции в точке.}\n\n");

    fprintf (tex_file, "$ f(%lg) = %lg $", f_x_value, Calculate_func (tree->first_node, f_x_value, f_constants));
}


void Calculate_derivative_in_point (Text *text, Root *diff_tree, Constants *f_constants)
{
    assert (text);
    assert (diff_tree);

    double df_x_value = 0;

    if (sscanf (text->meta_string[derivative_calc_str].string_point, "Derivative calculation point: %lg", &df_x_value) != 1)
    {
        printf ("Введите точку, в которой требуется вычислить значение производной: ");
        scanf ("%lg", &df_x_value);
    }

    fprintf (tex_file, "\n\\section{Вычисление производной функции в точке.}\n\n");

    double f_x_value = Calculate_func (diff_tree->first_node, df_x_value, f_constants);

    fprintf (tex_file, "$ f^{'}(%lg) = %lg $", df_x_value, f_x_value);
}


static void Simplification_in_tex (Root *diff_tree)
{
    assert (diff_tree);

    fprintf (tex_file, "\n\\section{Упрощение.}\n\n");

    Graph_print_tree (diff_tree);
    fprintf (tex_file, "$");
    Print_tex (tex_file, 0, diff_tree->first_node);
    fprintf (tex_file, " = ");

    diff_tree->first_node = Simpler (diff_tree->first_node);
    Print_tex (tex_file, 0, diff_tree->first_node);
    fprintf (tex_file, "$");
}


void Taking_nth_derivative (Text *text, Root *tree, Root *diff_tree)
{
    assert (text);
    assert (diff_tree);

    int derivative_n = 0;
    Root next_diff_tree = {};

    diff_tree->first_node = tree->first_node;

    if (sscanf (text->meta_string[derivative_n_str].string_point, "Derivative power: %d", &derivative_n) != 1)
    {
        printf ("Введите степень производной: ");
        scanf ("%d", &derivative_n);
    }

    int t_derivative_n = derivative_n;

    fprintf (tex_file, "\n\\section{Взятие %d-ой производной по x.}\n\n", derivative_n);
    
    fprintf (tex_file, "$ f(x) = ");
    Print_tex (tex_file, 0, tree->first_node);
    fprintf (tex_file, "$\n\n");

    Graph_print_tree (tree);

    bool show_steps = true;

    if (derivative_n > 2)
        show_steps = false;
    
    while (derivative_n--)
    {
        diff_tree->first_node = Simpler (diff_tree->first_node);
        next_diff_tree.first_node = Diff (diff_tree->first_node, show_steps);
        diff_tree->first_node = Copy (next_diff_tree.first_node);
        Free_tree (next_diff_tree.first_node);
    }

    fprintf (tex_file, "Получаем:\n\n");
    fprintf (tex_file, "$ f^{(%d)}(x) = ", t_derivative_n);
    Print_tex (tex_file, 0, diff_tree->first_node);
    fprintf (tex_file, "$\n");

    Simplification_in_tex (diff_tree);

}


void Calculate_full_derivative (Root *tree, Constants *f_constants)
{
    Root temp_diff_tree = {};

    fprintf (tex_file, "\n\\section{Взятие полной производной}\n\n");
    fprintf (tex_file, "$F(x");

    for (size_t index = 0; index < f_constants->capacity; index++)
    {
        char *var_name = f_constants->list[index].var_name;
        fprintf (tex_file, ", %s", var_name);
    }
    fprintf (tex_file, ") = \\sqrt{");

    temp_diff_tree.first_node = Diff (tree->first_node, 0);
    temp_diff_tree.first_node = Simpler (temp_diff_tree.first_node);

    fprintf (tex_file, "(");
    Print_tex (tex_file, 0, temp_diff_tree.first_node);
    fprintf (tex_file, " \\cdot \\Delta x)^2");

    for (size_t index = 0; index < f_constants->capacity; index++)
    {
        char *var_name = f_constants->list[index].var_name;
        temp_diff_tree.first_node = Diff (tree->first_node, 0, var_name);
        temp_diff_tree.first_node = Simpler (temp_diff_tree.first_node);

        fprintf (tex_file, " + (");
        Print_tex (tex_file, 0, temp_diff_tree.first_node);
        fprintf (tex_file, " \\cdot \\Delta %s)^2", var_name);

    }
    fprintf (tex_file, "} $\n");
}