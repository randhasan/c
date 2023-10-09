#include <stdio.h>
#include <string.h>
#include <math.h>

/**
  Integer TO Array
  Converts an int, to a string (aka an array of chars terminated by character '\0')
    If base is 10, negative numbers are prefixed with a - sign.
    All other bases are assumed to be unsigned.
    str WILL be enough to hold the number! Including the nul terminator `\0`
*/
void my_itoa( int value, char *str, int base ) {
    int isNegative = 0;
    int index = 0;
    
    if (value < 0 && base == 10) //value is negative
    {
        isNegative = 1;
        value *= -1; //make positive
    }
  
    do {
        if (base!=10)
        {
            unsigned int digit = (unsigned)value % base;
            if (digit < 10) 
            {
                str[index++] = '0' + digit;
            } 
            else 
            {
                str[index++] = 'A' + (digit - 10);
            }
            
        }
        else
        {
            int digit = value % base;
            if (digit < 10) 
            {
                str[index++] = '0' + digit;
            } 
            else 
            {
                str[index++] = 'A' + (digit - 10);
            }
        }
        value /= (unsigned)base; //operations are automatically signed
    } while (value > 0);
    
    if (isNegative == 1 && base == 10) //only base 10 is negative the other bases are unsighted
    {
        str[index++] = '-';
    }
    
    str[index] = '\0'; //concatenate sentinel char

    int start = 0; //reverse string bc it is backwards
    int end = index - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// No main function
int main() {
    char str[100];

    my_itoa(-1, str, 10);
    printf("Base 10: %s\n", str);

    my_itoa(-1, str, 2);
    printf("Base 2: %s\n", str);

    my_itoa(-1, str, 8);
    printf("Base 8: %s\n", str);

    my_itoa(-1, str, 16);
    printf("Base 16: %s\n", str);

    my_itoa(1, str, 10);
    printf("Base 10: %s\n", str);

    my_itoa(1, str, 2);
    printf("Base 2: %s\n", str);

    my_itoa(1, str, 8);
    printf("Base 8: %s\n", str);

    my_itoa(1, str, 16);
    printf("Base 16: %s\n", str);

    return 0;
}