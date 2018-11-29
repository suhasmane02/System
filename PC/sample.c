#include <stdio.h>

int main()
{
	char *ptr = "hello";
	int q = 10;
	int *p = &q;
	printf("addr = %p\n", ptr);
	printf("addr = %p\n", ptr+2);
	printf("addrp = %p\n", p);
	printf("addrp = %p\n", p+1);
	return 0;
}
