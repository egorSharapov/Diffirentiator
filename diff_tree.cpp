#include <string.h>
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include "diff_tree.hpp"



Tree_node *New_num (double number)
{
    Tree_node *new_node = (Tree_node *) calloc (sizeof (Tree_node), 1);

    new_node->type = NUMBER;
    new_node->num_value = number;
    new_node->op_value = OP_NON;
    new_node->var_value = NULL;

    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

Tree_node *New_var (const char* variable)
{
    Tree_node *new_node = (Tree_node *) calloc (sizeof (Tree_node), 1);

    new_node->type = VARIABLE;
    new_node->num_value = NAN;
    new_node->op_value = OP_NON;
    new_node->var_value = variable;

    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

static OPERATORS Find_operator (const char *op_value)
{
    if (strcmp (op_value, "+") == 0)
        return OP_ADD;
    if (strcmp (op_value, "-") == 0)
        return OP_SUB;
    if (strcmp (op_value, "*") == 0)
        return OP_MUL;  
    if (strcmp (op_value, "\\") == 0)
        return OP_DIV;
    if (strcmp (op_value, "sin") == 0)
        return OP_SIN;
    if (strcmp (op_value, "cos") == 0)
        return OP_COS;
    if (strcmp (op_value, "^") == 0)
        return OP_PWR;
    if (strcmp (op_value, "ln") == 0)
        return OP_LN;
    
    return OP_NON;
}

Tree_node *New_operator (const char* op_value, Tree_node *left_son, Tree_node *right_son)
{
    Tree_node *new_node = (Tree_node *) calloc (sizeof (Tree_node), 1);

    new_node->type = OPERATOR;
    new_node->num_value = NAN;
    new_node->op_value = Find_operator (op_value);
    new_node->var_value = NULL;

    new_node->left = left_son;
    new_node->right = right_son;

    return new_node;
}


static const char *convert_op (OPERATORS op_value)
{
    if (op_value == OP_ADD)
        return " + ";

    if (op_value == OP_SUB)
        return "-";

    if (op_value == OP_MUL)
        return " * ";
    
    if (op_value == OP_DIV)
        return " \\\\ ";

    if (op_value == OP_SIN)
        return "sin";
    
    if (op_value == OP_COS)
        return "cos";
    
    if (op_value == OP_PWR)
        return "^";

    if (op_value == OP_LN)
        return "ln";
    
    return "non operator";
}


static void Graph_print_node (FILE* graph_file, Tree_node *parent, Tree_node *son)
{

    assert (graph_file);

    if (son)
    {
        if (parent)
        {
            if (parent->left == son)
                fprintf (graph_file, "    node%p [label = \"op: %s L\"]\n", parent, convert_op (parent->op_value));
            else
                fprintf (graph_file, "    node%p [label = \"op: %s R\"]\n", parent, convert_op (parent->op_value));
                
            if (!isnan (son->num_value))
                fprintf (graph_file, "    node%p [label = \"num: %lg\"]\n", son, son->num_value);
            else if (son->op_value != OP_NON)
                fprintf (graph_file, "    node%p [label = \"op: %s\"]\n", son, convert_op (son->op_value));
            else
                fprintf (graph_file, "    node%p [label = \"var: %s\"]\n", son, son->var_value);

            fprintf (graph_file, "    node%p -> node%p\n", parent, son);
        }
        Graph_print_node (graph_file, son, son->left);
        Graph_print_node (graph_file, son, son->right);

    }
}

void Graph_print_tree (Root *tree_root)
{
    assert (tree_root);

    FILE *graph_file = fopen ("out\\input.gv", "wb");
    
    if (graph_file == NULL)
        printf ("open %s error", "out\\input.dot");

    fprintf (graph_file, "digraph {\n");
    fprintf (graph_file, "    labelloc = \"t\"\n");
    fprintf (graph_file, "    fontsize = \"70\"\n");
    fprintf (graph_file, "    nodesep = 0.4\n");
    fprintf (graph_file, "    node [shape = record]\n");
    fprintf (graph_file, "    splines=ortho\n    rankdir=TB\n");

    Graph_print_node (graph_file, NULL, tree_root->first_node);

    fprintf (graph_file, "}");
    
    if (fclose (graph_file) != 0)
        printf ("graph file close error");
    
    system ("dot out\\input.gv -Tpng -o out\\output.png");
  
}