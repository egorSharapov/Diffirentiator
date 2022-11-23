#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include "utilities.hpp"
#include "file_analize.hpp"
#include "diffirentiator.hpp"
#include "diff_tree.hpp"

extern FILE *tex_file;

#define dL Diff(node->left, is_dump_to_tex)
#define dR Diff(node->right, is_dump_to_tex)
#define cL Copy(node->left)
#define cR Copy(node->right)
#define Add(L, R) New_operator("+", L, R)
#define Sub(L, R) New_operator("-", L, R)
#define Mul(L, R) New_operator("*", L, R)
#define Div(L, R) New_operator("\\", L, R)
#define Sin(R) New_operator("sin", NULL, R)
#define Cos(R) New_operator("cos", NULL, R)
#define Pwr(L, R) New_operator("^", L, R)

#define TEX_DIFF_SUBTREE(type) \
    if (is_dump_to_tex)        \
    Tex_subtree(tex_file, type, node)

#define TEX_NUM(value)  \
    if (is_dump_to_tex) \
    fprintf(tex_file, "%d$\n\n", value)

Tree_node *Diff(const Tree_node *node, bool is_dump_to_tex, const char diff_var)
{
    assert (node);

    if (is_dump_to_tex)
    {
        Print_tex_keywords(tex_file);
        fprintf(tex_file, "$");
        Print_tex(tex_file, 1, node);
        fprintf(tex_file, " = ");
    }

    switch (node->type)
    {
    case NUMBER:
    {
        TEX_NUM(0);
        return New_num(0);
    }
    case VARIABLE:
    {
        if (*node->var_value != diff_var)
        {
            TEX_NUM(0);
            return New_num(0);
        }

        TEX_NUM (1);
        return New_num(1);
    }
    case OPERATOR:
    {
        switch (node->op_value)
        {
        case OP_ADD:
        {
            TEX_DIFF_SUBTREE(OP_ADD);
            return Add(dL, dR);
        }
        case OP_SUB:
        {
            TEX_DIFF_SUBTREE(OP_SUB);
            return Sub(dL, dR);
        }
        case OP_MUL:
        {
            TEX_DIFF_SUBTREE(OP_MUL);
            return Add(Mul(dL, cR), Mul(cL, dR));
        }
        case OP_DIV:
        {
            TEX_DIFF_SUBTREE(OP_DIV);
            return Div(Sub(Mul(dL, cR), Mul(cL, dR)), Mul(cR, cR));
        }
        case OP_SIN:
        {
            TEX_DIFF_SUBTREE(OP_SIN);
            return Mul(Cos(cR), dR);
        }
        case OP_COS:
        {
            TEX_DIFF_SUBTREE(OP_COS);
            return Mul(Mul(Sin(cR), New_num(-1)), dR);
        }
        case OP_PWR:
        {
            TEX_DIFF_SUBTREE(OP_PWR);
            if (node->right->type == NUMBER)
                return Mul(Mul(cR, Pwr(cL, Sub(cR, New_num(1)))), dL);

            // else if (node->right->type == NUMBER)
            //     return Mul (Mul ())
            break;

        }
        case OP_LN:
        {
            TEX_DIFF_SUBTREE (OP_LN);
            return Div (dR, cR);
        }
        case OP_NON:
            return NULL;

        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    return NULL;
}

Tree_node *Constant_simplification(Tree_node *node, bool *is_modifed)
{
    assert (node);
    assert (is_modifed);

    if (node->type == OPERATOR and !is_unary (node->op_value))
    {
        node->left =  Constant_simplification (node->left, is_modifed);
        node->right = Constant_simplification (node->right, is_modifed);

        if (node->left->type == NUMBER and node->right->type == NUMBER)
        {
            double temp_left = node->left->num_value;
            double temp_right = node->right->num_value;

            free(node->left);
            free(node->right);
            free(node);
            *is_modifed = true;

            if (node->op_value == OP_ADD)
                node = New_num(temp_right + temp_left);

            if (node->op_value == OP_MUL)
                node = New_num(temp_right * temp_left);

            if (node->op_value == OP_SUB)
                node = New_num(temp_left - temp_right);

            if (node->op_value == OP_DIV)
                node = New_num(temp_left / temp_right);
        }
    }

    return node;
}


#define TYPE_IS(son, node_type) node->son->type == node_type
#define OP_IS(type) node->op_value == type
#define IS_VAL(son, val) is_equal (node->son->num_value, val)

#define FREE_AND_MODIFICATE(free_son, swap_son) \
    Tree_node *temp = node->swap_son; \
    free (node->free_son);            \
    free (node);                      \
    node = temp;                      \
    *is_modifed = true;


static Tree_node *Removal_neutral_elements(Tree_node *node, bool *is_modifed)
{
    assert (node);
    assert (is_modifed);

    if (node->type == OPERATOR and !is_unary (node->op_value))
    {
        node->left =  Removal_neutral_elements(node->left, is_modifed);
        node->right = Removal_neutral_elements(node->right, is_modifed);

        if (TYPE_IS (left, NUMBER) and OP_IS (OP_MUL) and IS_VAL (left, 0))
        {
            FREE_AND_MODIFICATE (right, left);
        }
        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_MUL) and IS_VAL (right, 0))
        {
            FREE_AND_MODIFICATE (left, right);
        }
        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_ADD) and IS_VAL (right, 0))
        {
            FREE_AND_MODIFICATE (right, left);
        }
        else if (TYPE_IS (left, NUMBER) and OP_IS (OP_ADD) and IS_VAL (left, 0))
        {
            FREE_AND_MODIFICATE (left, right);
        }
        else if (TYPE_IS (left, NUMBER) and OP_IS (OP_MUL) and IS_VAL (left, 1))
        {
            FREE_AND_MODIFICATE (left, right);
        }
        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_MUL) and IS_VAL (right, 1))
        {
            FREE_AND_MODIFICATE (right, left);
        }
        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_MUL) and !(TYPE_IS (left, NUMBER)))
        {
            Tree_node *temp = node->left;
            node->left = node->right;
            node->right = temp;
        }
    }

    return node;
}

