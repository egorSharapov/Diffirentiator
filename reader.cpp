#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <windows.h>
#include "reader.hpp"
#include "utilities.hpp"

const int op_number = 12;

const char *valid_operators[op_number] = {"sin",  "cos",  "+",    "-",    "*",    "/",    "^",    "(",     ")",    "ln",  "log",   "tg"};
OPERATORS   valid_enum_op  [op_number] = {OP_SIN, OP_COS, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_PWR, OP_LBRC, OP_RBRC, OP_LN, OP_LOG, OP_TAN};


static Tree_tokens *Tokens_ctor (size_t size)
{
    Tree_tokens *tokens = (Tree_tokens *) Safety_calloc (sizeof (Tree_tokens));
    tokens->capacity = 0;
    tokens->size = 0;
    tokens->array = (Tree_node **) Safety_calloc (sizeof (Tree_node *) * size);

    return tokens;
}


static void Skip_spaces (const char **str)
{
    while (**str == ' ')
        *str += 1;
}

Tree_tokens *Tokenizer (const char **str)
{

    Tree_tokens *tokens = Tokens_ctor (strlen (*str));
    Tree_node   *node   = NULL;

    while (**str != '\0')
    {
        Skip_spaces (str);

        if ((node = Is_operator (str)))
            tokens->array[tokens->capacity++] = node;

        else if ((node = Is_num (str)))
            tokens->array[tokens->capacity++] = node;

        else if ((node = Is_variable (str)))
            tokens->array[tokens->capacity++] = node;

        else if (**str != '\0')
            printf ("lex error\n");

    }

    tokens->array[tokens->capacity++] = New_operator (OP_END, NULL, NULL);


    return tokens;
}


Tree_node *Is_num (const char **str)
{
    double number = 0;
    int  lenght = 0;

    if (sscanf (*str, "%lg%n", &number, &lenght) == 1)
    {
        *str += lenght;
        return New_num (number);
    }
    return NULL;
}

Tree_node *Is_variable (const char **str)
{
    char *var_value = (char *) Safety_calloc (strlen (*str));

    if (sscanf (*str, " %[\\a-z]", var_value) == 1)
    {
        *str += strlen (var_value);
        return New_var (var_value);
    }

    free (var_value);

    return NULL;
}
//strdl
//

Tree_node *Is_operator (const char **str)
{
    for (int index = 0; index < op_number; index++)
    {
        size_t size = strlen (valid_operators[index]);

        if (strncmp (*str, valid_operators[index], size) == 0)
        {
            *str += size;
            return New_operator (valid_enum_op[index], NULL, NULL);
        }
    }
    return NULL;
}


Tree_node* Get_G (Tree_tokens *tokens)
{
    assert (tokens);

    Tree_node* first_node = Get_E (tokens);

    if (IS_TYPE(OPERATOR) and IS_OP (OP_END))
        printf ("succes syntax analize\n");

    return first_node;
}


Tree_node* Get_E (Tree_tokens *tokens)
{
    Tree_node* node_left = Get_T (tokens);

    while (IS_TYPE (OPERATOR) and (IS_OP (OP_ADD) or IS_OP (OP_SUB)))
    {
        OPERATORS temp_op = tokens->array[tokens->size]->op_value;
        tokens->size += 1;

        Tree_node* node_right = Get_T (tokens);

        node_left = New_operator (temp_op, node_left, node_right);

    }
    return node_left;
}


Tree_node* Get_T (Tree_tokens *tokens)
{
    Tree_node* node_left = Get_W (tokens);

    while (IS_TYPE (OPERATOR) and (IS_OP (OP_MUL) or IS_OP (OP_DIV)))
    {
        OPERATORS temp_op = tokens->array[tokens->size]->op_value;
        tokens->size += 1;
        Tree_node* node_right = Get_W (tokens);

        node_left = New_operator (temp_op, node_left, node_right);
    }
    return node_left;
}


Tree_node* Get_P (Tree_tokens *tokens)
{
    Tree_node* val = NULL;

    if (IS_TYPE (OPERATOR) and IS_OP (OP_LBRC))
    {
        tokens->size += 1;
        val = Get_E (tokens);

        if (!(IS_TYPE (OPERATOR) and IS_OP (OP_RBRC)))
            printf ("syntax error");
        else
            tokens->size += 1;
    }
    else if (IS_TYPE (NUMBER))
        val = Get_N (tokens);
    else if (IS_TYPE (VARIABLE))
        val = Get_V (tokens);
    else
        printf ("get P error\n");

    return val;
}


Tree_node* Get_N (Tree_tokens *tokens)
{
    if (IS_TYPE (NUMBER))
        return tokens->array[tokens->size++];
    else
        printf ("get number error");

    return NULL;
}

Tree_node* Get_V (Tree_tokens *tokens)
{
    if (IS_TYPE (VARIABLE))
        return tokens->array[tokens->size++];
    else
        printf ("get variable error");
    
    return NULL;
}


Tree_node* Get_W (Tree_tokens *tokens)
{
    Tree_node* node_left = Get_F (tokens);
    
    if (IS_TYPE (OPERATOR) and IS_OP (OP_PWR))
    {
        tokens->size += 1;

        Tree_node* node_right = Get_F (tokens);

        return New_operator (OP_PWR, node_left, node_right);
    }
    return node_left;
}


Tree_node *Get_F (Tree_tokens *tokens)
{
    Tree_node* node_right = NULL;

    if (IS_TYPE (OPERATOR) and (IS_UNARY()))
    {
        OPERATORS temp_op = tokens->array[tokens->size]->op_value;
        tokens->size += 1;
        if (!(IS_TYPE (OPERATOR) and IS_OP (OP_LBRC)))
            printf ("syntax error");
        else
            tokens->size += 1;

        node_right = New_operator (temp_op, NULL, Get_E (tokens));
        tokens->size += 1;
    }
    else
        node_right = Get_P (tokens);

    return node_right;
}