#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define STACK_SIZE 100

int numberStack[100];
int topnum = -1;

char operatorStack[100];
int topop = -1;

void pushNumber(int n){
    if(topnum >= (STACK_SIZE - 1) ){
        printf("Number Stack Overflow\n");
    }
    else{
        numberStack[++topnum] = n;
    }
}
int popNumber(int *error){
    int result = 0;
    if(topnum < 0){
        printf("Stack Underflow");
        *error = 1;
    }else{
        result = numberStack[topnum--];
    }
    return result;
}
void pushOperator(char c){
    if(topop >= STACK_SIZE - 1){
        printf("Operator stack overflow");
    }else{
        operatorStack[++topop] = c;
    }
}
char popOperator(int *error){
    int result = 0;
    if(topop < 0){
        printf("Stack underflow");
        *error = 1;
    }else{
        result =  operatorStack[topop--];
    }
    return result;
}
int priority(char operator){
    int prvalue = 0;
    switch(operator){
        case '+':
            prvalue = 1;
            break;
        case '-':
            prvalue = 1;
            break;
        case '*':
            prvalue = 2;
            break;
        case '/':
            prvalue = 2;
            break;
        default:
            prvalue = 0;
            break;
    }
    return prvalue;
}
int calculate(int first, int second, char operator, int *error){
    int result = 0;
    switch(operator){
        case '+':
            result = first + second;
            break;
        case '-':
            result = first - second;
            break;
        case '*':
            result = first * second;
            break;
        case '/':
            if(second == 0){
                printf("Division by zero not allowed\n");
                *error = 1;
            }else{
                result = first / second;
                break;
            }
            break;
        default:
            printf("Invalid Operator\n");
            *error = 1;
            break;
    }
    return result;
}
int evaluateExpression(char *expression){
    int error = 0;
    int idx = 0;
    int final = 0;

    while(expression[idx] && !error){
        if(isspace(expression[idx])){
            idx++;
            continue;
        }
        else if(isdigit(expression[idx])){
            int number = 0;
            while(isdigit(expression[idx]) || isspace(expression[idx])){
                if(isspace(expression[idx])){
                    idx++;
                    continue;
                }
                number = number * 10 + expression[idx] - '0';
                idx++;
            }
            pushNumber(number);
            continue;
        }
        else if(strchr("+-*/",expression[idx])){
            while(topop != -1 && priority(operatorStack[topop]) >= priority(expression[idx]) && error != 1){
                int secondValue = popNumber(&error);
                int firstValue = popNumber(&error);
                int operator = popOperator(&error);
                int tempResult = calculate(firstValue,secondValue,operator,&error);
                pushNumber(tempResult);
            }
            pushOperator(expression[idx++]);
            continue;
        }
        else{
            printf("Invalid Expression: %c\n",expression[idx]);
            error = 1;
        }
    }
    while(topop != -1 && !error){
        int secondValue = popNumber(&error);
        int firstValue = popNumber(&error);
        int operator = popOperator(&error);
        int tempResult = calculate(firstValue,secondValue,operator,&error);
        pushNumber(tempResult);
    }
    if(!error && topnum >= 0){
        final = numberStack[topnum];
    }
    return error ? 0 : final;
}

int main(){
    char expression[100];
    printf("Enter the expression\n");
    fgets(expression,sizeof(expression),stdin);

    int result = evaluateExpression(expression);
    printf("result = %d",result);
    return 0;
}