Tree_node *Simpler(Tree_node *node)
{
    assert (node);

    bool is_modifed = true;

    while (is_modifed)
    {
        is_modifed = false;
        node = Constant_simplification(node, &is_modifed);
        node = Removal_neutral_elements (node, &is_modifed);
    }
    return node;
}

Tree_node *Copy(Tree_node *node)
{
    if (node)
    {
        Tree_node *copy_node = (Tree_node *) Safety_calloc (sizeof (Tree_node));

        copy_node->type = node->type;

        if (node->type == VARIABLE)
        {
            char *temp_var = (char *) Safety_calloc (sizeof(char) * max_cmd_size);
            temp_var = strcpy(temp_var, node->var_value);
            copy_node->var_value = temp_var;
        }
        else 
            copy_node->var_value = NULL;

        copy_node->num_value = node->num_value;
        copy_node->op_value = node->op_value;

        copy_node->right = Copy(node->right);
        copy_node->left = Copy(node->left);

        return copy_node;
    }

    return NULL;
}


#define CALC_L Calculate_func (node->left, x_value)
#define CALC_R Calculate_func (node->right, x_value)

double Calculate_func (Tree_node *node, double x_value)
{
    assert (node);

    switch (node->type)
    {
    case NUMBER:
        return node->num_value;
    case VARIABLE:
        return x_value;
    case OPERATOR:
    {
        switch (node->op_value)
        {
            case OP_ADD: return (CALC_L + CALC_R);
            case OP_MUL: return (CALC_L * CALC_R);
            case OP_SUB: return (CALC_L - CALC_R);
            case OP_DIV: return (CALC_L / CALC_R);

            case OP_SIN: return sin (CALC_R);
            case OP_COS: return cos (CALC_R);
            case OP_PWR: return pow (CALC_L, CALC_R);
            case OP_LN:  return log (CALC_R);

            case OP_NON: printf ("invalid operator");
            default:
                break;
        }
        break;
    }
    default:
        break;
    }

    return NAN;
}



int Factorial (int number)
{
    if (number > 0)
        return (number*Factorial (number - 1));
    
    return 1;
}


void Taylor_series_calculation (Text *text, Root* tree)
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

    int index = 0;

    fprintf (tex_file, " %lg", Calculate_func (taylor_tree.first_node, 0));

    taylor_tree.first_node = Diff (taylor_tree.first_node, false);
    taylor_tree.first_node = Simpler (taylor_tree.first_node);

    for (index = 1; index < n_taylor + 1; index++)
    {
        double value = Calculate_func (taylor_tree.first_node, 0);

        if (!is_equal (value, 0))
        {
            if (value < 0)
                fprintf (tex_file, " - \\frac{%lg}{%d}", -value, Factorial (index));
            else
                fprintf (tex_file, " + \\frac{%lg}{%d}", value, Factorial (index));
            fprintf (tex_file, "\\cdot x^%d", index);
        }
        taylor_tree.first_node = Diff (taylor_tree.first_node, false); 
        taylor_tree.first_node = Simpler (taylor_tree.first_node);
    }

    fprintf (tex_file, " + o(x^%d) $", n_taylor);

}


