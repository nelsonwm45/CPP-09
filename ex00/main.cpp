#include "BitcoinExchange.hpp"

/*
	data.csv
	YYYY-MM-DD,exchange_rate (in this format)
	e.g.
	2011-01-03,0.3
	2011-01-09,0.32
	2012-01-11,7.1

	input file
	YYYY-MM-DD | value
	date | value
	2011-01-03 | 3
	2011-01-09 | 1
	2012-01-11 | 1.2
	2001-42-42

	std::map has lower_bound and keys are unique
*/

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: could not open file." << std::endl;
		return (1);
	}
	BitcoinExchange	be;
	if (be.loadCSVFile("data.csv") == false)
	{
		std::cerr << "Error: could not open file." << std::endl;
		return (1);
	}
	be.loadInputFile(av[1]);
	return (0);
}