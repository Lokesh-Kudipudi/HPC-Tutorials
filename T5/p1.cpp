#include <iostream>
#include <vector>
#include <omp.h>

// simple function f(i)
inline double f(int i)
{
  return 1.0 * (i + 1); // just returns i+1 as double
}

int main()
{
  // Problem sizes
  int m = 10; // length of v
  int n = 20; // number of updates

  // Initialize v with zeros
  std::vector<double> v(m, 0.0);

  // Build indices array with values in [0, m)
  std::vector<int> indices = {
      0,
      1,
      2,
      3,
      4,
      5,
      6,
      6,
      6,
      6,
      6,
      7,
      8,
      9,
      5,
      7,
      8,
      8,
      8,
      8};

// Parallel region
#pragma omp parallel for
  for (int i = 0; i < n; i++)
  {
    v[indices[i]] += f(i);
  }

  // Print result
  std::cout << "Resulting vector v:" << std::endl;
  for (int j = 0; j < m; j++)
  {
    std::cout << "v[" << j << "] = " << v[j] << std::endl;
  }

  return 0;
}
