#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <limits>
#include <sys/time.h> // gettimeofday in microseconds

int main(int ac, char **av)
{
	std::vector<unsigned int> input;
	if (parseArgs(ac, av, input) == false)
		return (1);
	printLineWithCap<std:vector<unsigned int> >("Before: ", input, 100);
	PmergeMe sorter;
	sorter.setInput(input);
}