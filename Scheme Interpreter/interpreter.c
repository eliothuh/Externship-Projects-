#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "value.h"
#include "talloc.h"
#include "tokenizer.h"
#include "interpreter.h"
#include "linkedlist.h"
#include "parser.h"

Frame* top_frame;

void print(Value *tree)
{
    if(tree->type == CONS_TYPE)
    {
        printf("(");
        printTree(tree);
        printf(")");
    }
    else
    {
        printTree(tree);
    }
    if(tree->type != VOID_TYPE)
    {
        printf("\n");
    }
}

Value *value_copy(Value *val)
  {
    //create a pointer to the copy to be returned.
    Value *item_copy = talloc(sizeof(Value));
    //instantiate the copy with the proper type and data.=
    switch(val->type)
    {
        case INT_TYPE:
            item_copy->type = INT_TYPE;
            item_copy->i = val->i;
            return item_copy;
            break;
        case DOUBLE_TYPE:
            item_copy->type = DOUBLE_TYPE;
            item_copy->d = val->d;
            return item_copy;
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
            return item_copy;
            break;
        case NULL_TYPE:
            break;
        case CONS_TYPE:
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            break;
        case CLOSE_TYPE:
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
    return val;
  }

//A helper function to look up symbol in a given frame
Value *lookUpSymbol(Value *tree, Frame *frame)
{
    while(frame != NULL)
    {
        Value* bindings = frame->bindings;
        while (bindings->type != NULL_TYPE)
        {
            Value *binding = car(bindings);
            if (!strcmp(car(binding)->s, tree->s))
            {
               return car(cdr(binding));
            }
            //look up symbol in the next binding
            bindings = cdr(bindings);
        }
        //look up symbol in the parent frame
        frame = frame->parent;
    }
    //symbol not found
    printf("unrecognized symbol %s\n", tree->s);
    texit(1);
    return tree;
}

Value *primitiveAdd(Value *args) 
{
    double answer = 0;
    Value* num;
    bool isInt = true;
    while (args->type != NULL_TYPE)
    {
        num = car(args);
        if (num->type == DOUBLE_TYPE)
        {
            answer += num->d;
            isInt = false;
        }
        else if (num->type == INT_TYPE)
        {
            answer += num->i;
        }
        else
        {
            printf("bad syntax for add: must be a number\n");
            texit(1);
        }
        args = cdr(args);
    }
    Value* result = talloc(sizeof(Value));
    if(isInt) 
    {
        result->type = INT_TYPE;
        result->i = (int)answer;
    }
    else 
    {
        result->type = DOUBLE_TYPE;
        result->d = answer;
    }
    return result; 
}

Value *primitiveTimes(Value *args) 
{
    if(length(args)<2)
    {
        printf("bad syntax for *\n");
        texit(1);
    }
    double answer = 1;
    Value* num;
    bool isInt = true;
    while (args->type != NULL_TYPE)
    {
        num = car(args);
        if (num->type == DOUBLE_TYPE)
        {
            answer *= num->d;
            isInt = false;
        }
        else if (num->type == INT_TYPE)
        {
            answer *= num->i;
        }
        else
        {
            printf("bad syntax for *: must be a number\n");
            texit(1);
        }
        args = cdr(args);
    }
    Value* result = talloc(sizeof(Value));
    if(isInt) 
    {
        result->type = INT_TYPE;
        result->i = (int)answer;
    }
    else 
    {
        result->type = DOUBLE_TYPE;
        result->d = answer;
    }
    return result; 
}

Value *primitiveMinus(Value *args) 
{
    if(length(args)!=2)
    {
        printf("bad syntax for -: expected 2 numbers\n");
        texit(1);
    }
    Value* result = talloc(sizeof(Value));
    Value* first = car(args);
    Value* second = car(cdr(args));
    if(first->type == INT_TYPE && second->type == INT_TYPE )
    {
        result->type = INT_TYPE;
        result->i = first->i - second->i;
    }
    else if(first->type == DOUBLE_TYPE && second->type == INT_TYPE)
    {
        result->type = DOUBLE_TYPE;
        result->d = first->d - second->i;
    }
    else if(first->type == INT_TYPE && second->type == DOUBLE_TYPE)
    {
        result->type = DOUBLE_TYPE;
        result->d = first->i - second->d;
    }
    else if(first->type == DOUBLE_TYPE && second->type == DOUBLE_TYPE)
    {
        result->type = DOUBLE_TYPE;
        result->d = first->d - second->d;
    }
    else
    {
        printf("Take numeric arguments\n");
        texit(1);
    }
    return result; 
}

Value *primitiveDivide(Value *args) 
{
    if(length(args)<2)
    {
        printf("bad syntax for /\n");
        texit(1);
    }
    if((car(cdr(args))->type == DOUBLE_TYPE && car(cdr(args))->d == 0)
    || (car(cdr(args))->type == INT_TYPE && car(cdr(args))->i == 0))
    {
        printf("/: division by zero\n");
        texit(1);
    }
    Value* result = talloc(sizeof(Value));
    Value* first = car(args);
    Value* second = car(cdr(args));
    if(first->type == INT_TYPE && second->type == INT_TYPE )
    {
        if (first->i % second->i == 0)
        {
            result->type = INT_TYPE;
            result->i = first->i / second->i;
        }
        else
        {
            result->type = DOUBLE_TYPE;

            result->d = (double)first->i / second->i;
        }  
    }
    else if(first->type == DOUBLE_TYPE && second->type == INT_TYPE)
    {
        result->type = DOUBLE_TYPE;
        result->d = first->d / second->i;
    }
    else if(first->type == INT_TYPE && second->type == DOUBLE_TYPE)
    {
        result->type = DOUBLE_TYPE;
        result->d = first->i / second->d;
    }
    else if(first->type == DOUBLE_TYPE && second->type == DOUBLE_TYPE)
    {
        result->type = DOUBLE_TYPE;
        result->d = first->d / second->d;
    }
    else
    {
        printf("Take numeric arguments\n");
        texit(1);
    }
    return result; 
}

//functions that process >, >=, <, <=, =
Value *primitiveCompare(Value *args, char* symbol)
{
    if(length(args)!=2)
    {
        printf("bad syntax for <\n");
        texit(1);
    }
    Value* result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    Value *first = car(args);
    Value* second = car(cdr(args));
    if((first->type != DOUBLE_TYPE && first->type != INT_TYPE)
    || (second->type != DOUBLE_TYPE && second->type != INT_TYPE))
    {
        printf("Take numeric arguments\n");
        texit(1);
    }
    else if((first->type == DOUBLE_TYPE && second->type == INT_TYPE 
    && (first->d < second->i)) || (first->type == DOUBLE_TYPE && second->type == DOUBLE_TYPE 
    && (first->d < second->d)) || (first->type == INT_TYPE && second->type == INT_TYPE 
    && (first->i < second->i)) || (first->type == INT_TYPE && second->type == DOUBLE_TYPE 
    && (first->i < second->d)))
    {
        if (!strcmp(symbol, "<") || !strcmp(symbol, "<="))
        {
            result->s = "#t";
        }
        else
        {
            result->s = "#f";
        }
        
    }
    else if((first->type == DOUBLE_TYPE && second->type == INT_TYPE 
    && (first->d == second->i)) || (first->type == DOUBLE_TYPE && second->type == DOUBLE_TYPE 
    && (first->d == second->d)) || (first->type == INT_TYPE && second->type == INT_TYPE 
    && (first->i == second->i)) || (first->type == INT_TYPE && second->type == DOUBLE_TYPE 
    && (first->i == second->d)))
    {
        if (!strcmp(symbol, "=") || !strcmp(symbol, "<=") || !strcmp(symbol, ">="))
        {
            result->s = "#t";
        }
        else
        {
            result->s = "#f";
        }
        
    }
    else 
    {
        if (!strcmp(symbol, ">") || !strcmp(symbol, ">="))
        {
            result->s = "#t";
        }
        else
        {
            result->s = "#f";
        }
    }
    return result;
}

Value *primitiveSmaller(Value* args)
{
    return primitiveCompare(args, "<");
}

Value *primitiveLarger(Value* args)
{
    return primitiveCompare(args, ">");
}

Value *primitiveEqual(Value* args)
{
    return primitiveCompare(args, "=");
}

Value *primitiveLargerEqual(Value* args)
{
    return primitiveCompare(args, ">=");
}

Value *primitiveSmallerEqual(Value* args)
{
    return primitiveCompare(args, "<=");
}

Value *primitiveModulo(Value* args)
{
    if(length(args)!=2)
    {
        printf("bad syntax for <\n");
        texit(1);
    }
    Value* result = talloc(sizeof(Value));
    result->type = INT_TYPE;
    Value *first = car(args);
    Value *second = car(cdr(args));
    if(first->type != INT_TYPE || second->type != INT_TYPE)
    {
        printf("Take numeric arguments\n");
        texit(1);
    }
    if(second->i == 0)
    {
        printf("Modulo: undefined for 0\n");
        texit(1);
    }
    result->i = first->i % second->i;
    if(first->i * second->i < 0)
    {    
        result->i = -result->i;
    }
    return result;
}

Value *primitiveCar(Value *args)
{
    if (length(args) != 1)
    {
        printf("bad syntax for car: take only one parameter\n");
        texit(1);
    }
    if (car(args)->type != CONS_TYPE)
    {
        printf("bad syntax for car: the parameter should be a list\n");
        texit(1);
    }
    return car(car(args));
}

Value *primitiveCdr(Value *args)
{
    if (length(args) != 1)
    {
        printf("bad syntax for cdr: take only one parameter\n");
        texit(1);
    }
    if (car(args)->type != CONS_TYPE)
    {
        printf("bad syntax for cdr: the parameter should be a list\n");
        texit(1);
    }
    //(cdr (quote (1 . 3)))
    if(cdr(car(args))->type == CONS_TYPE && car(cdr(car(args)))->type == DOT_TYPE)
    {
        return car(cdr(cdr(car(args))));
    }
    return cdr(car(args));
}

Value *primitiveNull(Value *args)
{
    if (length(args) != 1)
    {
        printf("bad syntax for null?: take only one parameter\n");
        texit(1);
    }
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    if((car(args)->type == CONS_TYPE && car(car(args))->type == NULL_TYPE) || car(args)->type == NULL_TYPE)
    {
        result->s = "#t";
    }
    else
    {
        result->s = "#f";
    }
    return result;
}

Value *primitiveCons(Value *args)
{
    if (length(args) != 2)
    {
        printf("bad syntax for cons: take only two parameter\n");
        texit(1);
    }
    Value *result = cons(car(args),car(cdr(args)));
    return result;
}

Value *primitiveEqualquestion(Value *args)
{
    if (length(args) != 2)
    {
        printf("bad syntax for equal?: take only two parameter\n");
        texit(1);
    }
    Value *first = car(args);
    Value *second = car(cdr(args));
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    if((first->type != INT_TYPE && first->type != DOUBLE_TYPE && 
    first->type != STR_TYPE) || 
    (second->type != INT_TYPE && second->type != DOUBLE_TYPE && 
    second->type != STR_TYPE))
    {
        printf("bad syntax for equal?: should be numbers, strings, or symbols\n");
        texit(1);
    }
    if(first->type == INT_TYPE && second->type == INT_TYPE && first->i == second->i)
    {
        result->s = "#t";
    }
    else if(first->type == DOUBLE_TYPE && second->type == DOUBLE_TYPE && first->d == second->d)
    {
        result->s = "#t";
    }
    else if(first->type == STR_TYPE && second->type == STR_TYPE && !strcmp(first->s,second->s))
    {
        result->s = "#t";
    }
    else
    {
        result->s = "#f";
    }
    return result;
}

Value* primitiveEq(Value* args)
{
    if (length(args) != 2)
    {
        printf("bad syntax for eq?: take only two parameters\n");
        texit(1);
    }
    Value *first = car(args);
    Value *second = car(cdr(args));
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    if (first==second)
    {
        result->s = "#t";
    }
    else
    {
        result->s = "#f";
    }
    return result;
}

Value* primitiveList (Value* args) 
{
    return args;
}

Value *replaceLast(Value *list, Value *last)
{
    while(list->type != NULL_TYPE)
    {
        return (cons(car(list), replaceLast(list->c.cdr, last)));
    }
    return last;
}

Value* primitiveAppend (Value* args) 
{
    if(args->type != CONS_TYPE && args->type != NULL_TYPE)
    {
        printf("append: contract violation\n");
        texit(1);
    }
    Value *answer = makeNull();
    //go throught all the parameter except the last one 
    while (args->type == CONS_TYPE && cdr(args)->type != NULL_TYPE)
    {
        Value *val = car(args);
        //error checking: the parameter can only be a list, whether empty or list
        if(val->type != CONS_TYPE  && val->type != NULL_TYPE)
        {
            printf("append: contract violation\n");
            texit(1);
        }
        //if the list is not empty, add the item in the list to answer
        if(val->type == CONS_TYPE)
        {
            while(val->type != NULL_TYPE)
            {
                //error checking for improper list
                if(val->type != CONS_TYPE)
                {
                    printf("append: contract violation\n");
                    texit(1);
                }
                answer = cons(car(val),answer);
                val = cdr(val);
            }
        }
        args = cdr(args);
    }
    //check for the last parameter
    if(args->type == CONS_TYPE)
    {
        Value *val = car(args);
        while(val->type != NULL_TYPE)
        {
            //if it is improper list
            if(val->type != CONS_TYPE)
            {
                answer = reverse(answer);
                answer = replaceLast(answer, val);
                return answer;
            }
            answer = cons(car(val),answer);
            val = cdr(val);
        }
        return reverse(answer);
    }
    else if(args->type == NULL_TYPE)
    {
        return reverse(answer);
    }
    else
    {
        answer = reverse(answer);
        answer = replaceLast(answer, args);
        return answer;
    }
}

void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    // Add primitive functions to top-level bindings list
    Value *value = talloc(sizeof(Value));
    value->type = PRIMITIVE_TYPE;
    value->pf = function;
    Value *primitive_name = talloc(sizeof(Value));
    primitive_name->type = SYMBOL_TYPE;
    primitive_name->s = name;
    Value* binding = makeNull();
    binding = cons(value, binding);
    binding = cons(primitive_name, binding);
    frame->bindings = cons(binding, frame->bindings);
}

