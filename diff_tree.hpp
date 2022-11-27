#ifndef DIFF_TREE_HPP_INCLUDED
#define DIFF_TREE_HPP_INCLUDED

#include <stdio.h>


enum NODE_TYPE
{
    VARIABLE = 0,
    NUMBER   = 1,
    OPERATOR = 2,
};


enum OPERATORS
{
    OP_NON  = 0,
    OP_ADD  = 1,
    OP_SUB  = 2,
    OP_MUL  = 3,
    OP_DIV  = 4,
    OP_SIN  = 5,
    OP_COS  = 6,
    OP_PWR  = 7,
    OP_LN   = 8,
    OP_TAN  = 9,
    OP_COT  = 10,
    OP_LOG = 11,
    OP_LBRC = 11,
    OP_RBRC = 12,
    OP_END = 13,
};

typedef const char* Elem_t;


struct Constant
{
    char  *var_name   = NULL;
    double var_value = 0;
};


struct Constants
{
    size_t capacity = 0;
    Constant *list = NULL;
};



struct Tree_node
{
    enum NODE_TYPE type;

    size_t str_position = 0;
    size_t str_number   = 0;

    const char* var_value;
    double      num_value;
    enum OPERATORS op_value;

    Tree_node *left  = NULL;
    Tree_node *right = NULL;

};


typedef struct
{
    size_t number_of_elem = 0;
    Constants *f_constants = NULL;
    Tree_node *first_node = NULL;

} Root;


union value
{
    double num;
    const char *var;
    OPERATORS op;
};

Tree_node *New_num      (double number, size_t str_pos = 0, size_t str_num = 0);
Tree_node *New_var      (const char *variable);
Tree_node *New_operator (OPERATORS op_value, Tree_node *left, Tree_node *right);

Tree_node *New_node     (NODE_TYPE type, OPERATORS op_value, const char *variable, double number, 
                        Tree_node *left, Tree_node *right,   size_t str_pos,       size_t str_num);

Tree_node *Find_node_by_value (Tree_node *parent, Elem_t value);
// OPERATORS Find_operator (const char *op_value);

void show_tree (Root *tree_root);
void Print_node (const Tree_node *node);
void Free_tree (Tree_node *node);
void Graph_print_tree (Root *tree_root);
#endif