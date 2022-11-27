#include <ctype.h>
#include <time.h>
#include <sys\stat.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include "file_analize.hpp"
#include "utilities.hpp"
#include "diffirentiator.hpp"
#include "reader.hpp"

const char *keywords[keywords_number] = {"Очевидно, что:",
                                         "Заметим, что:", 
                                         "Нетрудно заметить:", 
                                         "Не требует дальнейших комментариев:", 
                                         "Увидим, что:",
                                         "Из этого исходит, что:",
                                         "Можно убедиться, что:",
                                         "Кто бы мог подумать что:"};

//--------------------------------------------------------------------------------------------------------------------

static size_t file_size_count (const char * file_name)
{
    assert (file_name);

    struct stat buff = {};
    buff.st_size = 0;

    stat(file_name, &buff);

    return buff.st_size;
}

//--------------------------------------------------------------------------------------------------------------------

void counting_strings (Text * text)
{
    assert (text);

    text->source [text->count_of_symbols] = '\0';
    
    char * point = text->source;
    
    while ((point = strchr (point, '\n')) != NULL)
    {
        if (*(point - 1) == 13)
            *(point - 1) = ' ';
        
        *point = '\0';
        point++;
        text->count_of_strings++;
    }
    text->count_of_strings++;

}

//--------------------------------------------------------------------------------------------------------------------

