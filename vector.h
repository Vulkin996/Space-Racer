#ifndef VECTOR
#define VECTOR
//struktura vektora radi lakseg cuvanja koordinata
typedef struct vector{
	double x,y,z;
} vector;

void printVector(vector v);
double dist(vector v1, vector v2);

#endif