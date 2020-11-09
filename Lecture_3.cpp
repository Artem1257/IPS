#include <stdio.h>
#include <ctime>
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/cilk.h"
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/cilk_api.h"
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/reducer_opadd.h"
#include "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2020.2.254/windows/compiler/include/icc/cilk/reducer_vector.h"
#include <chrono>

using namespace std::chrono;

const int MATRIX_SIZE = 1500;
const int TEST_MATRIX_SIZE = 4;



// ========== Initialize Matrix ==========
void InitMatrix(double** matrix)
{
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		matrix[i] = new double[MATRIX_SIZE + 1];
	}
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		for (int j = 0; j <= MATRIX_SIZE; ++j)
		{
			matrix[i][j] = rand() % 2500 + 1;
		}
	}
}



// ========== Serial Gauss ==========
duration<double> SerialGaussMethod(double** matrix, const int rows, double* result)
{
	double koef;
	
	// прямой ход метода Гаусса
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int k = 0; k < rows; ++k)
	{
		for (int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}
	auto t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);
	printf("Serial Gauss Time :\t%f\n", duration);

	// обратный ход метода Гаусса
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];
	
	for (int k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		for (int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		}

		result[k] /= matrix[k][k];
	}
	return duration;
}



// ========== Parallel Gauss ==========
duration<double> ParallelGaussMethod(double** matrix, const int rows, double* result)
{
	// прямой ход метода Гаусса
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int k = 0; k < rows; ++k)
	{
		cilk_for (int i = k + 1; i < rows; ++i)
		{
			double koef;
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}
	auto t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);
	printf("Parallel Gauss Time :\t%f\n", duration);

	// обратный ход метода Гаусса
	cilk::reducer_opadd<double> sum(0.0);
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];
	
	for (int k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];
		sum.set_value(0.0);
		
		for (int j = k + 1; j < rows; ++j)
		{
			sum += matrix[k][j] * result[j];
		}

		result[k] -= sum.get_value();
		result[k] /= matrix[k][k];
	}
	return duration;
}



// ========== Print Result ==========
void PrintResult(double* result, int size)
{
	for (int i = 0; i < size; ++i)
	{
		printf("x(%d) = %f;\t", i, result[i]);
	}
	printf("\n");
}



int main()
{
	srand((unsigned)time(0));
	int i;
	

	// Test Matrix Serial and Parallel
	double** test_matrix_serial = new double* [TEST_MATRIX_SIZE];
	double* test_result_serial = new double[TEST_MATRIX_SIZE];

	double** test_matrix_parallel = new double* [TEST_MATRIX_SIZE];
	double* test_result_parallel = new double[TEST_MATRIX_SIZE];

	for (i = 0; i < TEST_MATRIX_SIZE; ++i)
	{
		test_matrix_serial[i] = new double[TEST_MATRIX_SIZE  + 1];
		test_matrix_parallel[i] = new double[TEST_MATRIX_SIZE + 1];
	}

	test_matrix_serial[0][0] = 2; test_matrix_serial[0][1] = 5;  test_matrix_serial[0][2] = 4;  test_matrix_serial[0][3] = 1;  test_matrix_serial[0][4] = 20;
	test_matrix_serial[1][0] = 1; test_matrix_serial[1][1] = 3;  test_matrix_serial[1][2] = 2;  test_matrix_serial[1][3] = 1;  test_matrix_serial[1][4] = 11;
	test_matrix_serial[2][0] = 2; test_matrix_serial[2][1] = 10; test_matrix_serial[2][2] = 9;  test_matrix_serial[2][3] = 7;  test_matrix_serial[2][4] = 40;
	test_matrix_serial[3][0] = 3; test_matrix_serial[3][1] = 8;  test_matrix_serial[3][2] = 9;  test_matrix_serial[3][3] = 2;  test_matrix_serial[3][4] = 37;

	test_matrix_parallel[0][0] = 2; test_matrix_parallel[0][1] = 5;  test_matrix_parallel[0][2] = 4;  test_matrix_parallel[0][3] = 1;  test_matrix_parallel[0][4] = 20;
	test_matrix_parallel[1][0] = 1; test_matrix_parallel[1][1] = 3;  test_matrix_parallel[1][2] = 2;  test_matrix_parallel[1][3] = 1;  test_matrix_parallel[1][4] = 11;
	test_matrix_parallel[2][0] = 2; test_matrix_parallel[2][1] = 10; test_matrix_parallel[2][2] = 9;  test_matrix_parallel[2][3] = 7;  test_matrix_parallel[2][4] = 40;
	test_matrix_parallel[3][0] = 3; test_matrix_parallel[3][1] = 8;  test_matrix_parallel[3][2] = 9;  test_matrix_parallel[3][3] = 2;  test_matrix_parallel[3][4] = 37;
	

	// Matrix For Time Comparison
	double** matrix_serial = new double* [MATRIX_SIZE];
	double** matrix_parallel = new double* [MATRIX_SIZE];
	double* result = new double[MATRIX_SIZE];
	InitMatrix(matrix_serial);
	InitMatrix(matrix_parallel);


	printf("\n========== Compare Serial and Parallel Results ==========\n\n");
	SerialGaussMethod(test_matrix_serial, TEST_MATRIX_SIZE, test_result_serial);
	ParallelGaussMethod(test_matrix_parallel, TEST_MATRIX_SIZE, test_result_parallel);

	PrintResult(test_result_serial, TEST_MATRIX_SIZE);
	PrintResult(test_result_parallel, TEST_MATRIX_SIZE);


	printf("\n========== Compare Serial and Parallel Time ==========\n\n");
	auto t1 = SerialGaussMethod(matrix_serial, MATRIX_SIZE, result);
	auto t2 = ParallelGaussMethod(matrix_parallel, MATRIX_SIZE, result);
	printf("Speed Growth :\t%f\n", t1/t2);


	// Clear Memory
	for (i = 0; i < MATRIX_SIZE; ++i)
	{
		delete[] matrix_serial[i];
		delete[] matrix_parallel[i];
	}
	for (i = 0; i < TEST_MATRIX_SIZE; ++i)
	{
		delete[] test_matrix_serial[i];
		delete[] test_matrix_parallel[i];
	}
	delete[] test_result_serial;
	delete[] test_result_parallel;
	delete[] result;

	return 0;
}