ERRORS_CODE count_and_read (const char *file_name, Text * text)
{
    assert (text);
    assert (file_name);

    FILE * input_file = fopen (file_name, "rb");

    if (!input_file)
        return FALL;

    size_t file_size = file_size_count (file_name);
    
    text->source = (char *) Safety_calloc (file_size + 2);

    if (text->source == NULL)
        return NO_MEM_ERROR;
    
    text->count_of_symbols = fread (text->source, sizeof(char), file_size, input_file);

    if (file_size != text->count_of_symbols)
        return FALL;

    counting_strings (text);
    
    if (fclose (input_file) != 0)
    {
        printf ("close file %s error", file_name);
        return FALL;
    }

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------------

void create_pointers (Text * text)
{
    assert (text);

    text->meta_string = (String *) Safety_calloc ((text->count_of_strings + 1) * sizeof (String));
    size_t index_of_string = 0;
    char * point = text->source;

    text->meta_string [index_of_string].string_point = point;
    while (index_of_string++ < text->count_of_strings)
    {
        point += strlen (point);
        point += 1;
        text->meta_string [index_of_string].string_point = point;
        text->meta_string [index_of_string - 1].string_len = point - text->meta_string [index_of_string - 1].string_point - 2;
    }

}

//--------------------------------------------------------------------------------------------------------------------

void save_tree (const char* file_name, Root* tree_root)
{
    FILE *output_file = fopen (file_name, "a");
    if (!output_file)
        printf ("open file error");

    save_node (output_file, tree_root->first_node, NULL, NULL);
    fprintf (output_file, "\n");

    if (fclose (output_file) != 0)
        printf ("close file error");
}

//--------------------------------------------------------------------------------------------------------------------

void save_node (FILE* output_file, Tree_node* node, Tree_node *parent, Constants *f_constants)
{
    if (node)
    {
        if (parent and node->op_value and node->op_value < parent->op_value)
            fprintf (output_file, "(");

        if (parent and is_unary (parent->op_value))
            fprintf (output_file, "(");

        if (!is_unary (node->op_value))
            save_node (output_file, node->left, node, f_constants);

    
        if (node->type == OPERATOR)
            fprintf (output_file, "%s", convert_op (node->op_value));

        if (node->type == NUMBER)
        {
            if (node->num_value < 0)
                fprintf (output_file, "(%lg)", node->num_value);
            else
                fprintf (output_file, "%lg", node->num_value);
        }
        if (node->type == VARIABLE)
        {
            if (*node->var_value == 'x')
                fprintf (output_file, "%s", node->var_value); 
            else 
                fprintf (output_file, "%lg", Find_const_by_name (node->var_value, f_constants));
        }


        save_node (output_file, node->right, node, f_constants);

        if (parent and is_unary (parent->op_value))
            fprintf (output_file, ")");
        
        if (parent and node->op_value and node->op_value < parent->op_value)
            fprintf (output_file, ")");

        
    }
}

//--------------------------------------------------------------------------------------------------------------------

static void Read_constants (Constants *constants, Text *text)
{
    size_t constants_str = 8;
    double var_value = 0;

    constants->list = (Constant *) Safety_calloc ((text->count_of_strings - constants_str) * sizeof (Constant));
    constants->capacity = text->count_of_strings - constants_str;

    while (text->count_of_strings != constants_str)
    {
        char *var = (char *) Safety_calloc (max_cmd_size);

        if (sscanf (text->meta_string[constants_str].string_point, "%s = %lg", var, &var_value) != 2)
            printf ("incorrect input\n");

        constants->list[text->count_of_strings - constants_str - 1].var_name = var;
        constants->list[text->count_of_strings - constants_str - 1].var_value = var_value;

        constants_str++;
    }
}

//--------------------------------------------------------------------------------------------------------------------

void read_t_file (Text *text, const char *t_file_name, Root *tree_root, Constants *constants)
{
    assert (text);
    assert (t_file_name);
    assert (tree_root);


    count_and_read (t_file_name, text);
    create_pointers (text);

    const char *temp_source = text->source;

    Tree_tokens *tokens = Tokenizer (&temp_source);

    tree_root->first_node = Get_G (tokens);

    Read_constants (constants, text);

}

//--------------------------------------------------------------------------------------------------------------------

static void Print_node_value (const Tree_node *node, FILE *output_file)
{
    if (node->type == OPERATOR)
        fprintf (output_file, "%s", convert_tex_op (node->op_value));

    else if (node->type == NUMBER)
    {
        if (node->num_value < 0)
            fprintf (output_file, "(%lg)", node->num_value);
        else
            fprintf (output_file, "%lg", node->num_value);
        }
    else if (node->type == VARIABLE)
        fprintf (output_file, "%s", node->var_value); 
            
}

//--------------------------------------------------------------------------------------------------------------------

static void Tex_node (FILE* output_file, const Tree_node* node, const Tree_node *parent)
{
    assert (output_file);

    if (node)
    {

        fprintf (output_file, "{");
        
        if (parent and node->type == OPERATOR and node->op_value < parent->op_value)
            fprintf (output_file, "(");
        else if (parent and is_unary (parent->op_value))
            fprintf (output_file, "(");
            
        Tex_node (output_file, node->left, node);

        Print_node_value (node, output_file);

        Tex_node (output_file, node->right, node);

        if (parent and is_unary (parent->op_value))
            fprintf (output_file, ")");
        else if (parent and node->type == OPERATOR and node->op_value < parent->op_value)
            fprintf (output_file, ")");
        
        fprintf (output_file, "}");
    }
}

//--------------------------------------------------------------------------------------------------------------------

void Print_tex (FILE *tex_file, bool is_diff, const Tree_node *node, int diff_power)
{
    assert (node);
    assert (tex_file);

    if (is_diff)
        fprintf (tex_file, "{(");

    Tex_node (tex_file, node, NULL);

    if (is_diff and diff_power == 1)
        fprintf (tex_file, ")}^{\'}");
    else if (is_diff)
        fprintf (tex_file, ")}^{%d}", diff_power);

}

//--------------------------------------------------------------------------------------------------------------------

static void Tex_mul_subtree (FILE *tex_file, const Tree_node *node, bool direction)
{
    Print_tex (tex_file, direction, node->left);
    fprintf (tex_file, "\\cdot ");
    Print_tex (tex_file, !direction, node->right); 
}

//--------------------------------------------------------------------------------------------------------------------

void Tex_subtree (FILE *tex_file, OPERATORS op_value, const Tree_node *node)
{
    assert (tex_file);
    assert (node);

    switch (op_value)
    {
    case OP_ADD:
    {
        Print_tex (tex_file, 1, node->left);
        fprintf (tex_file, " + ");
        Print_tex (tex_file, 1, node->right);
        break;
    }
    case OP_SUB:
    {
        Print_tex (tex_file, 1, node->left);
        fprintf (tex_file, " - ");
        Print_tex (tex_file, 1, node->right);
        break;
    }
    case OP_MUL:
    {
        Tex_mul_subtree (tex_file, node, 1);
        fprintf (tex_file, " + "); 
        Tex_mul_subtree (tex_file, node, 0);
        break; 
    }
    case OP_DIV:
    {
        fprintf (tex_file, "{{");
        Tex_mul_subtree (tex_file, node, 1);
        fprintf (tex_file, " - "); 
        Tex_mul_subtree (tex_file, node, 0);
        fprintf (tex_file, "} \\over {(");
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, ")}^2}");
        break;
    }
    case OP_SIN:
    {
        fprintf (tex_file, "cos(");
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, ") \\cdot");
        Print_tex (tex_file, 1, node->right);
        break;
    }
    case OP_COS:
    {
        fprintf (tex_file, "- \\sin (");
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, ") \\cdot");
        Print_tex (tex_file, 1, node->right);
        break;
    }
    case OP_PWR:
    {
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, "\\cdot {");
        Print_tex (tex_file, 0, node->left);
        fprintf (tex_file, "}^{");
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, " - 1} \\cdot");
        Print_tex (tex_file, 1, node->left);
        break;
    }
    case OP_LN:
    {
        fprintf (tex_file, "{{");
        Print_tex (tex_file, 1, node->right);
        fprintf (tex_file, " } \\over {");
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, "}}");
        break;
    }
    case OP_TAN:
    {
        fprintf (tex_file, "{");
        Print_tex (tex_file, 1, node->right);
        fprintf (tex_file, " \\over cos^2(");
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, ")}");
        break;
    }
    case OP_COT:
    {
        fprintf (tex_file, " - {");
        Print_tex (tex_file, 1, node->right);
        fprintf (tex_file, " \\over sin^2(");
        Print_tex (tex_file, 0, node->right);
        fprintf (tex_file, ")}");
        break;
    }
    case OP_NON:
    {
        printf ("Invalid operator for tex");
        break;
    }
    default:
        break;
    }

    fprintf (tex_file, "$\n\n"); 
}

