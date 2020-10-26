#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/cilk.h"
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/cilk_api.h"
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/reducer_max.h"
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/reducer_min.h"
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/reducer_vector.h"
#include <chrono>
#include <vector>

#define start_size 10000

using namespace std::chrono;



// ========== Initialize Array ==========
void ArrayInit(int* mass, int mass_size)
{
	long i;
	for (i = 0; i < mass_size; ++i)
	{
		mass[i] = (rand() % 25000) + 1;
	}
}



// ========== Find Max Element ==========
void ReducerMaxTest(int* mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("Maximal element = %d has index = %d\n",
		maximum->get_reference(), maximum->get_index_reference());
}



// ========== Find Min Element ==========
void ReducerMinTest(int* mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Minimal element = %d has index = %d\n",
		minimum->get_reference(), minimum->get_index_reference());
}



// ========== Sort Array ==========
void ParallelSort(int* begin, int* end)
{

	if (begin != end)
	{
		--end;
		int* middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle);
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
}

void Sorting(int* begin, int* end)
{
	auto t1 = std::chrono::high_resolution_clock::now();
	ParallelSort(begin, end);
	auto t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);

	printf("Sort time : %f\n", duration);
}



// ========== Compare Array Initializing Time ==========
void CompareForAndCilk_For(int sz)
{
	std::vector<int> vec;
	cilk::reducer<cilk::op_vector<int>>red_vec;

	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < sz; ++i)
	{
		vec.push_back(rand() % 20000 + 1);
	}
	auto t2 = high_resolution_clock::now();
	duration<double> duration_vec = (t2 - t1);
	
	t1 = std::chrono::high_resolution_clock::now();
	cilk_for(int i = 0; i < sz; ++i)
	{
		red_vec->push_back(rand() % 20000 + 1);
	}
	t2 = high_resolution_clock::now();
	duration<double> duration_cilk = (t2 - t1);
	
	printf("Size : %d,\t Time (for) : %f,\t Time (cilk) : %f\n", sz, duration_vec, duration_cilk);
}



// ========== MAIN ==========
int main()
{
	srand((unsigned)time(0));
	__cilkrts_set_param("nworkers", "4");

	long mass_size;
	int* mass_begin, * mass_end;
	int* mass;


	printf("========== Task 2 ==========\n\n");

	mass_size = start_size;
	mass = new int[mass_size];
	ArrayInit(mass, mass_size);
	mass_begin = mass;
	mass_end = mass_begin + mass_size;

	ReducerMinTest(mass, mass_size);
	ReducerMaxTest(mass, mass_size);
	Sorting(mass_begin, mass_end);
	ReducerMinTest(mass, mass_size);
	ReducerMaxTest(mass, mass_size);
	delete[] mass;


	printf("\n========== Task 3 ==========\n\n");

	std::vector<int> size_mult = {10, 50, 100};
	for (int i : size_mult)
	{
		mass_size = start_size * i;
		printf("Array length : %d,\t", mass_size);
		mass = new int[mass_size];
		ArrayInit(mass, mass_size);
		mass_begin = mass;
		mass_end = mass_begin + mass_size;
		Sorting(mass_begin, mass_end);
		delete[] mass;
	}


	printf("\n========== Task 4 ==========\n\n");

	std::vector<int> sizes = {10, 50, 100, 1000, 10000, 100000, 1000000};
	for (int i : sizes)
	{
		CompareForAndCilk_For(i);
	}


	return 0;
}