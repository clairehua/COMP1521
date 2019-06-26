// Copy input to output
// COMP1521 18s1

#include <stdlib.h>
#include <stdio.h>

void copy(FILE *, FILE *);

int main(int argc, char *argv[])
{
	if (argc == 1) {
		copy(stdin,stdout);
	}
	else {
		for (int i = 1; i < argc; i++) {
		// attempt to open the named file for reading
		FILE *file = fopen(argv[i], "r");
			// if it fails to open
			if (file == NULL) {
				printf("Can't read NameOfFile");
			}
			else {
				copy(file,stdout);
				fclose(file);
			}	
		}
	}

	return EXIT_SUCCESS;
}

// Copy contents of input to output, char-by-char
// Assumes both files open in appropriate mode

void copy(FILE *input, FILE *output)
{
	char line[BUFSIZ] = {'\0'};
	while (fgets(line, BUFSIZ, input) != NULL) {
		fputs(line, output);
	}
}
