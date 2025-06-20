#include <stdio.h>


int fun(int a, int b)
{
	int x = a + b;
	int y = x + a + b;
	int z = x + y + 224;
	return 8*z;
}


void main()
{
	int i[] = {0,1,2,3,4,5};
	char *c = (char *)i;
	char d = *(c+5);
	int j = *(i+3);
	int k = fun(i[4], j);
	printf("The result of the fun is: %d\n", k);

}