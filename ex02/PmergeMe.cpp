#include "PmergeMe.hpp"
#include <algorithm> // std::sort
#include <limits>

// ==============================================================
// Ctors & Dtors & Copy Assignment Operator
// ==============================================================
PmergeMe::PmergeMe()
{}

PmergeMe::~PmergeMe()
{}

PmergeMe::PmergeMe(const PmergeMe &other):
	_vec(other._vec),
	_deq(other._deq)
{}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	if (this != &other)
	{
		_vec = other._vec;
		_deq = other._deq;
	}
	return (*this);
}

// ==============================================================
// Public API
// ==============================================================
void	PmergeMe::setInput(const std::vector<unsigned int> &input)
{
	this->_vec = input;
	_deq.clear();
	size_t	i = 0;
	while (i < values.size())
	{
		_deq.push_back(input[i]);
		i++;
	}
}

void	PmergeMe::sortUsingVector(std::vector<unsigned int> &outSorted)
{
	outSorted = this->_vec;
	if (outSorted.size() <= 1)
		return ;
	fjSortVector(outSorted);
}


// ==============================================================
// Ford-Johnson (Vector)
// ==============================================================



// ==============================================================
// Ford-Johnson (Deque)
// ==============================================================



// ==============================================================
// Shared Helper
// ==============================================================




// ==============================================================
// Main : Parser && Helper
// ==============================================================
bool	strIsAllDigits(const std::string &str)
{
	size_t	i = 0;
	while (i < str.size())
	{
		char	c = str[i];
		if (c < '0' || c > '9')
		{
			std::cerr << "Error" << std::endl;
			return (false);
		}
		i++;
	}
	return (true);
}

bool	addIntoContainer(const std::string &str, std::vector<unsigned int> &out)
{
	unsigned long	value = 0;
	std::istringstream	stream(str);
	stream >> value;
	if (!stream || !stream.eof()) // checks for failbits/badbits and eof
	{
		std::cerr << "Error" << std::endl;
		return (false);
	}
	if (value == 0)
	{
		std::cerr << "Error" << std::endl;
		return (false);
	}
	if (value > static_cast<unsigned long>(std::numeric_limits<unsigned int>::max()))
	{
		std::cerr << "Error" << std::endl;
		return (false);
	}
	out.push_back(static_cast<unsigned int>(value));
}

bool	parseArgs(int ac, char **av, std::vector<unsigned int> &out)
{
	out.clear();
	int	i = 1;
	while (i < ac)
	{
		std::string	str = argv[i];
		if (str.empty() == true)
		{
			std::cerr << "Error" << std::endl;
			return (false);
		}
		if (strIsAllDigits(str) == false)
			return (false);
		if (addIntoContainer(str, out) == false)
			return (false);
		i++;
	}
	if (out.empty() == true)
	{
		std::cerr << "Error" << std::endl;
		return (false);
	}
	return (true);
}
