int strcmp(char * str1, char * str2) {
	int s1;
    int s2;
    do {
        s1 = *str1++;
        s2 = *str2++;
        if (s1 == 0)
            break;
    } while (s1 == s2);
    return (s1 < s2) ? -1 : (s1 > s2);
}

char * strcpy(char * destination, char * origin) {
	int i = 0;
    while ((destination[i] = origin[i]) != '\0')
    {
        i++;
    }
	return destination;
}

int strlen(char * str) {
    int i = 0;
    while( str[i] != '\0' )
        i++;

    return i;
}

void alignString(char * str, char * buffer, int length) {
	int dif;
	int i, j = 0;
	for(i = 0; (i < length || length == 0) && str[i] != 0; i++) {
		buffer[i] = str[i];
	}

	if(i < length) {
		dif = (length - i);
		for(j = 0; j < dif; j++)
			buffer[i + j] = ' ';
	}

	buffer[i + j] = '\0';
}

// A utility function to check whether x is numeric
int isNumeric(char x)
{
    return (x >= '0' && x <= '9')? 1 : 0;
}

// A simple atoi() function. If the given string contains
// any invalid character, then this function returns 0
int atoi(char *str)
{
    if (*str == '\0')
       return 0;

    int res = 0;  // Initialize result
    int sign = 1;  // Initialize sign as positive
    int i = 0;  // Initialize index of first digit

    // If number is negative, then update sign
    if (str[0] == '-')
    {
        sign = -1;
        i++;  // Also update index of first digit
    }

    // Iterate through all digits of input string and update result
    for (; str[i] != '\0'; ++i)
    {
        if (isNumeric(str[i]) == 0)
            return 0; // You may add some lines to write error message
                      // to error stream
        res = res*10 + str[i] - '0';
    }

    // Return result with sign
    return sign*res;
}
