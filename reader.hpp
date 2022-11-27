#ifndef READER_HPP_INCLUDED
#define READER_HPP_INCLUDED

#include "diff_tree.hpp"

#define IS_TYPE(type_val) tokens->array[tokens->size]->type == type_val
#define IS_OP(op_val)     tokens->array[tokens->size]->op_value == op_val
#define IS_UNARY()        IS_OP (OP_SIN) or IS_OP (OP_COS) or IS_OP (OP_LN) or IS_OP (OP_TAN)

struct Tree_tokens
{
    size_t size = 0;
    size_t capacity = 0;
    Tree_node **array = NULL;
    Tree_node *current = NULL;
};

Tree_node* Get_G (Tree_tokens *tokens);
Tree_node* Get_E (Tree_tokens *tokens);
Tree_node* Get_T (Tree_tokens *tokens);
Tree_node* Get_P (Tree_tokens *tokens);
Tree_node* Get_N (Tree_tokens *tokens);
Tree_node* Get_W (Tree_tokens *tokens);
Tree_node* Get_V (Tree_tokens *tokens);
Tree_node* Get_F (Tree_tokens *tokens);

Tree_tokens *Tokenizer (const char **str);
Tree_node *Is_num      (const char **str);
Tree_node *Is_variable (const char **str);
Tree_node *Is_operator (const char **str);

#endif