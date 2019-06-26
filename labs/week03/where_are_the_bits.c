// where_are_the_bits.c ... determine bit-field order
// COMP1521 Lab 03 Exercise
// Written by Claire Hua

#include <stdio.h>
#include <stdlib.h>

struct _bit_fields {
   unsigned int a : 4,
                b : 8,
                c : 20;
};
// 1100 0001
/* Little endian
0001
1100 -> 0001 1100
*/

/* big 
1100
0001 -> 1100 0001
*/

/* WRONG
1000 0011
*/

/*
00000000001


little

0001 0000000000000

big

00000.... 0001*/


int main(void)
{
	struct _bit_fields x = {1, 1, 1};
	// big endian looks like:
		// 1 10000000 10000000000000000000000
		// but will print 
		// 10000000000000000000000 10000000 1
	// opposite for little endian

	unsigned int *ptr = (unsigned int *)&x;
	unsigned int mask = 1;
	int i = 0;
	
	while (i < 32) {
		if ((*ptr & mask) > 0) {
			//bit at i = 1;
			printf("1");
		}
		else {
			// bit at i = 0;
			printf("0");
		}
		i++;
		mask = mask << 1;
	} 
	
	printf("\n%u\n", sizeof(x));

	return 0;
}
