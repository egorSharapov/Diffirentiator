#include <string.h>
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include "utilities.hpp"
#include "diff_tree.hpp"


//кафедра приветствует тебя, если ты дочитал до этого места
//


Tree_node *New_node (NODE_TYPE type, OPERATORS op_value, const char *variable, double number, 
                    Tree_node *left, Tree_node *right,   size_t str_pos,       size_t str_num)
{
    Tree_node *new_node = (Tree_node *) Safety_calloc (sizeof (Tree_node));

    new_node->type = type;

    new_node->num_value = number;
    new_node->op_value = op_value;
    new_node->var_value = variable;

    new_node->str_position = str_pos;
    new_node->str_number = str_num;

    new_node->left = left;
    new_node->right = right;

    return new_node;
}


Tree_node *New_num (double number, size_t str_pos, size_t str_num)
{
    Tree_node *new_node = (Tree_node *) Safety_calloc (sizeof (Tree_node));

    new_node->type = NUMBER;

    new_node->num_value = number;
    new_node->op_value = OP_NON;
    new_node->var_value = NULL;

    new_node->str_position = str_pos;
    new_node->str_number = str_num;

    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

Tree_node *New_var (const char* variable)
{
    assert (variable);

    Tree_node *new_node = (Tree_node *) Safety_calloc (sizeof (Tree_node));

    new_node->type = VARIABLE;

    new_node->num_value = NAN;
    new_node->op_value = OP_NON;
    new_node->var_value = variable;

    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}


Tree_node *New_operator (OPERATORS op_value, Tree_node *left_son, Tree_node *right_son)
{

    Tree_node *new_node = (Tree_node *) Safety_calloc (sizeof (Tree_node));

    new_node->type = OPERATOR;

    new_node->num_value = NAN;
    new_node->op_value = op_value;
    new_node->var_value = NULL;

    new_node->left = left_son;
    new_node->right = right_son;

    return new_node;
}


static const char *convert_graph_op (OPERATORS op_value)
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
    if (op_value == OP_TAN)
        return "tg";
    if (op_value == OP_COT)
        return "ctg";

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
                fprintf (graph_file, "    node%p [label = \"op: %s L\"]\n", parent, convert_graph_op (parent->op_value));
            else
                fprintf (graph_file, "    node%p [label = \"op: %s R\"]\n", parent, convert_graph_op (parent->op_value));
                
            if (son->type == NUMBER)
                fprintf (graph_file, "    node%p [label = \"num: %lg\"]\n", son, son->num_value);
            else if (son->type == OPERATOR)
                fprintf (graph_file, "    node%p [label = \"op: %s\"]\n", son, convert_graph_op (son->op_value));
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


void Free_tree (Tree_node *node)
{
    if (node)
    {
        if (node->left)
            Free_tree (node->left);
        
        if (node->right)
            Free_tree (node->right);
            
        free (node);
    }
}