#include <stdio.h>


int main() {

	int x = 0x602A;
			printf("page is %d\n", x/8192);
		printf("remainder is %d\n", x%8192);
	printf("%d\n", x%8192 + 2*8192);
return 0;
}
