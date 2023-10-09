#include <stdio.h>
#include <string.h>
#include <math.h>

/**
  Array TO Integer
  Converts a string (aka an array of chars terminated by character '\0')
    to an int
*/
int my_atoi( const char *str ) {
    int length = strlen(str);
    int num = 0;
    int isNegative = 1;
    int power_of_ten = length - 1;
    for (int i = 0; i<length; i++)
    {
        if (i == 0) //first char
        {
            if (str[0] == '+') //if the first char is a positive sign
            {
                continue; //do nothing
            }
            else if (str[0] == '-') //if the first char is a negative sign
            {
                isNegative = -1; //the number is negative
                continue;
            }
            else if (str[0] == '0') //if the first char is a leading 0
            {
                
                continue; //do nothing
            }
            power_of_ten -- ;
        }
        //if first char is a num or we aren't on the first char
        num += ((int)str[i] - '0') * (int)pow(10, power_of_ten); //add whatever num we are currently on multiplied by the tens digit its at to our total number
        power_of_ten--; //tens digit decreases as we move down a place
    }
    return num * isNegative; //if the number had a negative sign we make sure to inclue that here
}