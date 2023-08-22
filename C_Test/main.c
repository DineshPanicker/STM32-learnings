#include<stdio.h>


int main(){

	float number1,number2,number3;
	float average;

	printf("Enter 3 nos ");
	scanf("%f %f %f",&number1,&number2,&number3);

	average = (number1 + number2 + number3)/3;
	printf("Average is: %f",average);

	printf("Press enter key to exit the appn!\n");
	while(getchar() != '\n'){}
	getchar();
	return 0;
}