//look up primitive type value in the bindings of top frame
Value* lookUpPrimitive(char *name)
{
    Value* bindings = top_frame->bindings;
    Value* binding_pair;
    while(bindings->type != NULL_TYPE)
    {
        binding_pair = car(bindings);
        if (!strcmp(car(binding_pair)->s, name))
        {
            return (car(cdr(binding_pair)));
        }
        bindings = cdr(bindings);
    }
    printf("primitive type does not exist");
    texit(1);
    return bindings;
}


void interpret(Value *tree)
{
    //create a global frame
    top_frame = talloc(sizeof(Frame));
    top_frame->parent = NULL;
    top_frame->bindings = makeNull();
    // Make top-level bindings list
    bind("+",primitiveAdd, top_frame);
    bind("car",primitiveCar, top_frame);
    bind("cdr",primitiveCdr, top_frame);
    bind("null?",primitiveNull, top_frame);
    bind("cons",primitiveCons, top_frame);
    bind("equal?",primitiveEqualquestion, top_frame);
    bind("eq?",primitiveEq, top_frame);
    bind("list",primitiveList, top_frame);
    bind("append",primitiveAppend, top_frame);
    bind("*",primitiveTimes, top_frame);
    bind("-",primitiveMinus, top_frame);
    bind("/",primitiveDivide, top_frame);
    bind("<",primitiveSmaller, top_frame);
    bind(">",primitiveLarger, top_frame);
    bind("=",primitiveEqual, top_frame);
    bind(">=",primitiveLargerEqual, top_frame);
    bind("<=",primitiveSmallerEqual, top_frame);
    bind("modulo",primitiveModulo, top_frame);
    //evaluate every S expresion in the list
    while (tree->type != NULL_TYPE)
    {
        print(eval(tree->c.car, top_frame));
        tree = tree->c.cdr;
    }
}


