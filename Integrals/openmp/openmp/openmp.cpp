#include <omp.h>
#include <iostream>
#include <random>
#include "../../integral_examples.h"
#include <ctime>
#include <iomanip>

double openmp_monte_carlo_integral(double(*f)(double), const double x_min, const double x_max, const double y_min,
	const double y_max, const int n, const int number_of_threads)
{
	const auto number_of_points = static_cast<int>(ceil(1. * n / number_of_threads));
	auto total_in_box = 0;
#pragma omp parallel num_threads(number_of_threads)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		const std::uniform_real_distribution<> dis(0.0, 1.0);
		auto in_box = 0;
		for (auto j = 0; j < number_of_points; j++)
		{
			const auto x = ((x_max - x_min) * dis(gen)) + x_min;
			const auto y = ((y_max - y_min) * dis(gen)) + y_min;
			const auto val = f(x);
			if (val > y)
			{
				++in_box;
			}
		}
		total_in_box += in_box;
	}
	return (x_max - x_min) * (y_max - y_min) * total_in_box / (ceil(1. * n / number_of_threads) * number_of_threads);
}

double openmp_simpson_integral(double(*f)(double), const double x_from, const double x_to, const int n, const int number_of_threads)
{
	double result = 0;
	const auto delta = (x_to - x_from) / n;
	const int number_of_iterations = ceil(1. * n / number_of_threads);
#pragma omp parallel num_threads(number_of_threads)
	{
		auto start = x_from + delta * number_of_iterations * omp_get_thread_num();
		double thread_result = 0;
		for (auto j = 0; j < number_of_iterations; j++)
		{
			thread_result += delta / 6 * (f(start) + 4 * f(start + delta / 2) + f(start + delta));
			start += delta;
		}
		result += thread_result;
	}
	return result;
}

double openmp_gaussian_integral(double(*f)(double), const double x_from, const double x_to, const int n, const int number_of_threads)
{
	const auto x1 = (sqrt(5. - 2. * sqrt(10. / 7))) / 3.;
	const auto x2 = (sqrt(5. + 2. * sqrt(10. / 7))) / 3.;
	const auto w1 = 128. / 225;
	const auto w2 = (322. + 13. * sqrt(70.)) / 900.;
	const auto w3 = (322. - 13. * sqrt(70.)) / 900.;
	const double x[]{ 0, x1, -1. * x1, x2, -1. * x2 };
	const double w[]{ w1, w2, w2, w3, w3 };

	double result = 0;

	const auto delta((x_to - x_from) / n);
	const auto number_of_iterations(ceil(1. * n / number_of_threads));

#pragma omp parallel num_threads(number_of_threads)
	{
		auto start = x_from + delta * number_of_iterations * omp_get_thread_num();
		double thread_result = 0;

		for (auto k = 0; k < number_of_iterations; k++)
		{
			double current_result = 0;
			for (auto j = 0; j < 5; j++)
			{
				current_result += w[j] * f(delta / 2. * x[j] + start + delta / 2.);
			}
			current_result *= delta / 2.;
			thread_result += current_result;
			start += delta;
		}

		result = result + thread_result;
	}
	return result;
}

int main()
{
	/*std::cout << openmp_monte_carlo_integral(f1, 0, exp(1), 0, 30, 1000000, 8) << " " <<
				 openmp_simpsons_integral(f1, 0, exp(1), 1000000, 8) << " " <<
				 openmp_gaussian_integral(f1, 0, exp(1), 1000000, 8);*/

	int n = 100000000;
	int number_of_threads = 8;

	double res_min, res_avg, res_max,
		time_min, time_avg, time_max,
		err_min, err_avg, err_max,
		val = 14.1542622414793;

	clock_t begin = clock();
	res_min = res_avg = res_max = openmp_monte_carlo_integral(f1, 0, exp(1),0, 20, n, number_of_threads);
	clock_t end = clock();
	time_min = time_avg = time_max = double(end - begin) / CLOCKS_PER_SEC;
	err_min = err_avg = err_max = abs(res_min - val);

	for (auto i = 1; i < 20; ++i)
	{
		begin = clock();
		double cur_res = openmp_monte_carlo_integral(f1, 0, exp(1), 0, 20, n, number_of_threads);
		end = clock();

		res_avg += cur_res;
		if (res_min > cur_res)
			res_min = cur_res;
		if (res_max < cur_res)
			res_max = cur_res;

		double cur_time = double(end - begin) / CLOCKS_PER_SEC;
		time_avg += cur_time;
		if (time_min > cur_time)
			time_min = cur_time;
		if (time_max < cur_time)
			time_max = cur_time;

		double cur_err = abs(cur_res - val);
		err_avg += cur_err;
		if (err_min > cur_err)
			err_min = cur_err;
		if (err_max < cur_err)
			err_max = cur_err;

	}
	res_avg /= 20;
	time_avg /= 20;
	err_avg /= 20;
	std::cout << std::setprecision(15) <<
		res_min << "\n" << res_avg << "\n" << res_max << "\n" <<
		time_min << "\n" << time_avg << "\n" << time_max << "\n" <<
		err_min << "\n" << err_avg << "\n" << err_max << "\n";
	return 0;
}
