#include <iostream>
#include <omp.h>
#include <time.h>

static long num_steps = 100000;
double step;

int main()
{
  int i;
  double x, pi, sum = 0.0;
  step = 1.0 / (double)num_steps;

  // start time
  clock_t start = clock();
#pragma omp parallel for default(none) shared(num_steps, step, sum) private(i, x)
  for (i = 0; i < num_steps; i++)
  {
    x = (i + 0.5) * step;
#pragma omp atomic
    sum = sum + 4.0 / (1.0 + x * x);
  }
  // end time
  clock_t end = clock();
  pi = step * sum;
  std::cout << pi << std::endl;
  double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
  std::cout << "Time taken: " << time_taken << " seconds" << std::endl;

  return 0;
}