//--------------------------------------------------------------------------------------------------------------------

void Print_tex_keywords (FILE *tex_file)
{
    assert (tex_file);

    int random_keyword = rand() % keywords_number;

    fprintf (tex_file, "%s\n\n", keywords[random_keyword]);
}

//--------------------------------------------------------------------------------------------------------------------

void Print_tex_title (FILE *tex_file, Root *tree_root, Constants *f_constants)
{
    assert (tex_file);

    fprintf (tex_file,  "\\documentclass{article}\n"
                        "\\usepackage[utf8]{inputenc}\n"
                        "\\usepackage[T2A]{fontenc}\n"
                        "\\usepackage[russian]{babel}\n"
                        "\\usepackage{amsthm}\n"
                        "\\usepackage{amssymb}\n"
                        "\\usepackage[a1paper]{geometry}\n"
                        "\\geometry{papersize={29.7 cm, 25.0 cm}}\n"
                        "%% \\usepackage{tikz}\n"
                        "\\usepackage{textcomp}\n"
                        "%% \\usepackage{marvosym}\n"
                        "%% \\usepackage{ esint }\n"
                        "\\usepackage{graphicx}"
                        "\\usepackage{amsfonts}\n"
                        "\\setlength{\\topmargin}{-0.5in}\n"
                        "\\setlength{\\textheight}{8.1in}\n"
                        "\\setlength{\\oddsidemargin}{-0.4in}\n"
                        "\\setlength{\\evensidemargin}{-0.4in}\n"
                        "\\setlength{\\textwidth}{7in}\n"
                        "\\setlength{\\parindent}{0ex}\n"
                        "\\setlength{\\parskip}{1ex}\n"
                        "\\title{Полный анализ функции}"
                        "\\begin{document}\n\n"
                        "\\maketitle");
        
    fprintf (tex_file,  "\n\\renewcommand{\\abstractname}{Введение}"
                        "\\begin{abstract}"
                        "Данный документ содержит полный анализ функции с разложением в ряд Тейлора, построением графика"
                        " и взятием полной производной."
                        "\\end{abstract}");
    
    fprintf (tex_file, "\n\\section*{Исходная функция.}\n\n");

    fprintf (tex_file, "$f(x");
    for (size_t index = 0; index < f_constants->capacity; index++)
    {
        fprintf (tex_file, ", %s", f_constants->list[index].var_name);
    }
    fprintf (tex_file, ") = ");

    Print_tex (tex_file, 0, tree_root->first_node);
    fprintf (tex_file, "$\n");

}

//--------------------------------------------------------------------------------------------------------------------

void Print_tex_end (FILE *tex_file)
{
    fprintf (tex_file, "\\end{document}");
    fclose (tex_file);

    system ("pdflatex --output-directory=out out/tex_input.tex > out/tex_log.txt");

    printf ("latex compile succes");

}






// //--------------------------------------------------------------------------------------------------------------------

// Tree_node *read_leather (const char** source)
// {
//     assert (source);

//     double number = 0;
//     char *symbol  = (char *) Safety_calloc (max_cmd_size);

//     if (sscanf (*source, " (%lf)", &number))
//     {
//         *source = strchr (*source, ')') + 1;
//         return New_num (number);
//     }
//     else if (sscanf (*source, " (%1s)", symbol))
//     {
//         if (strcmp (symbol, "c") == 0 or strcmp (symbol, "s") == 0 or 
//             strcmp (symbol, "l") == 0 or strcmp (symbol, "(") == 0 or
//             strcmp (symbol, "t") == 0)

//             return read_node (source);

//         *source = strchr (*source, ')') + 1;
//         return New_var (symbol);
//     }

//     return NULL;
// }

// //--------------------------------------------------------------------------------------------------------------------

// Tree_node* read_node (const char **source)
// {
//     assert (source);

//     char op_value[5] = {0};
//     Tree_node* parent = NULL;
//     int read_counter = 0;

//     if (strchr (*source, '(') < strchr (*source, ')'))
//     {
//         *source = strchr (*source, '(') + 1;

//         Tree_node* left = read_leather (source);

//         sscanf (*source, " %s%n ", op_value, &read_counter);
//         OPERATORS enum_op_value = Find_operator (op_value);
//         parent = New_operator (enum_op_value, left, NULL);
//         *source += read_counter;

//         parent->right = read_leather (source);

//         *source = strchr (*source, ')') + 1;
//     }


//     return parent;

// }

// //--------------------------------------------------------------------------------------------------------------------

// char * create_dir_name (const char *file_path, const char *file_name)
// {
//     assert (file_path);
//     assert (file_name);

//     char *file_dir = (char *) Safety_calloc (strlen (file_path) + strlen (file_name) + 1);

//     strcpy (file_dir, file_path);
//     strcat (file_dir, file_name);

//     return file_dir;
// }