Value* evalWhenAndUnless(Value* args, Frame* frame, bool when)
{
    if (length(args)<2)
    {
        printf("When an when or unless expression has fewer or more than 2 arguments\n");
        texit(1);
    }
    Value *first = car(args);
    first = eval(first, frame);
    if (first->type != BOOL_TYPE)
    {
        printf("an when or unless expression evaluates to something other than a boolean\n");
        texit(1);
    }
    //if this is "when #t <body>" or "unless #f <body>"
    Value* list = makeNull();
    if ((when && !strcmp(first->s,"#t")) || (!when && !strcmp(first->s,"#f")))
    {
        //evaluate everything in the body
        args = cdr(args);
        while (args->type != NULL_TYPE)
        {
            list = cons(eval(car(args), frame), list);
            args = cdr(args);
        }
    }
    return reverse(list);
}

Value* evalIf(Value* args, Frame* frame)
{
    if (length(args)!=3)
    {
        printf("an if expression does not have 3 arguments\n");
        texit(1);
    }
    Value *first = car(args);
    first = eval(first, frame);
    if (first->type != BOOL_TYPE)
    {
        printf("an if expression evaluates to something other than a boolean\n");
        texit(1);
    }
    //for (if #t a b), evaluate a
    if (!strcmp(first->s,"#t"))
    {
        return eval(car(cdr(args)), frame);
    }
    //for (if #t a b), evaluate b
    else
    {
        return eval(car(cdr(cdr(args))),frame);
    }
}

