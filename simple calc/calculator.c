#include <stdio.h>
#include <stdbool.h>

int main(){
    char op;
    int num1, num2;
    bool validInput;

    while(1)
    {
        printf("Enter your calculation: ");
        int result;
        validInput = true;

        if (scanf("%d %c %d", &num1, &op, &num2) != 3)
        {
            printf("Invalid calculation! \"%d %c %d\"\n", num1, op, num2);
            validInput = false;
            break;
        }
        else{

            switch (op)
            {
                case '+':
                    result = num1 + num2;
                    break;
                case '-':
                    result = num1 - num2;
                    break;
                case '*':
                    result = num1 * num2;
                    break;
                case '&':
                    result = num1 & num2;
                    break;
                case '/':
                    if (num2 == 0)
                    {
                        printf("Invalid calculation! \"%d %c %d\"\n", num1, op, num2);
                        validInput = false;
                        break;
                    }
                    result = num1 / num2;
                    break;
                case '%':
                    if (num2 == 0)
                    {
                        printf("Invalid calculation! \"%d %c %d\"\n", num1, op, num2);
                        validInput = false;
                        break;
                    }
                    result = num1 % num2;
                    break;
                default:
                    printf("Invalid calculation! \"%d %c %d\"\n", num1, op, num2);
                    validInput = false;
                    break;
            }

            if (validInput)
            {
                printf("> %d %c %d = %d\n", num1, op, num2, result);
            }
            else
            {
                break;
            }
        }
    }
    return 0;
}