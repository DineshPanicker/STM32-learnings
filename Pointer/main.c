#include<stdio.h>


long long int g_data = 0xFFFEABCD11112345;
int main()
{
	int *pAddress1;

	pAddress1 = (int*)&g_data;

	printf("Value of Addr is %p\n",pAddress1);
	printf("Value at address %p is: %x\n",pAddress1,*pAddress1);

	pAddress1 = pAddress1 + 1;

	printf("Value of Addr is %p\n",pAddress1);
	printf("Value at address %p is: %x\n",pAddress1,*pAddress1);



	return 0;
}
