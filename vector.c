#include <stdio.h>
#include <math.h>
#include "vector.h"

//stampanje vektora korisceno debagovanju
void printVector(vector v){
	printf("%lf %lf %lf\n", v.x,v.y,v.z);
}
//fja euklidskog rastojanja 2 tacke u prostoru
double dist(vector v1, vector v2){
	return sqrt( (v2.x - v1.x)*(v2.x - v1.x) + (v2.y - v1.y)*(v2.y - v1.y) + (v2.z - v1.z)*(v2.z - v1.z) );
}