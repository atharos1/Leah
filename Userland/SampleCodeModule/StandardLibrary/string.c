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