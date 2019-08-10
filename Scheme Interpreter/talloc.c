#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "value.h"
#include "linkedlist.h"

// A global variable for active list
Value* active_list;

// The Replacement for malloc. Like malloc, talloc allocates memory and returns a pointer 
// to it. Additionally, it stores the pointers in a linked list called active list.
void *talloc(size_t size)
{
    //if the active list is not initialised
    if(active_list == NULL)
    {
        active_list = malloc(sizeof(Value));
        active_list->type = NULL_TYPE;
        active_list->marked = false;
    }

    //allocate memory for a node 
    void *node = malloc(size);

    //create a pointer type value that points to the node
    Value *pointer = malloc(sizeof(Value));
    pointer -> type = PTR_TYPE;
    pointer -> p = node;
    pointer->marked = false;

    //add the pointer to active list 
    Value *cons_node = malloc(sizeof(Value));
    cons_node -> type = CONS_TYPE;
    cons_node -> c.car = pointer;
    cons_node -> c.cdr = active_list;
    cons_node->marked = false;
    active_list = cons_node;

    return node;
}

//This function takes a value as input and mark things differently according to the type of value
void mark(Value *value)
{
    //mark the console in our active list whose car is a pointer
    //that points to value
    Value *current_node = active_list;
    while(current_node->c.car->p != value){
        current_node = current_node->c.cdr;
    }
    current_node->marked = true;

    //If the input value is a string type
    if(value->type == STR_TYPE){
        //we additionally mark the console in our active list 
        //whose car is a pointer that points to actual string
        current_node = active_list;
        while(current_node->c.car->p != value->s){
            current_node = current_node->c.cdr;
        }
        current_node->marked = true;
    }

    //If the input value is a cons type, which means we pass a linked list
    //we recursively mark everything in the linked list
    if(value->type == CONS_TYPE)
    {
        mark(value->c.car);
        mark(value->c.cdr);
    }
}

// This helper function free everything in the input list except the last element. 
Value *tfree_helper(Value *head)
{
    if (head->type == NULL_TYPE)
    {
        return head;
    }
    else if(head->marked == false)
    {
        Value *new_list = head->c.cdr;
        Value *node = head->c.car->p;
        //free the memory that the pointer points to
        free(node);
        //free the pointer
        free(head->c.car);
        //free the head of of the list
        free(head);
        //recursively free the rest of the list
        return tfree_helper(new_list);
    }
    else
    {
        head->c.cdr = tfree_helper(head->c.cdr);
        return head;
    }
}

// This function free all unmarked pointers allocated by talloc, whatever memory that is
// allocated in lists to hold those unmarked pointers and unmark all the marked values
void tfree()
{
    printf("length of the active linkedlist before freeing those unmarked: %d\n", length(active_list));
    active_list = tfree_helper(active_list);
    printf("length of the active linkedlist after freeing those unmarked: %d\n", length(active_list));
    if(active_list->type == NULL_TYPE)
    {
        free(active_list);
        active_list = NULL;
    }
    //unmark all the marked values
    else
    {
        Value *current_node = active_list;
        while(current_node->type != NULL_TYPE)
        {
            current_node->marked = false;
            current_node = current_node->c.cdr;
        }
    }
}

// This function free all the unmarked memory and exit.
void texit(int status)
{
    tfree();
    tfree();
    exit(status);
}
