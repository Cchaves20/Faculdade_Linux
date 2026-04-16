#include <stdio.h>
#include <math.h>
#include "ex4.h"
#define _USE_MATH_DEFINES

int main() {
	double a[] = {M_PI_2, M_PI, 0, 0, M_PI_2};
	printf("%f\n", foo3(a,5));
	return 0;
}