//check if a given symbol is in the list
bool isInList(Value* list, Value* symbol)
{
    Value *temp = symbol;
    // printf("$ ");
    // printTree(list);
    // printTree(symbol);
    // printf("$ ");
    while (list->type != NULL_TYPE)
    {
        if (!strcmp(car(list)->s, temp->s))
        {
            return true;
        }
        list = cdr(list);
    }
    return false;
}

//This function evaluate let by add all the binding to the new frame 
//and evaluate the last item in the new frame
Value* evalLet(Value* args, Frame* frame)
{
    if(length(args)<2)
    {
        printf("let: bad syntax (missing binding pairs or body)\n");
        texit(1);
    }
    Value *binding_list = car(args);
    //check if <binding_list> is nested list ((x 2)(y 3))
    if(binding_list->type != CONS_TYPE)
    {
        printf("the list-of-bindings for let does not contain a nested list\n");
        texit(1);
    }
    //iterate through the binding list to check if there is syntax error
    Value *symbol_list = makeNull();
    Value *binding_pairs;
    Value *new_binding_list = makeNull();
    Value *new_binding_pair = makeNull();
    while(binding_list->type != NULL_TYPE)
    {
        binding_pairs = car(binding_list);
        //check if <binding_list> is nested list ((x 2)(y 3))
        if(binding_pairs->type != CONS_TYPE)
        {
            printf("the list-of-bindings for let does not contain a nested list\n");
            texit(1);
        }
        Value* symbol = car(binding_pairs);
        //error checking for ((x 1 2)), ((1 2)), ((x 2) (x 3))
        if (length(binding_pairs) != 2 || symbol->type != SYMBOL_TYPE
        || isInList(symbol_list, symbol))
        {
            printf("let: bad syntax (not an identifier and expression for a binding)\n");
            texit(1);
        }
        symbol_list = cons(symbol, symbol_list);
        //evaluate the expression in <val expr>, such as ((x y))
        Value* val = eval(car(cdr(binding_pairs)), frame);
        new_binding_pair = cons(val, new_binding_pair);
        new_binding_pair = cons(symbol, new_binding_pair);
        new_binding_list = cons(new_binding_pair, new_binding_list);
        // *car(cdr(binding_pairs)) = *eval(car(cdr(binding_pairs)), frame);
        // printf("%i", eval(car(cdr(binding_pairs)), frame)->i);
        binding_list = cdr(binding_list);
    }
    //create new frame
    Frame *new_frame = talloc(sizeof(Frame));
    new_frame->parent = frame;
    new_frame->bindings = new_binding_list;
    args = cdr(args);
    //for (let <list_of_binding> <body>), evaluate each value
    //return the last value in the body
    while(cdr(args)->type != NULL_TYPE)
    {
        eval(car(args), new_frame);
        args = cdr(args);
    }
    return eval(car(args), new_frame);
}

