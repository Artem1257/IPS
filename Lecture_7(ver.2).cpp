#include <iostream>
#include "C:\Program Files (x86)\IntelSWTools\compilers_and_libraries_2020.2.254\windows\compiler\include\omp.h"
#include <chrono>

long long num = 100000000;
double step;


// Parallel Calculation
std::chrono::duration<double> par(void)
{
	int num_of_threads = 3;
	long long inc = 0;
	double x = 0.0;
	double pi;
	double S = 0.0;
	step = 1.0 / (double)num;

	auto t1 = std::chrono::high_resolution_clock::now();

	#pragma omp parallel for private(x) reduction(+: S) num_threads(3)
	for (long long i = 0; i < num; i++)
	{
		x = (i + 0.5) * step;
		S += 4.0 / (1.0 + x * x);
		#pragma omp atomic
		inc++;
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = (t2 - t1);

	pi = step * S;
	printf("Result: pi = %.14f\n", pi);

	return duration;
}


int main()
{
	printf("Time: %f sec.\n", par());

	return 0;
}