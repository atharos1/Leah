#include "stdlib.h"

int strlen(char * str) {
    int i = 0;
    while(str[i] != 0)
        i++;

    return i;
}

void strcpy(char * dest, char * origin) {
    int i;
    for(i = 0; origin[i] != 0; i++)
        dest[i] = origin[i];

    dest[i+1] = 0;
}

int strcmp(char string1[], char string2[] ) {
    for (int i = 0; ; i++) {
        if (string1[i] != string2[i])
            return string1[i] < string2[i] ? -1 : 1;

        if (string1[i] == '\0')
            return 0;
    }
}