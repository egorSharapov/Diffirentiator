#ifndef DIFF_DSL_INCLUDED
#define DIFF_DSL_INCLUDED


#define dL Diff(node->left, is_dump_to_tex, diff_var)
#define dR Diff(node->right, is_dump_to_tex, diff_var)
#define cL Copy(node->left)
#define cR Copy(node->right)
#define Add(L, R) New_operator(OP_ADD, L, R)
#define Sub(L, R) New_operator(OP_SUB, L, R)
#define Mul(L, R) New_operator(OP_MUL, L, R)
#define Div(L, R) New_operator(OP_DIV, L, R)
#define Sin(R)    New_operator(OP_SIN, NULL, R)
#define Cos(R)    New_operator(OP_COS, NULL, R)
#define Pwr(L, R) New_operator(OP_PWR, L, R)
#define Ln(R)     New_operator(OP_LN,  NULL, R)
#define Log(R)    New_operator(OP_LOG, NULL, R)

#define TEX_DIFF_SUBTREE(type) \
    if (is_dump_to_tex)        \
    Tex_subtree(tex_file, type, node)

#define TEX_NUM(value)  \
    if (is_dump_to_tex) \
    fprintf(tex_file, "%d$\n\n", value)

#define TYPE_IS(son, node_type) node->son->type == node_type
#define OP_IS(type) node->op_value == type
#define IS_VAL(son, val) is_equal (node->son->num_value, val)

#define FREE_AND_MODIFICATE(free_son, swap_son) \
{                                     \
    Tree_node *temp = node->swap_son; \
    free (node->free_son);            \
    free (node);                      \
    node = temp;                      \
    *is_modifed = true;               \
}


#define CALC_L Calculate_func (node->left, x_value, f_constants)
#define CALC_R Calculate_func (node->right, x_value, f_constants)



#endif