// Students.c ... implementation of Students datatype

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "Students.h"


int ordered(char *name1, char *name2);
StuRec copy(StuRec dest, StuRec src);


typedef struct _stu_rec {
	int   id;
	char  name[20];
	int   degree;
	float wam;
} sturec_t;

typedef struct _students {
    int    nstu;
    StuRec recs;
} students_t;

// build a collection of student records from a file descriptor
Students getStudents(int in)
{
    int ns;  // count of #students

	// Make a skeleton Students struct
	Students ss;
	if ((ss = malloc(sizeof (struct _students))) == NULL) {
		fprintf(stderr, "Can't allocate Students\n");
		return NULL;
	}

	// count how many student records
    int stu_size = sizeof(struct _stu_rec);
    sturec_t s;
	ns = 0;
    while (read(in, &s, stu_size) == stu_size) ns++;
    ss->nstu = ns;
    if ((ss->recs = malloc(ns*stu_size)) == NULL) {
		fprintf(stderr, "Can't allocate Students\n");
		free(ss);
		return NULL;
	}

	// read in the records
	lseek(in, 0L, SEEK_SET);
	for (int i = 0; i < ns; i++)
		read(in, &(ss->recs[i]), stu_size);

	close(in);
	return ss;
}

// show a list of student records pointed to by ss
void showStudents(Students ss)
{
	for (int i = 0; i < ss->nstu; i++)
		showStuRec(&(ss->recs[i]));
}

// show one student record pointed to by s
void showStuRec(StuRec s)
{
	printf("%7d %s %4d %0.1f\n", s->id, s->name, s->degree, s->wam);
}

void sortByName(Students ss)
{
	int i = 0;
	int nswaps = 1;
	while (nswaps != 0) {
		nswaps = 0;
		i = 0;
		while (i < ss->nstu - 1) {
			// if recs[i] > recs[i+1], swap
			if (ordered(ss->recs[i].name, ss->recs[i+1].name) == 0) {
				// swap
				StuRec tmp = malloc(sizeof(struct _stu_rec));
				if (tmp == NULL)
					fprintf(stderr, "not enough memory\n");		
				copy(tmp, &(ss->recs[i]));
				copy(&(ss->recs[i]), &(ss->recs[i+1]));
				copy(&(ss->recs[i+1]), tmp);
				nswaps++;
				free(tmp);
			}
			i++;
		}
	}
}

int ordered(char *name1, char *name2) {
	int i = 0;
	// check first letter
	if (name1[i] < name2[i]) {
		return 1;
	}
	if (name1[i] > name2[i]) {
		return 0;
	}
	i++;
	// check other letters
	while (name1[i] != '\0' && name2[i] != '\0') {
		if (name1[i] < name2[i]) {
			return 1;
		}
		else if (name1[i] > name2[i]) {
			return 0;
		}
		i++;
	}
	return 0;
}

StuRec copy(StuRec dest, StuRec src) {
	dest->id = src->id;
	dest->degree = src->degree;
	dest->wam = src->wam;
	
	// dest->name = strdup(src->name);
	int i = 0;
	while (src->name[i] != '\0') {
		dest->name[i] = src->name[i];
		i++;
	}
	
	return dest;
}
