#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <limits>
#include <iomanip>
#include <sys/time.h> // gettimeofday in microseconds

double	currentTimeMicroseconds()
{
	struct	timeval	tv;
	if (gettimeofday(&tv, 0) != 0)
		return (0.0);
	double	secs = static_cast<double>(tv.tv_sec) * 1000000.0;
	double	usecs = static_cast<double>(tv.tv_usec);
	double	currentTime = secs + usecs;
	return (currentTime);
}

/*
	Sanity Check
	Both container should yield the same results
	The same algorithm
*/
bool	areSameResults(std::vector<unsigned int> &sortedVector, std::deque<unsigned int> &sortedDeque)
{
	if (sortedVector.size() != sortedDeque.size())
		return (false);
	else
	{
		size_t i = 0;
		while (i < sortedVector.size())
		{
			if (sortedVector[i] != sortedDeque[i])
				return (false);
			i++;
		}
	}
	return (true);
}

int	maxComparison(int ac)
{
	int sum = 0;
	for (int k = 1; k <= ac; ++k)
	{
		double value = (3.0 / 4.0) * k;
		sum += static_cast<int>(ceil(log2(value)));
	}
	return (sum);
}

void	printTimeDifference(const std::vector<unsigned int> &input,
						double timeDifferenceVector,
						double timeDifferenceDeque)
{
	std::cout.setf(std::ios::fixed);
	std::cout.precision(5);

	std::cout << "Time to process a range of "
				<< input.size()
				<< " elements with std::vector : "
				<< timeDifferenceVector * CONVERSION
				<< " s"
				<< std::endl;

	std::cout << "Time to process a range of "
				<< input.size()
				<< " elements with std::deque  : "
				<< timeDifferenceDeque * CONVERSION
				<< " s"
				<< std::endl;

}

void	printComparisonCount(int size, size_t vectorComparisons, size_t dequeComparisons)
{
	std::cout.unsetf(std::ios::floatfield);
	std::cout << "Size of Container: "
				<< size
				<< std::endl;
	std::cout << "Maximum Comparison: "
				<< maxComparison(size)
				<< std::endl;
	std::cout << "Comparisons with std::vector : "
				<< vectorComparisons
				<< std::endl;
	std::cout << "Comparisons with std::deque  : "
				<< dequeComparisons
				<< std::endl;
}

int main(int ac, char **av)
{
	std::vector<unsigned int> input;
	if (parseArgs(ac, av, input) == false)
		return (1);
	printLineWithCap<std::vector<unsigned int> >("Before: ", input, 10);
	PmergeMe	sorter;
	sorter.setInput(input);

	// Vector
	std::vector<unsigned int> sortedVector;
	double	t0 = currentTimeMicroseconds();
	sorter.sortUsingVector(sortedVector);
	double	t1 = currentTimeMicroseconds();

	// Deque
	std::deque<unsigned int> sortedDeque;
	double t2 = currentTimeMicroseconds();
	sorter.sortUsingDeque(sortedDeque);
	double t3 = currentTimeMicroseconds();

	// Check Results
	if (areSameResults(sortedVector, sortedDeque) == false)
	{
		std::cerr << "Error" << std::endl;
		return (1);
	}
	printLineWithCap<std::vector<unsigned int> >("After: ", sortedVector, 10);
	double	timeDifferenceVector = t1 - t0;
	double	timeDifferenceDeque = t3 - t2;
	printTimeDifference(input,
						timeDifferenceVector,
						timeDifferenceDeque);
	// printComparisonCount(sortedVector.size(), sorter.getVectorComparisonCount(), sorter.getDequeComparisonCount());
	return (0);
}
