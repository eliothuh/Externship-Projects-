#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "value.h"
#include "talloc.h"


// Create a new NULL_TYPE value node.
Value *makeNull()
{
    Value* node = talloc(sizeof(Value));
    node->type = NULL_TYPE;
    return node;
}

// Create a new CONS_TYPE value node.
// newCar: a pointer to a value
// newCdr: a pointer to a value
// return: a CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr)
{
    // assert (newCdr->type == CONS_TYPE || newCdr->type == NULL_TYPE);
    // assert (newCar->type != CONS_TYPE || newCdr->type != NULL_TYPE);
    Value* node = talloc(sizeof(Value));
    node->type = CONS_TYPE;
    //if newCar points to a string, make a copy of that string.
    if (newCar->type ==  STR_TYPE)
    {
        char *str = newCar->s;
        char *copy = talloc((strlen(str)+1) * sizeof(char));
        for (int i = 0; i<strlen(str); i++)
        {
            copy[i] = str[i];
        }
        copy[strlen(str)] = '\0';
        newCar->s = copy;
        node->c.car = newCar;
    }
    else
    {
        node->c.car = newCar;
    }
    node->c.cdr = newCdr;
    return node;
}


// Utility to make it less typing to get car value.
// list:  a CONS_TYPE value
Value *car(Value *list)
{
    assert(list->type == CONS_TYPE);
    return list->c.car;
}

// Utility to make it less typing to get cdr value.
// list:  a CONS_TYPE value
Value *cdr(Value *list)
{
    assert(list->type == CONS_TYPE);
    return list->c.cdr;
}

// Utility to check if pointing to a NULL_TYPE value.
// value:  a Value Struct
bool isNull(Value *value)
{
    return (value->type == NULL_TYPE);
}

// Measure length of list
// value:  a Value Struct
int length(Value *value)
{
    if(isNull(value))
    {
        return 0;
    }
    else if(value->type == CONS_TYPE)
    {
        return (1 + length(cdr(value)));
    }
    else
    {
        return 1;
    }
}

//helper method for display. Displays the middle of a given list.
//list: a CONS_TYPE value
void displayhelper(Value *list)
{
    switch(list->type)
    {
        case INT_TYPE:
            printf("%i ", list->i);
            break;
        case DOUBLE_TYPE:
            printf("%f ", list->d);
            break;
        case STR_TYPE:
            printf("%s ", list->s);
            break;
        case NULL_TYPE:
            printf("() ");
            break;
        case CONS_TYPE:
            if(list->c.car->type == CONS_TYPE)
            {
              printf("( ");
              displayhelper(list->c.car);
              printf(") ");
            }
            else
            {
              displayhelper(list->c.car);
            }
            if(!isNull(list->c.cdr))
            {
              displayhelper(list->c.cdr);
            }
            break;
        case PTR_TYPE:
            printf("%p ", list->p);
            break;
        case OPEN_TYPE:
            printf("(\n");
            break;
        case CLOSE_TYPE:
            printf(")\n");
            break;
        case BOOL_TYPE:
            break;
        case SYMBOL_TYPE:
            printf("%s ", list->s);
            break;
        case SINGLEQUOTE_TYPE:
            break;
        case OPENBRACKET_TYPE:
            break;
        case CLOSEBRACKET_TYPE:
            break;
        case DOT_TYPE:
            break;
        case VOID_TYPE:
            break;
        case CLOSURE_TYPE:
            break;
        case PRIMITIVE_TYPE:
            break;
    }
}

// Display the contents of the linked list to the screen in some kind of
// readable format
//list: a CONS_TYPE value
void display(Value *list)
{
//   printf("( ");
  displayhelper(list);
//   printf(")\n");
}

//Return a new list that is the reverse of the one that is passed in.
//list: a CONS_TYPE or NULL_TYPE value
Value *reverse(Value *list)
{
    assert(list->type == CONS_TYPE || list->type == NULL_TYPE);
    Value *reversed_list = makeNull();
    //if the list is not empty
    //we continue to make copy of the items in the original list
    //and update our reversed list.
    while(length(list)>0)
    {
        Value *current_item = list->c.car;
        reversed_list = cons(current_item, reversed_list);
        list = list->c.cdr;
    }
   return reversed_list;
}