Value* evalLetStar(Value* args, Frame* frame)
{
    if(length(args)<2)
    {
        printf("let*: bad syntax (missing binding pairs or body)\n");
        texit(1);
    }
    Value *binding_list = car(args);
    //check if <binding_list> is nested list ((x 2)(y 3))
    if(binding_list->type != CONS_TYPE)
    {
        printf("the list-of-bindings for let* does not contain a nested list\n");
        texit(1);
    }
    //iterate through the binding list to check if there is syntax error
    Value *symbol_list = makeNull();
    Value *binding_pairs;
    while(binding_list->type != NULL_TYPE)
    {
        binding_pairs = car(binding_list);
        //check if <binding_list> is nested list ((x 2)(y 3))
        if(binding_pairs->type != CONS_TYPE)
        {
            printf("the list-of-bindings for let* does not contain a nested list\n");
            texit(1);
        }
        Value* symbol = car(binding_pairs);
        //error checking for ((x 1 2)), ((1 2))
        if (length(binding_pairs) != 2 || symbol->type != SYMBOL_TYPE)
        {
            printf("let*: bad syntax (not an identifier and expression for a binding)\n");
            texit(1);
        }
        //evaluate the expression in <val expr>, such as ((x y))
        *car(cdr(binding_pairs)) = *eval(car(cdr(binding_pairs)), frame);
        Frame *new_frame = talloc(sizeof(Frame));
        new_frame->parent = frame;
        Value *pairs = cons(binding_pairs,makeNull());
        new_frame->bindings = pairs;
        frame = new_frame;
        binding_list = cdr(binding_list);
    }
    args = cdr(args);
    //for (let <list_of_binding> <body>), evaluate each value
    //return the last value in the body
    while(cdr(args)->type != NULL_TYPE)
    {
        eval(car(args), frame);
        args = cdr(args);
    }
    return eval(car(args), frame);
}

