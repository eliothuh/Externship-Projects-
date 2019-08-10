#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"

//Global variable for current token and index 
char *curtoken;
int index_num;
//global variable to keep track of space
int space;

// Displays the contents of the linked list as tokens, with type information
// list: the linked list of tokens 
void displayTokens(Value *list)
{
    switch(list->type)
    {
        case INT_TYPE:
            printf("%i:integer\n", list->i);
            break;
        case DOUBLE_TYPE:
            printf("%f:float\n", list->d);
            break;
        case STR_TYPE:
            printf("%s:string\n", list->s);
            break;
        case NULL_TYPE:
            break;
        case CONS_TYPE:
            displayTokens(list->c.car);
            displayTokens(list->c.cdr);
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            printf("(:open\n");
            break;
        case CLOSE_TYPE:
            printf("):close\n");
            break;
        case BOOL_TYPE:
            printf("%s:boolean\n", list->s);
            break;
        case SYMBOL_TYPE:
            printf("%s:symbol\n", list->s);
            break;
        case SINGLEQUOTE_TYPE:
            printf("':singlequote\n");
            break;
        case OPENBRACKET_TYPE:
            printf("[:openbracket\n");
            break;
        case CLOSEBRACKET_TYPE:
            printf("]:closebracket\n");
            break;
        case DOT_TYPE:
            printf(".:dot\n");
            break;
        case VOID_TYPE:
            break;
        case CLOSURE_TYPE:
            break;
        case PRIMITIVE_TYPE:
            break;
    }
}


//the below three functions check if the given token is a symbol type
bool check_initial(char c)
{
    char initial[14]={'!','$','%','&','*','/',':','=','<','>','?','~','_','^'};
    for (int i=0; i < 14; i++) {
        if (c == initial[i])
        {
            return true;
        }
    }
    if(isalpha(c))
    {
        return true;
    }
    return false;
}

bool check_subsequent(char c)
{
    return (check_initial(c) || isdigit(c) || c == '.' || c == '+' || c == '-');
}