void Graph_plotter (Text *text, Root *tree, const char *graph_file_name)
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
        printf ("incorrect tangent point");

        printf ("Введите точку, в которой требуется найти касательную: ");
        scanf ("%lg", &tangent_point);
    }


    fprintf (tex_file, "\n\\section{Уравнение касательной функции в точке x = %lg.}\n\n", tangent_point);

    first_diff_tree.first_node = Diff (tree->first_node, false);
    first_diff_tree.first_node = Simpler (first_diff_tree.first_node);

    double f_x = Calculate_func (tree->first_node, tangent_point);
    double df_x = Calculate_func (first_diff_tree.first_node, tangent_point);

    fprintf (tex_file, "g(x) = $%lg \\cdot x + %lg $", df_x, f_x - df_x * tangent_point);

    fprintf (tex_file, "\n\\section{График функции и касательной к ней.}\n\n");


    if (sscanf (text->meta_string[graph_range_str].string_point, "Graph range: (%lg,    %lg),  (%lg,    %lg)", 
                                                                                &x_min, &x_max, &y_min, &y_max) != 4)
    {
        printf ("incorrect graph range");

        printf ("Введите границы построения графика в формате (x_min, x_max), (y_min, y_max):\n");
        scanf (" (%lg, %lg), (%lg, %lg)", &x_min, &x_max, &y_min, &y_max);
    }


    fprintf (tex_file,  "\n\\begin{figure}[ht]\n"
                        "\\center\n"
                        "\\includegraphics[scale=0.65]{graph.png}\n"
                        "\\end{figure}\n");
    
    fprintf (graph_file,    "set terminal png size 800, 600\n"
                            "set output \"out/graph.png\"\n"

                            "set xlabel \"X\"\n"
                            "set ylabel \"Y\"\n"
                            "set grid\n"
                            "set yrange [%lg:%lg]\n"
                            "set xrange[%lg:%lg]\n"
                            "plot ", y_min, y_max, x_min, x_max);

    save_node (graph_file, tree->first_node, NULL);
    
    fprintf (graph_file, ", %lg * (x - %lg) + %lg", df_x, tangent_point, f_x);
    

    fclose (graph_file);

    char *command = (char *) Safety_calloc (sizeof ("gnuplot ") + sizeof (graph_file_name));
    sprintf (command, "gnuplot %s", graph_file_name);
    system (command);

}


void Calculate_function_in_point (Text *text, Root *tree)
{
    assert (text);
    assert (tree);

    double f_x_value = 0;

    if (sscanf (text->meta_string[func_calc_str].string_point, "Function calculation point: %lg", &f_x_value) != 1)
    {
        printf ("incorrect point format\n");

        printf ("Введите точку, в которой требуется вычислить значение функции: ");
        scanf ("%lg", &f_x_value);
    }

    fprintf (tex_file, "\n\\section{Вычисление значения функции в точке.}\n\n");

    fprintf (tex_file, "$ f(%lg) = %lg $", f_x_value, Calculate_func (tree->first_node, f_x_value));
}


void Calculate_derivative_in_point (Text *text, Root *diff_tree)
{
    assert (text);
    assert (diff_tree);

    double df_x_value = 0;

    if (sscanf (text->meta_string[derivative_calc_str].string_point, "Derivative calculation point: %lg", &df_x_value) != 1)
    {
        printf ("incorrect point format\n");

        printf ("Введите точку, в которой требуется вычислить значение производной: ");
        scanf ("%lg", &df_x_value);
    }

    fprintf (tex_file, "\n\\section{Вычисление производной функции в точке.}\n\n");
    
    fprintf (tex_file, "$ f^{'}(%lg) = %lg $", df_x_value, Calculate_func (diff_tree->first_node, df_x_value));
}


void Taking_nth_derivative (Text *text, Root *tree, Root *diff_tree)
{
    assert (text);
    assert (diff_tree);
    
    int derivative_n = 0;
    Root next_diff_tree = {};

    diff_tree->first_node = tree->first_node;
    next_diff_tree.first_node = Copy (diff_tree->first_node);

    if (sscanf (text->meta_string[derivative_n_str].string_point, "Derivative power: %d", &derivative_n) != 1)
    {
        printf ("incorrect derivative power\n");

        printf ("Введите степень производной: ");
        scanf ("%d", &derivative_n);
    }

    int t_derivative_n = derivative_n;

    fprintf (tex_file, "\n\\section{Взятие %d-ой производной.}\n\n", derivative_n);
    
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

    fprintf (tex_file, "\n\\section{Упрощение.}\n\n");

    Graph_print_tree (diff_tree);
    fprintf (tex_file, "$");
    Print_tex (tex_file, 0, diff_tree->first_node);
    fprintf (tex_file, " = ");

    diff_tree->first_node = Simpler (diff_tree->first_node);
    Print_tex (tex_file, 0, diff_tree->first_node);
    fprintf (tex_file, "$");

}
    