Value* evalLetrec(Value* args, Frame* frame)
{
    if(length(args)<2)
    {
        printf("letrec: bad syntax (missing binding pairs or body)\n");
        texit(1);
    }
    Value *binding_list = car(args);
    //check if <binding_list> is nested list ((x 2)(y 3))
    if(binding_list->type != CONS_TYPE)
    {
        printf("the list-of-bindings for letrec does not contain a nested list\n");
        texit(1);
    }
    //iterate through the binding list to check if there is syntax error
    Value *symbol_list = makeNull();
    Value *expr_list = makeNull();
    Value *binding_pairs;
    Frame *new_frame = talloc(sizeof(Frame));
    new_frame->parent = frame;
    new_frame->bindings = makeNull();
    while(binding_list->type != NULL_TYPE)
    {
        binding_pairs = car(binding_list);
        //check if <binding_list> is nested list ((x 2)(y 3))
        if(binding_pairs->type != CONS_TYPE)
        {
            printf("the list-of-bindings for letrec does not contain a nested list\n");
            texit(1);
        }
        Value* symbol = car(binding_pairs);
        Value* expr = car(cdr(binding_pairs));
        //error checking for ((x 1 2)), ((1 2)), ((x 2) (x 3))
        if (length(binding_pairs) != 2 || symbol->type != SYMBOL_TYPE || isInList(symbol_list, symbol))
        {
            printf("letrec: bad syntax (not an identifier and expression for a binding)\n");
            texit(1);
        }
        //(define x 2)
        //(letrec ((y 2) (x x)) y)
        if (expr->type == SYMBOL_TYPE)
        {
            Value *expr1 = talloc(sizeof(Value));
            expr1->type = SYMBOL_TYPE;
            expr1->s = expr->s;
            expr_list = cons(expr1, expr_list);
        }
        if(isInList(expr_list, symbol))
        {
            printf("letrec: bad syntax (not an identifier and expression for a binding)\n");
            texit(1);
        }
        symbol_list = cons(symbol, symbol_list);
        //evaluate the expression in <val expr>, such as ((x y))
        *car(cdr(binding_pairs)) = *eval(car(cdr(binding_pairs)), new_frame);
        new_frame->bindings = cons(binding_pairs,new_frame->bindings);
        binding_list = cdr(binding_list);
    }
    args = cdr(args);
    //for (let <list_of_binding> <body>), evaluate each value
    //return the last value in the body
    while(cdr(args)->type != NULL_TYPE)
    {
        eval(car(args), new_frame);
        args = cdr(args);
    }
    return eval(car(args), new_frame);
}

//This function evaluate the lambda expression
//The lambda expression could be lambda(), lambda(x) or lambda x
//Return a closure which has paramNames, functionCode and frame
Value* evalLambda(Value* args, Frame* frame)
{
    Value* closure = talloc(sizeof(Value));
    closure->type = CLOSURE_TYPE;
    if(length(args)<2)
    {
        printf("lambda: wrong syntax\n");
        texit(1);
    }
    if(car(args)->type != SYMBOL_TYPE && car(args)->type != CONS_TYPE
      && car(args)->type != NULL_TYPE)
    {
        printf("lambda: not an identifier\n");
        texit(1);
    }
    //error checking for cons_type
    if (car(args)->type == CONS_TYPE)
    {
        Value *symbol_list = makeNull();
        Value *paramNamesList = car(args);
        Value *param;
        while(paramNamesList->type != NULL_TYPE)
        {
            //report error for lambda(1)
            param = car(paramNamesList);
            if(param->type != SYMBOL_TYPE)
            {
                printf("lambda: not an identifier\n");
                texit(1);
            }
            //report error for lambda(x x)
            if (isInList(symbol_list, param))
            {
                printf("lambda: duplicate argument name\n");
                texit(1);
            }
            symbol_list = cons(param, symbol_list);
            paramNamesList = cdr(paramNamesList);
        }
    }
    closure->cl.paramNames = car(args);
    //only consider the last item in args as functionCode
    // while(cdr(args)->type != NULL_TYPE)
    // {
    //     args = cdr(args);
    // }
    closure->cl.functionCode = cdr(args);
    closure->cl.frame = frame;
    return closure;
}

//Define function evaluate the <expr> in (Define <val> <expr>) 
//add binding to the top frame 
//The define expression can be (define a <expr>) or (define (hi z) <expr>)
//Return a void type Value
Value* evalDefine(Value* args, Frame* frame)
{
    if(length(args) < 2)
    {
        printf("Define: bad syntax\n");
        texit(1);
    }
    if (car(args)->type != SYMBOL_TYPE && car(args)->type != CONS_TYPE)
    {
        printf("Define: bad syntax\n");
        texit(1);
    }
    Value* binding = makeNull();
    Value* val;
    Value* expr;
    //case for: (define (try1) 1)
    //          (define (try1 x) 1 x)
    // lambda(x) (1 x)
    if(car(args)->type == CONS_TYPE)
    {
        //(define (try1) 1)
        //lambda should be () 1
        Value *lambda = cons(cdr(car(args)), cdr(args));
        Value* expr = cons(evalLambda(lambda,frame), makeNull());
        binding = cons(car(car(args)),expr);
    }
    //case for: (define a 2)
    //          (define this (lambda (x) x))
    else
    {  
        if(length(args) != 2)
        {
            printf("define: bad syntax\n");
            texit(1);
        }
        val = car(args);
        expr = eval(car(cdr(args)),frame);
        binding = cons(expr, binding);
        binding = cons(val,binding);
    }
    top_frame->bindings = cons(binding, top_frame->bindings);
    Value* empty = talloc(sizeof(Value));
    empty->type = VOID_TYPE;
    return empty;
}

