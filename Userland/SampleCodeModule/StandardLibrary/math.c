int sign(int n) {
	return (n < 0 ? -1 : 1);
}

int abs(int n) {
	return n * sign(n);
}

long long int pow(int base, int exp) {
    if( exp == 0 )
        return 1;

    int num = base;

    for(int i = 2; i <= exp; i++)
        num *= base;

    return num;

}