//make a copy of the list, and make the last thing of the list point to tail
//list: a CONS_TYPE value
Value *append_helper(Value *list, Value *tail)
{
    // assert(list->type == CONS_TYPE || list->type == NULL_TYPE);
    if (list->type == NULL_TYPE)
    {
        return tail;
    }
    Value *list_copy;
    Value *current_item = list->c.car;
    Value *item_copy = talloc(sizeof(Value));
    switch(current_item->type)
    {
        case INT_TYPE:
            item_copy->type = INT_TYPE;
            item_copy->i = current_item->i;
            list_copy = cons(item_copy, append_helper(list->c.cdr, tail));
            break;
        case DOUBLE_TYPE:
            item_copy->type = DOUBLE_TYPE;
            item_copy->d = current_item->d;
            list_copy = cons(item_copy, append_helper(list->c.cdr, tail));
            break;
        case STR_TYPE:
            item_copy->type = STR_TYPE;
            //make a copy of the string in the original list
            char* str = talloc((strlen(current_item->s)+1)*sizeof(char));
            for (int i = 0; i<strlen(current_item->s); i++)
            {
                str[i] = current_item->s[i];
            }
            str[strlen(current_item->s)] = '\0';
            item_copy->s = str;
            list_copy = cons(item_copy, append_helper(list->c.cdr, tail));
            //free up the first string copy that we create
            free(str);
            break;
        case NULL_TYPE:
            break;
        case CONS_TYPE:
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            printf("(\n");
            break;
        case CLOSE_TYPE:
            printf(")\n ");
            break;
        case BOOL_TYPE:
            break;
        case SYMBOL_TYPE:
            break;
        case SINGLEQUOTE_TYPE:
            break;
        case OPENBRACKET_TYPE:
            break;
        case CLOSEBRACKET_TYPE:
            break;
        case DOT_TYPE:
            break;
        case VOID_TYPE:
            break;
        case CLOSURE_TYPE:
            break;
        case PRIMITIVE_TYPE:
            break;
    }
    return list_copy;
}

//Append list2 to list1, assuming both are proper lists.
//list1: a CONS_TYPE value
//list2: a CONS_TYPE value
Value *append(Value *list1, Value *list2)
{
    assert (list1->type == CONS_TYPE || list1->type == NULL_TYPE);
    assert (list2->type == CONS_TYPE || list2->type == NULL_TYPE);

    if (list1->type == NULL_TYPE)
    {
        Value *list2_copy = append_helper(list2, makeNull());
        return list2_copy;
    }
    else
    {
        if (list2->type == NULL_TYPE)
        {
            Value *list1_copy = append_helper(list1, makeNull());
            return list1_copy;
        }
        else if(list2->type != CONS_TYPE)
        {
            Value *list_copy = append_helper(list1, list2);
            return list_copy;
        }
        else
        {
            Value *list2_copy = append_helper(list2, makeNull());
            Value *list_copy = append_helper(list1, list2_copy);
            return list_copy;
        }
    }
  }
  //helper method for the list function. Returns a copy of a value
  //that is not of type cons
  Value *copy_value(Value *val)
  {
    //create a pointer to the copy to be returned.
    Value *item_copy = talloc(sizeof(Value));
    //instantiate the copy with the proper type and data.=
    switch(val->type)
    {
        case INT_TYPE:
            item_copy->type = INT_TYPE;
            item_copy->i = val->i;
            break;
        case DOUBLE_TYPE:
            item_copy->type = DOUBLE_TYPE;
            item_copy->d = val->d;
            break;
        case STR_TYPE:
            item_copy->type = STR_TYPE;
            //make a copy of the string referred to by the input value
            char* str = talloc((strlen(val->s)+1)*sizeof(char));
            for (int i = 0; i<strlen(val->s); i++)
            {
                str[i] = val->s[i];
            }
            str[strlen(val->s)] = '\0';
            item_copy->s = str;
            break;
        case NULL_TYPE:
            break;
        case CONS_TYPE:
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            printf("(\n");
            break;
        case CLOSE_TYPE:
            printf(")\n ");
            break;
        case BOOL_TYPE:
            break;
        case SYMBOL_TYPE:
            break;
        case SINGLEQUOTE_TYPE:
            break;
        case OPENBRACKET_TYPE:
            break;
        case CLOSEBRACKET_TYPE:
            break;
        case DOT_TYPE:
            break;
        case VOID_TYPE:
            break;
        case CLOSURE_TYPE:
            break;
        case PRIMITIVE_TYPE:
            break;
    }
    return item_copy;
  }

//Returns a list containing list1 and list2 as values
//list1: a CONS_TYPE value
//list2: a CONS_TYPE value
  Value *list(Value *value1, Value *value2)
  {
    //make copies of value1 and value2
    Value *value1_copy;
    Value *value2_copy;
    if (value1->type == NULL_TYPE || value1->type == CONS_TYPE)
    {
      value1_copy = append_helper(value1, makeNull());
    }
    else
    {
      value1_copy = copy_value(value1);
    }
    if (value2->type == NULL_TYPE || value2->type == CONS_TYPE)
    {
      value2_copy = append_helper(value2, makeNull());
    }
    else
    {
      value2_copy = copy_value(value2);
    }
    //create a cons cell with the car pointing to the copy of value2
    //and the cdr pointing to a null value
    Value *cons2 = talloc(sizeof(Value));
    cons2->type = CONS_TYPE;
    cons2->c.car = value2_copy;
    cons2->c.cdr = makeNull();

    //create a cons cell with the car pointing to the copy of value1
    //and the cdr pointing to the cons2.
    Value *cons1 = talloc(sizeof(Value));
    cons1->type = CONS_TYPE;
    cons1->c.car = value1_copy;
    cons1->c.cdr = cons2;

    return cons1;
  }