//evaluate each item in the argument, return a linked list
Value* evalEach(Value* args, Frame* frame)
{
    Value* list = makeNull();
    Value* result;
    while (args->type != NULL_TYPE)
    {
        result = eval(car(args), frame);
        list = cons(result,list);
        args = cdr(args);
    }
    return reverse(list);
}

//Apply function take in a closure and a argument
//Construct a new frame whose parent frame is the environment stored in the closure.
//Add bindings to the new frame mapping each formal parameter to the corresponding actual parameter.
//Evaluate the function body with the new frame as its environment, 
//and return the result of the call to eval. 
Value* apply(Value* evaledOperator, Value* evaledArgs)
{
    Frame* new_frame = talloc(sizeof(Frame));
    new_frame->parent = evaledOperator->cl.frame;
    Value* params = evaledOperator->cl.paramNames;
    Value* binding_list = makeNull();
    Value* binding = makeNull();
    //params could be null, a single parameter or a list of parameter
    //if lambda that takes a single parameter, not in parentheses, 
    //that gets bound to a list of parameters.
    if(params->type == SYMBOL_TYPE)
    {
        binding = cons(evaledArgs, binding);
        binding = cons(params, binding);
        binding_list = cons(binding, binding_list);
    }
    //if params is a list of parameter
    else if (params->type == CONS_TYPE)
    {
        if (length(params) != length(evaledArgs))
        {
            printf("too many or too few parameters");
            texit(1);
        }
        //error checking for (lambda () 4 4))
        while (params->type != NULL_TYPE)
        {
            binding = cons(car(evaledArgs),binding);
            binding = cons(car(params),binding);
            binding_list = cons(binding, binding_list);
            params = cdr(params);
            evaledArgs = cdr(evaledArgs);
        }
    }
    else if (params->type == NULL_TYPE)
    {
        if (length(params) != length(evaledArgs))
        {
            printf("too many or too few parameters");
            texit(1);
        }
    }
    else
    {
        printf("bad syntax for define");
        texit(1);
    }
    new_frame->bindings = binding_list;
    Value* function_code = evaledOperator->cl.functionCode;
    while (cdr(function_code)->type != NULL_TYPE)
    {
        eval(car(function_code), new_frame);
        function_code = cdr(function_code);
    }
    return eval(car(function_code), new_frame);
}

Value* evalPrimitive(char* name, Value* args, Frame* frame)
{
    Value *(*function)(struct Value *) =  lookUpPrimitive(name)->pf;
    Value *evaledArgs = evalEach(args, frame);
    return (*function)(evaledArgs);
}

Value* evalSet(Value* args, Frame* frame)
{
    if(length(args)!=2)
    {
        printf("set!: wrong syntax\n");
        texit(1);
    }
    Value *first = car(args);
    if(first->type != SYMBOL_TYPE)
    {
        printf("set!: not an identifier\n");
        texit(1);
    }
    Value *second = eval(car(cdr(args)), frame);
    *lookUpSymbol(first, frame) = *second;
    Value* empty = talloc(sizeof(Value));
    empty->type = VOID_TYPE;
    return empty;
}

Value* evalBegin(Value* args, Frame* frame)
{
    Value* result = talloc(sizeof(Value));
    if(length(args) == 0) 
    {
        result->type = VOID_TYPE;
        return result;
    }
    
    while (cdr(args)->type != NULL_TYPE)
    {
        eval(car(args), frame);
        args = cdr(args);
    }
    return eval(car(args), frame);
}

Value* evalCond(Value* args, Frame* frame) 
{
    Value* result = talloc(sizeof(Value));
    result->type = VOID_TYPE;
    if(length(args) == 0)
    {
        return result;
    }
    Value* statement; 
    Value* condition;
    Value* body;
    while(args->type != NULL_TYPE) 
    {
        statement = car(args);
        if(length(statement)!= 2) 
        {
            printf("too many or too few arguments");
            texit(1);
        }
        condition = car(statement);
        body = car(cdr(statement));
        if(!strcmp(condition->s,"else"))
        {
            return eval(body, frame);
        }
        if(eval(condition, frame)->type != BOOL_TYPE)
        {
            printf("condition is not true or false");
            texit(1);
        }
        if(!strcmp(eval(condition, frame)->s,"#t")) 
        {
            return eval(body, frame);
        }
        args = cdr(args);
    }
    return result;
}

