#include<stdio.h>

int main()
{
	char c1,c2,c3,c4,c5,c6;
	printf("Enter any 6 characters of your choice! \n");
	scanf("%c %c %c %c %c %c",&c1,&c2,&c3,&c4,&c5,&c6);

	printf("\nASCII codes %u %u %u %u %u %u",c1,c2,c3,c4,c5,c6);

	printf("\nPress enter key to exit the appn!\n");
		while(getchar() != '\n'){}
		getchar();
		return 0;
	return 0;
}
