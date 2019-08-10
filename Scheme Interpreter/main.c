#include <stdio.h>
#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"
#include "interpreter.h"

int main() {
    Value *list = tokenize(stdin);
    Value *tree = parse(list);
    //illustrate that our mark and sweep works
    mark(list);
    interpret(tree);
    tfree();
    tfree();
    //now everything is freed
    //Demo of mark and sweep
    //make mutiple values 
    printf("Demo of our mark and sweep:\n");
    Value *val1 = talloc(sizeof(Value));
    val1->type = INT_TYPE;
    val1->i = 12;

    Value *val2 = talloc(sizeof(Value));
    val2->type = DOUBLE_TYPE;
    val2->d = 4.3;

    Value *val3 = talloc(sizeof(Value));
    val3->type = STR_TYPE;
    val3->s = "apple";

    Value *val4 = talloc(sizeof(Value));
    val4->type = DOUBLE_TYPE;
    val4->d = 2.7;

    Value *head = makeNull();

    head = cons(val1,head);
    head = cons(val2,head);
    head = cons(val3,head);
    head = cons(val4,head);
    display(head);
    printf("\n");
    //test cases for mark
    //mark the third element of the linked list
    Value *mark_node = head->c.cdr->c.cdr;
    mark(val1);
    mark(val4);
    tfree();
    printf("Mark val1 and val4. We still have access to val1 and val4 after freeing the rest:\n");
    printf("val1: ");
    display(val1);
    printf("\nval4: ");
    display(val4);
    printf("\n");
    mark(val1);
    tfree();
    printf("Mark val1. We still have access to val1 after freeing the rest:\n");
    printf("val1: ");
    display(val1);
    printf("\n");
    tfree();
    return 0;
}


