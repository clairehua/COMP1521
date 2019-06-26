// BigNum.h ... LARGE positive integer values

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "BigNum.h"

#define TRUE 1
#define FALSE 0

// Initialise a BigNum to N bytes, all zero
void initBigNum(BigNum *n, int Nbytes)
{
   n->nbytes = Nbytes;
   n->bytes = malloc(Nbytes);
   assert(n->bytes != NULL);
   return;
}

// Add two BigNums and store result in a third BigNum
void addBigNums(BigNum n, BigNum m, BigNum *res)
{
	int i = 0;
	int sum = 0;
	int carry = 0;

	int max = n.nbytes;
	if (m.nbytes > max) {
		max = m.nbytes;
	}

	if (res->nbytes < max) {
		res->bytes = realloc(res->bytes, max + 1); // addition will only result in num 1 bigger max
		assert(res->bytes != NULL);
		res->nbytes = max + 1;
	}

	// if n is longer than m
	if (n.nbytes > m.nbytes) {
		for (i = 0; i < m.nbytes; i++) {
			if (n.bytes[i] == '\0' && m.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
			}
			else {
				if (n.bytes[i] == '\0') {
					sum = (m.bytes[i] - '0') + carry;
				}
				else if (m.bytes[i] == '\0') {
					sum = (n.bytes[i] - '0') + carry;
				}
				else {
					sum = (n.bytes[i] - '0') + (m.bytes[i] - '0') + carry;
				}
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
		}
		while (i < n.nbytes) {
			if (n.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
				sum = 0;
			}
			else {
				sum = (n.bytes[i] - '0') + carry;
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
			i++;
		}	
		while (i < res->nbytes) {
			res->bytes[i] = '\0';
			i++;
		}			
	}
	// if m is longer than n
	else if (m.nbytes > n.nbytes) {
		for (i = 0; i < n.nbytes; i++) {
			if (n.bytes[i] == '\0' && m.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
				sum = 0;
			}
			else {
				if (n.bytes[i] == '\0') {
					sum = (m.bytes[i] - '0') + carry;
				}
				else if (m.bytes[i] == '\0') {
					sum = (n.bytes[i] - '0') + carry;
				}
				else {
					sum = (n.bytes[i] - '0') + (m.bytes[i] - '0') + carry;
				}
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
		}
		while (i < m.nbytes) {
			if (m.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
				sum = 0;
			}
			else {
				sum = (m.bytes[i] - '0') + carry;
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
			i++;
			i++;
		}
		while (i < res->nbytes) {
				res->bytes[i] = '\0';
			i++;
		}		
	}
	// equal lengths
	else {
		for (i = 0; i < n.nbytes; i++) {
			if (n.bytes[i] == '\0' && m.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
			}
			else {
				if (n.bytes[i] == '\0') {
					sum = (m.bytes[i] - '0') + carry;
				}
				else if (m.bytes[i] == '\0') {
					sum = (n.bytes[i] - '0') + carry;
				}
				else {
					sum = (n.bytes[i] - '0') + (m.bytes[i] - '0') + carry;
				}
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
		}
		while (i < res->nbytes) {
			res->bytes[i] = '\0';
			i++;
		}	
	}

   return;
}

// Set the value of a BigNum from a string of digits
// Returns 1 if it *was* a string of digits, 0 otherwise
int scanBigNum(char *s, BigNum *n)
{
	int i = 0;
	int end = 0;
	int start = 0;
	int num = 0;


	// int max = strlen(s);
	// if (n->nbytes < max) {
	// 	n->bytes = realloc(n->bytes, 2*max);
	// 	assert(n->bytes != NULL);
	// }

	// finding first digit
	while (('0' < s[i] && s[i] <= '9') != 1) {
		if (s[i] == '\0') {
			return 0;
		}
		i++;
	}
	// now at the index of the first number
	start = i;
	while ('0' <= s[i] && s[i] <= '9') {
		i++;
	}


	// now past last number
	end = i - 1;

	if (i > 20) {
		n->bytes = realloc(n->bytes, 2*n->nbytes);
		n->nbytes = 2*n->nbytes;
		assert(n->bytes != NULL);
	}

	//num = end - start;
	i = 0;
	num = n->nbytes;
	while (end >= start) {
		n->bytes[i] = s[end];
		end--;
		num--;
		i++;
	}
	while (i <= num) {
		n->bytes[i] = '\0';
		i++;
	}

	return 1;
}

// Display a BigNum in decimal format
void showBigNum(BigNum n)
{
	// size of array
	int size = n.nbytes;
	// largest index is one less than size of array
	int index = size - 1;
	int startFound = FALSE;

	while (index >= 0) { 
		if (startFound == FALSE) {
			if (n.bytes[index] != 0) {
				startFound = TRUE;
				printf("%c", n.bytes[index]);
			} 
		}
		else {
			printf("%c", n.bytes[index]);
	  	}
	index--;
	}
   
   return;
}

// Subtract two BigNums and store result in a third BigNum
void subtractBigNums(BigNum n, BigNum m, BigNum *res) {
	int i = 0;
	int sum = 0;
	int carry = 0;
	int num_neg = 0;

	int max = n.nbytes;
	if (m.nbytes > max) {
		max = m.nbytes;
	}

	if (res->nbytes < max) {
		res->bytes = realloc(res->bytes, max); // number can't be larger when subtracting
		assert(res->bytes != NULL);
		res->nbytes = max;
	}

	// if n is longer than m
	if (n.nbytes > m.nbytes) {
		for (i = 0; i < m.nbytes; i++) {
			if (n.bytes[i] == '\0' && m.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
			}
			else {
				if (n.bytes[i] == '\0') {
					sum = (m.bytes[i] - '0') + carry;
				}
				else if (m.bytes[i] == '\0') {
					sum = (n.bytes[i] - '0') + carry;
				}
				else {
					sum = (n.bytes[i] - '0') + (m.bytes[i] - '0') + carry;
				}
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
		}
		while (i < n.nbytes) {
			if (n.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
				sum = 0;
			}
			else {
				sum = (n.bytes[i] - '0') + carry;
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
			i++;
		}	
		while (i < res->nbytes) {
			res->bytes[i] = '\0';
			i++;
		}			
	}
	// if m is longer than n
	else if (m.nbytes > n.nbytes) {
		for (i = 0; i < n.nbytes; i++) {
			if (n.bytes[i] == '\0' && m.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
				sum = 0;
			}
			else {
				if (n.bytes[i] == '\0') {
					sum = (m.bytes[i] - '0') + carry;
				}
				else if (m.bytes[i] == '\0') {
					sum = (n.bytes[i] - '0') + carry;
				}
				else {
					sum = (n.bytes[i] - '0') + (m.bytes[i] - '0') + carry;
				}
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
		}
		while (i < m.nbytes) {
			if (m.bytes[i] == '\0') {
				if (carry == 0) {
					res->bytes[i] = '\0';
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
				sum = 0;
			}
			else {
				sum = (m.bytes[i] - '0') + carry;
				if (sum > 9) {
					sum -= 10;
					carry = 1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
			i++;
			i++;
		}
		while (i < res->nbytes) {
				res->bytes[i] = '\0';
			i++;
		}		
	}
	// equal lengths
	else {
		for (i = 0; i < n.nbytes; i++) {
			if (n.bytes[i] == '\0' && m.bytes[i] == '\0') {
				if (carry == 0) {
					if (num_neg == 1) {
						res->bytes[i] = '-';
						num_neg = 0;
					}
					else {
						res->bytes[i] = '\0';
					}
				}
				else {
					res->bytes[i] = '1';
				}
				carry = 0;
			}
			else {
				if (n.bytes[i] == '\0') {
					sum = (m.bytes[i] - '0') + carry;
					num_neg = 1;
				}
				else if (m.bytes[i] == '\0') {
					sum = (n.bytes[i] - '0') + carry;
				}
				else {
					sum = (n.bytes[i] - '0') - (m.bytes[i] - '0') + carry;
				}
				if (sum < 0) {
					sum += 10;
					carry = -1;
				}
				else {
					carry = 0;
				}
				res->bytes[i] = sum + '0';
			}
		}
		while (i < res->nbytes) {
			res->bytes[i] = '\0';
			i++;
		}	
	}

   return;
}