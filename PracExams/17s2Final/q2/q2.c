// COMP1521 Final Exam
// Read points and determine bounding box

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// Data type definitions

// all values are in the range 0..255
typedef unsigned char Byte;

// an (x,y) coordinate
typedef struct {
	Byte x;
	Byte y;
} Coord;

// a colour, given as 3 bytes (r,g,b)
typedef struct {
	Byte r;
	Byte g;
	Byte b;
} Color;

// a Point has a location and a colour
typedef struct {
	Coord coord;  // (x,y) location of Point
	Color color;  // colour of Point
} Point;

void boundingBox(int, Coord *, Coord *);

int main(int argc, char **argv)
{
	// check command-line arguments
	if (argc < 2) {
		fprintf(stderr, "Usage: %s PointsFile\n", argv[0]);
		exit(1);
	}

	// attempt to open specified file
	int in = open(argv[1],O_RDONLY);
	if (in < 0) {
		fprintf(stderr, "Can't read %s\n", argv[1]);
		exit(1);
	}

	// collect coordinates for bounding box
	Coord topLeft, bottomRight;
	boundingBox(in, &topLeft, &bottomRight);

	printf("TL=(%d,%d)  BR=(%d,%d)\n",
		 topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

	// clean up
	close(in);
	return 0;
}

void boundingBox(int in, Coord *TL, Coord *BR)
{
	Point cur;
	int first = 1;
	int leftX; // stores smallest x value
	int leftY; // stores largest y value
	int rightX; // stores largest x value
	int rightY; // stores smallest y value

	while (read(in, &cur, sizeof(Point)) == sizeof(Point)) {
		if (first == 1) {
			leftX = cur.coord.x;
			rightX = cur.coord.x;			
			leftY = cur.coord.y;			
			rightY = cur.coord.y;	
			first = 0;		
		}
		else {
			if (cur.coord.x < leftX) {
				leftX = cur.coord.x;
			}
			if (cur.coord.x > rightX) {
				rightX = cur.coord.x;
			}
			if (cur.coord.y > leftY) {
				leftY = cur.coord.y;
			}
			if (cur.coord.y < rightY) {
				rightY = cur.coord.y;
			}
		}
	}
	
	TL->x = leftX;
	TL->y = leftY;
	BR->x = rightX;
	BR->y = rightY;
}