bool check_symbol_type()
{
    if((curtoken[0]=='+' || curtoken[0]=='-') && strlen(curtoken) == 1)
    {
        return true;
    }
    if (check_initial(curtoken[0])) 
    {
        if(strlen(curtoken) == 1)
        {
            return true;
        }
        else 
        {
            for(int i = 1; i < strlen(curtoken); i++)
            {
                if(!check_subsequent(curtoken[i]))
                {
                    printf("invalid symbol token\n");
                    texit(1);
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

//the below three functions check if a given token is an integer or float type
bool check_decimal(int start)
{
    int dot_count = 0;
    int digit_count = 0;
    for (int i = start; i < strlen(curtoken); i++)
    {
        char cur = curtoken[i];
        if (cur == '.')
        {
            dot_count++;
        }
        else if (isdigit(cur))
        {
            digit_count++;
        }
        else
        {
            return false;
        }
    }
    return (dot_count==1 && digit_count>0);
}

bool check_integer(int start)
{
    int dot_count = 0;
    int digit_count = 0;
    for (int i = start; i < strlen(curtoken); i++)
    {
        char cur = curtoken[i];
        if (cur == '.')
        {
            dot_count++;
        }
        else if (isdigit(cur))
        {
            digit_count++;
        }
        else
        {
            return false;
        }
    }
    return (dot_count==0 && digit_count>0);
}

//check whether the token is an integer or a double type
//output: i integer
//        d double
//        f not a number type   
char check_number_type()
{
    if (check_integer(0)|| 
        ((curtoken[0]=='+' || curtoken[0]=='-') && check_integer(1)))
    {
        return 'i';
    }
    else if (check_decimal(0)|| 
        ((curtoken[0]=='+' || curtoken[0]=='-') && check_decimal(1)))
    {
        return 'd';
    }
    else
    {
        return 'f';
    }
}

//create different types of token value based on current token
Value *create_token()
{
    Value *token = talloc(sizeof(Value));
    //boolean type
    if(curtoken[0]=='#' && strlen(curtoken)==2)
    {
        token->type = BOOL_TYPE;
        char *s = talloc(sizeof(char)*3);
        s[0]='#';
        if (curtoken[1]=='t')
        {
            s[1]='t';
        }
        else if (curtoken[1]=='f')
        {
            s[1]='f';
        }
        else
        {
            printf("invalid boolean type\n");
            texit(1);
        }
        s[2]='\0';
        token->s = s;
    }
    //string type
    else if (curtoken[0] == '"')
    {
        token->type = STR_TYPE;
        char *s = talloc(sizeof(char)*(strlen(curtoken)+1));
        for (int i = 0; i < strlen(curtoken); i++)
        {
            s[i] = curtoken [i];
        }
        s[strlen(curtoken)]='\0';
        token->s = s;
    }
    //int type
    else if(check_number_type() == 'i')
    {
        token->type = INT_TYPE;
        token->i = atol(curtoken);
    }
    //double type
    else if (check_number_type() == 'd')
    {
        token->type = DOUBLE_TYPE;
        token->d = atof(curtoken);
    }
    //symbol type
    else if(check_symbol_type())
    {
        token->type = SYMBOL_TYPE;
        char *s = talloc(sizeof(char)*(strlen(curtoken)+1));
        for (int i = 0; i < strlen(curtoken); i++)
        {
            s[i] = curtoken [i];
        }
        s[strlen(curtoken)]='\0';
        token->s = s;
    }
    //the following check all the other types
    else if (curtoken[0] == '(')
    {
        token->type = OPEN_TYPE;
    }
    else if (curtoken[0] == ')')
    {
        token->type = CLOSE_TYPE;
    }
    else if (curtoken[0] == '\'')
    {
        token->type = SINGLEQUOTE_TYPE;
    }
    else if (curtoken[0] == '[')
    {
        token->type = OPENBRACKET_TYPE;
    }
    else if (curtoken[0] == ']')
    {
        token->type = CLOSEBRACKET_TYPE;
    }
    //check the length and content of curtoken and whether there is a space before curtoken
    else if (curtoken[0] == '.' && strlen(curtoken) == 1 && space > 0)
    {
        token->type = DOT_TYPE;
    }
    else
    {
        printf("invalid token\n");
        texit(1);
    }
    return token;
}


//a helper function that add value to the list
Value *add_to_list(Value *list){
    //if the current token is not empty 
    if(index_num != 0)
    {
        //create a token value and add to list
        curtoken[index_num] = '\0';
        Value *token = create_token();
        list = cons(token, list);

        //reset index and current token
        index_num=0;
        curtoken = talloc(sizeof(char)*1000);
    }
    return list;
}

// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize() {
    Value* list = makeNull();
    index_num = 0;
    space = 0;
    curtoken = talloc(sizeof(char)*1000);
    char charRead = fgetc(stdin);
    while (charRead != EOF) {
        if (charRead == '(' ||charRead == ')' || charRead == '\'' 
        || charRead == '[' ||charRead == ']') 
        {
            space = 0;
            list = add_to_list(list);
            curtoken[index_num++] = charRead;
            list = add_to_list(list);   
        }
        else if(charRead == ' ') 
        {
            list = add_to_list(list);
            space++;
        }
        else if (charRead == '\n')
        {   
            space = 0;
            if (index_num == 1 && curtoken[0] == '.')
            {
                printf("invalid dot token\n");
                texit(1);
            }
            list = add_to_list(list);
        }
        else if(charRead == '"') 
        {
            space = 0;
            list = add_to_list(list);
            curtoken[index_num++] = charRead;
            charRead = (char)fgetc(stdin);
            while(charRead != '"') 
            {
                curtoken[index_num++] = charRead;
                if(charRead == EOF)
                {
                    printf("invalid string token\n");
                    texit(1);
                }
                charRead = (char)fgetc(stdin);
            }
            curtoken[index_num++] = charRead;
            list = add_to_list(list);
        } 
        else if (charRead == ';') 
        {
            space = 0;
            while (charRead != '\n')
            {
                if(charRead == EOF) 
                {
                    break;
                }
                charRead = (char)fgetc(stdin);
            }
        } 

        else 
        {
            curtoken[index_num++] = charRead;
        }
        charRead = (char)fgetc(stdin);
    }
    if ((index_num == 1 && curtoken[0] == '.'))
    {
        printf("invalid dot token\n");
        texit(1);
    }
    list = add_to_list(list);
    Value *revList = reverse(list);
    return revList;
}

