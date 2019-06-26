#include <stdio.h>


int main (){


	unsigned int a, b, c;
	a = 0b101010101010101;  b = 0b1010101010101010;  c = 0b1;
	//printf("%d, %d, %d", a,b, c);
	printf("%d\n",	a | b );
	0xFFFF
	printf("%d\n",	a & b );
	0x0000
	printf("%d\n",	a ^ b );
	0xFFFF
	printf("%d\n",	a & (b << 1) );
	0x5554
	printf("%d\n",	a & ~c );
	0x5554
}