Value* evalAnd(Value* args, Frame* frame) 
{
    Value* result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    if(length(args) == 0)
    {
        printf("too few arguments \n");
        texit(1);
    }
    Value* current; 
    while(args->type != NULL_TYPE) 
    {
        current = eval(car(args), frame);
        if(current->type != BOOL_TYPE) 
        {
            printf("argument must be type boolean\n");
            texit(1);
        }
        if(!strcmp(current->s,"#f"))
        {
            result->s = "#f";
            return result; 
        }
        args = cdr(args);
    }
    result->s = "#t";
    return result;
} 
Value* evalOr(Value* args, Frame* frame) 
{
    Value* result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    if(length(args) == 0)
    {
        printf("too few arguments \n");
        texit(1);
    }
    Value* current; 
    while(args->type != NULL_TYPE) 
    {
        current = eval(car(args), frame);
        if(current->type != BOOL_TYPE) 
        {
            printf("argument must be type boolean\n");
            texit(1);
        }
        if(!strcmp(current->s,"#t"))
        {
            result->s = "#t";
            return result; 
        }
        args = cdr(args);
    }
    result->s = "#f";
    return result;
}
//A function to evaluate the parse tree
//Tree: a parse tree
//Frame: a frame 
Value *eval(Value *tree, Frame *frame) {
    char* s;
    switch (tree->type)  
    {
        case SYMBOL_TYPE:
        {
            return value_copy(lookUpSymbol(tree, frame));
            break;
        }
        case CONS_TYPE:
        {
            Value *first = car(tree);
            Value *args = cdr(tree);
            if(first->type != SYMBOL_TYPE && first->type != CONS_TYPE
            && first->type != PRIMITIVE_TYPE) 
            {
                printf("error: not a procedure\n");
                texit(1);
            }
            if (!strcmp(first->s,"if")) 
            {
                return evalIf(args, frame);
            }
            else if (!strcmp(first->s,"when")) 
            {
                return evalWhenAndUnless(args, frame, true);
            }
            else if (!strcmp(first->s,"unless")) 
            {
                return evalWhenAndUnless(args, frame, false);
            }
            else if (!strcmp(first->s,"let"))
            {
                return evalLet(args, frame);
            }
            else if (!strcmp(first->s,"quote"))
            {
                if (length(args) != 1)
                {
                    printf("bad syntax for quote\n");
                    texit(1);
                }
                return car(args);
            }
            else if (!strcmp(first->s,"define"))
            {
                return evalDefine(args, frame);
            }
            else if (!strcmp(first->s,"lambda"))
            {
                return evalLambda(args, frame);
            }
            else if (!strcmp(first->s,"let*"))
            {
                return evalLetStar(args, frame);
            }
            else if (!strcmp(first->s,"letrec"))
            {
                return evalLetrec(args, frame);
            }
            else if (!strcmp(first->s,"set!"))
            {
                return evalSet(args, frame);
            }
            else if (!strcmp(first->s,"begin"))
            {
                return evalBegin(args, frame);
            }
            else if (!strcmp(first->s,"cond"))
            {
                return evalCond(args, frame);
            }
            else if (!strcmp(first->s,"and"))
            {
                return evalAnd(args, frame);
            }
            else if (!strcmp(first->s,"or"))
            {
                return evalOr(args, frame);
            }
            else if (!strcmp(first->s,"+") || !strcmp(first->s,"car") || !strcmp(first->s,"cdr")
            || !strcmp(first->s,"null?") || !strcmp(first->s,"cons") || !strcmp(first->s,"equal?")
            || !strcmp(first->s,"eq?") || !strcmp(first->s,"list") || !strcmp(first->s,"append")
            || !strcmp(first->s,"*") || !strcmp(first->s,"-") || !strcmp(first->s,"/")
            || !strcmp(first->s,"<") || !strcmp(first->s,">") || !strcmp(first->s,"=")
            || !strcmp(first->s,"modulo") || !strcmp(first->s,"<=") || !strcmp(first->s,">="))
            {
                return evalPrimitive(first->s, args, frame);
            }
            else
            {
                // If not a special form, evaluate the first, evaluate the args, then
                // apply the first to the args.
                Value *evaledOperator = eval(first, frame);
                Value *evaledArgs = evalEach(args, frame);
                return apply(evaledOperator,evaledArgs);
            }
            break;
        }
        case INT_TYPE:
            break;
        case DOUBLE_TYPE:
            break;
        case STR_TYPE:
            break;
        case BOOL_TYPE:
            break;
        case NULL_TYPE:
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            break;
        case CLOSE_TYPE:
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
    return value_copy(tree);
}
