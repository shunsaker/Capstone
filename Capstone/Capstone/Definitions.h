#pragma once

#include <cmath>
#include <iostream>

#define PI 3.1415926535897932384626433832795
#define print std::cout<<
#define end <<std::endl

struct Complex {
	double real = 0;
	double imaginary = 0;

	Complex(int r, int i) {
		real = r;
		imaginary = i;
	}

	Complex() {}
};

double* mexFunction(double* x, int nsamples, int cf, int fs);

namespace staticTools{
	double* makeWave(int length, int frequency);
}