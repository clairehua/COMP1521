#include <stdio.h>

int main () {


/*
printf("%d \n", (0x3F | 0x80));
printf("%d \n", (0x33 ^ 0xCC));
printf("%d \n", (0x15 ^ 0x15));
printf("%d \n", (~0x42));
printf("%d \n", (0x30 >> 0x04));
printf("%d \n", (0x0F << 0x0F));
printf("%d \n", (0x03 | 0x0C));
*/


int a =  (0x03 | 0x0C);
unsigned int mask = 00000001;

int i = 0;
while (i < 8) {
	if ((a & mask) > 0) {
	
	printf("1");
	}
	else {
		printf("0");
	}
	mask = mask << 1;
	i++;
}


printf("\n");


return 0;
}


//0 10000010 01000000000000000000000

//sign exp matissa



//1 01111111 10000000000000000000000
