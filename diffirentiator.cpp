#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include "utilities.hpp"
#include "file_analize.hpp"
#include "diffirentiator.hpp"
#include "diff_tree.hpp"

extern FILE *tex_file;

//--------------------------------------------------------------------------------------------------------------------

Tree_node *Diff(const Tree_node *node, bool is_dump_to_tex, const char *diff_var)
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
        if (strcmp (node->var_value, diff_var))
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
        case OP_LOG:
        {
            TEX_DIFF_SUBTREE(OP_LOG);
            return Div (dR, Mul (cR, Log (New_num (10))));
        }
        case OP_PWR:
        {
            TEX_DIFF_SUBTREE(OP_PWR);
            if (node->right->type == NUMBER)
                return Mul(Mul(cR, Pwr(cL, Sub(cR, New_num(1)))), dL);

            else if (node->left->type == NUMBER)
                return Mul (Ln (cL), Mul (Pwr (cL, cR), dR));
            
            return Mul (Add (Mul (dR, Ln (cL)), Mul (cR, Div (dL, cR))), Pwr (cL, cR));
        }
        case OP_LN:
        {
             TEX_DIFF_SUBTREE (OP_LN);
            return Div (dR, cR);
        }
        case OP_TAN:
        {
            TEX_DIFF_SUBTREE (OP_TAN);
            return Div (dR, Pwr (Cos (cR), New_num (2)));
        }
        case OP_COT:
        {
            TEX_DIFF_SUBTREE (OP_COT);
            return Div (dR, Pwr (Sin (cR), New_num (2)));
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

//--------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------

static Tree_node *Removal_neutral_elements(Tree_node *node, bool *is_modifed)
{
    assert (node);
    assert (is_modifed);

    if (node->type == OPERATOR and !is_unary (node->op_value))
    {
        node->left =  Removal_neutral_elements(node->left, is_modifed);
        node->right = Removal_neutral_elements(node->right, is_modifed);

        if (TYPE_IS (left, NUMBER) and OP_IS (OP_MUL) and IS_VAL (left, 0))
            FREE_AND_MODIFICATE (right, left)

        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_MUL) and IS_VAL (right, 0))
            FREE_AND_MODIFICATE (left, right)

        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_ADD) and IS_VAL (right, 0))
            FREE_AND_MODIFICATE (right, left)

        else if (TYPE_IS (left, NUMBER) and OP_IS (OP_ADD) and IS_VAL (left, 0))
            FREE_AND_MODIFICATE (left, right)

        else if (TYPE_IS (left, NUMBER) and OP_IS (OP_MUL) and IS_VAL (left, 1))
            FREE_AND_MODIFICATE (left, right)

        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_MUL) and IS_VAL (right, 1))
            FREE_AND_MODIFICATE (right, left)

        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_PWR) and IS_VAL (right, 1))
            FREE_AND_MODIFICATE (right, left)

        else if (TYPE_IS (left, NUMBER) and OP_IS (OP_DIV) and IS_VAL (left, 0))
            FREE_AND_MODIFICATE (right, left)

        else if (TYPE_IS (right, NUMBER) and OP_IS (OP_MUL) and !(TYPE_IS (left, NUMBER)))
        {
            Tree_node *temp = node->left;
            node->left = node->right;
            node->right = temp;
        }
    }

    return node;
}

//--------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------

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
        if (node->type == NUMBER)
            copy_node->num_value = node->num_value;

        if (node->type == OPERATOR)
            copy_node->op_value = node->op_value;


        copy_node->right = Copy(node->right);
        copy_node->left = Copy(node->left);

        return copy_node;
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------

double Calculate_func (Tree_node *node, double x_value, Constants *f_constants)
{
    assert (node);

    switch (node->type)
    {
    case NUMBER:
        return node->num_value;
    case VARIABLE:
    {
        if (*node->var_value == 'x')
            return x_value;
        else 
            return Find_const_by_name (node->var_value, f_constants);
    }
        
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
            case OP_TAN: return tan (CALC_R);
            case OP_COT: return tan (M_PI/2 - CALC_R);

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




double Find_const_by_name (const char *var_name, Constants *f_constants)
{
    for (size_t index = 0; index < f_constants->capacity; index++)
    {
        if (strcmp (var_name, f_constants->list[index].var_name) == 0)
            return f_constants->list[index].var_value;
    }
    return NAN;
}
