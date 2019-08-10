#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"

void printTreeHelper(Value *list, bool space)
{
    switch(list->type)
    {
        case INT_TYPE:
            printf("%i", list->i);
            if (space)
            {
                printf(" ");
            }
            break;
        case DOUBLE_TYPE:
            printf("%f", list->d);
            if (space)
            {
                printf(" ");
            }
            break;
        case STR_TYPE:
            printf("%s", list->s);
            if (space)
            {
                printf(" ");
            }
            break;
        case NULL_TYPE:
            printf("()");
            if (space)
            {
                printf(" ");
            }
            break;
        case CONS_TYPE:
            if(list->c.car->type == CONS_TYPE)
            {
              printf("(");
              printTreeHelper(list->c.car, true);
              printf(")");
              if (list->c.cdr->type != NULL_TYPE)
              {
                printf(" ");
                if(cdr(list)->type != CONS_TYPE)
                {
                    printf(". ");
                    printTreeHelper(list->c.cdr, false);
                }
                else
                {
                    printTreeHelper(list->c.cdr, true);
                }
                
              }
            }
            else if(list->c.cdr->type == NULL_TYPE)
            {
                printTreeHelper(list->c.car, false);
            }
            else if(cdr(list)->type != CONS_TYPE)
            {
                printTreeHelper(list->c.car, true);
                printf(". ");
                printTreeHelper(list->c.cdr, false);
            }
            else
            {
              printTreeHelper(list->c.car, true);
              printTreeHelper(list->c.cdr, true);
            }
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            break;
        case CLOSE_TYPE:
            break;
        case BOOL_TYPE:
            printf("%s", list->s);
            if (space)
            {
                printf(" ");
            }
            break;
        case SYMBOL_TYPE:
            printf("%s", list->s);
            if (space)
            {
                printf(" ");
            }
            break;
        case SINGLEQUOTE_TYPE:
            printf("'");
            break;
        case OPENBRACKET_TYPE:
            break;
        case CLOSEBRACKET_TYPE:
            break;
        case DOT_TYPE:
            printf(".");
            if (space)
            {
                printf(" ");
            }
            break;
        case VOID_TYPE:
            break;
        case CLOSURE_TYPE:
            printf("#<procedure>");
            if (space)
            {
                printf(" ");
            }
            break;
        case PRIMITIVE_TYPE:
            break;
    }
}

//Display a tree
void printTree(Value *list)
{
    printTreeHelper(list, true);
}

//function to add the token to a parse tree 
//tree: a parse tree
//depth: the number of unclosed open parentheses
//token: a token value 
Value* addToParseTree(Value* tree, int* depth, Value* token)
{
    //if the token is not a close type, push it to the tree/stack
    if (token->type!= CLOSE_TYPE && token->type!= CLOSEBRACKET_TYPE)
    {
        tree = cons(token, tree);
        if (token->type == OPEN_TYPE || token->type == OPENBRACKET_TYPE)
        {
            *depth = *depth + 1;
        }
    }
    else
    {
        if (tree->type == NULL_TYPE)
        {
            printf("Syntax error: too many close parentheses\n");
            texit(1);
        }        
        //otherwise,  start popping items from your stack until
        //you pop off an open paren and form a list of them as you go)
        *depth = *depth-1;
        Value *item  = tree->c.car;
        tree = tree->c.cdr;
        Value *new_list = makeNull();
        if (token->type == CLOSE_TYPE)
        {
            while (item->type!=OPEN_TYPE)
            {
                if (tree->type == NULL_TYPE)
                {
                    printf("Syntax error: too many close parentheses\n");
                    texit(1);
                }
                new_list = cons(item, new_list);
                item  = tree->c.car;
                tree = tree->c.cdr;
            }
        }
        else
        {
            while (item->type!=OPENBRACKET_TYPE)
            {
                if (tree->type == NULL_TYPE)
                {
                    printf("Syntax error: too many close parentheses\n");
                    texit(1);
                }
                new_list = cons(item, new_list);
                item  = tree->c.car;
                tree = tree->c.cdr;
            }
        }
        //Push that list back on the stack.
        tree = cons(new_list, tree);
    }
    return tree;   
}

//function to create a parse tree
Value *parse(Value *tokens) {

    Value *tree = makeNull();
    int depth = 0;

    Value *current = tokens;
    assert(current != NULL && "Error (parse): null pointer");
    while (current->type != NULL_TYPE) {
        Value *token = car(current);
        tree = addToParseTree(tree, &depth, token);
        current = cdr(current);
    }
    if (depth != 0) {
        printf("Syntax error: not enough close parentheses\n");
        texit(1);
    }
    tree = reverse(tree);
    return tree;
}