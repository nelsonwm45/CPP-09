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
	_deq.clear();
	_vec.clear();
	this->_vec = input;
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

/*
	Straggler -> leftover single element
*/
void	PmergeMe::fjSortVector(std::vector<unsigned int> &data)
{
	VectorPair	vectorPair;
	initialiseVectorPair(vectorPair);
	makePairsVector(data, vectorPair);
	sortPairsByLargeVector(vectorPair.pairs);
	VectorChain	vectorChain;
	initialiseVectorChain(vectorChain);
	buildMainAndPendVector(vectorPair.pairs, vectorChain);
	buildJacobsthalOrder(vectorChain.pend.size(), vectorChain.order);

}

void	PmergeMe::initialiseVectorPair(VectorPair &vectorPair)
{
	vectorPair.pairs.clear();
	vectorPair.hasStraggler = false;
	vectorPair.straggler = 0;
}

void	PmergeMe::makePairsVector(const std::vector<unsigned int> &data,
							VectorPair &vectorPair)
{
	size_t	i = 0;
	while (i + 1 < data.size()) // because we access two elements at a time
	{
		unsigned int	a = data[i];
		unsigned int	b = data[i + 1];
		PairV	newPairs = compareNumberInPair(a, b);
		vectorPair.pairs.push_back(newPairs);
		i += 2;
	}
	if (i < data.size()) // means has straggler
	{
		vectorPair.hasStraggler = true;
		vectorPair.straggler = data[i];
	}
}

PairV	PmergeMe::compareNumberInPair(const unsigned int &a, const unsigned int &b)
{
	PairV	temp;
	if (a <= b)
	{
		temp.small = a;
		temp.large = b;
	}
	else
	{
		temp.small = a;
		temp.large = b;
	}
	return (temp);
}

/*
	std::sort is a function template.
	One of its overloads takes comparison predicate 
		---> bool comp(const T& a, const T& b);
*/
void	PmergeMe::sortPairsByLargeVector(std::vector<PairV> &pairs)
{
	std::sort(pairs.begin(), pairs.end(), PmergeMe::pairVLessByLarge);
}

/*
	A function to determine which pair come earlier : left/right side
	std::sort will determine swapping/keep according to its algorithm
*/

bool	PmergeMe::pairVLessByLarge(const PairV &lhs, const PairV &rhs)
{
	if (lhs.large < rhs.large)
		return (true);
	else
		return (false);
}

void	PmergeMe::initialiseVectorChain(VectorChain &vectorChain)
{
	vectorChain.mainChain.clear();
	vectorChain.pend.clear();
	vectorChain.posOfLarge.clear();
	vectorChain.order.clear();
}

void	PmergeMe::buildMainAndPendVector(const std::vector<PairV> &sortedPairs,
										VectorChain &vectorChain)
{
	size_t	i = 0;
	while (i < sortedPairs.size())
	{
		vectorChain.mainChain.push_back(sortedPairs[i].large);
		vectorChain.pend.push_back(sortedPairs[i].small);
		vectorChain.posOfLarge.push_back(i);
		i++;
	}
}


// ==============================================================
// Ford-Johnson (Deque)
// ==============================================================



// ==============================================================
// Helper
// ==============================================================

/*
	nPairs = 7, J = {0,1,3,5,11,...}
	Window (1,3] → indices 2,1 (push 1,0? careful: we push i-1,
		so 2→1, 1→0 but start is excluded → we actually push 1 then 0… 
		however we later also push a final 0, 
		so in practice we clamp at start=1 and skip i==1? 
		Then pushes i-1 while i>start, so it yields 2→1, i=1 stops → good: 1 only.)
	Window (3,5] → push 4,3 → indices 3,2
	Tail (5,7] → push 7,6,5 → indices 6,5,4
	Finally push 0
*/
void	PmergeMe::buildJacobsthalOrder(size_t numPairs, std::vector<size_t> &insertOrder)
{
	if (numPairs == 0)
		return ;
	if (numPairs == 1)
	{
		insertOrder.push_back(0);
		return ;
	}
	std::vector<size_t>	JacobsthalNumber;
	makeJacobsthalNumbers(numPairs, JacobsthalNumber);
	assignInsertOrder(numPairs, JacobsthalNumber, insertOrder);
}

/**
 * Double check
 */
void	PmergeMe::assignInsertOrder(size_t numPairs, std::vector<size_t> &JacobsthalNumber, std::vector<size_t> &insertOrder)
{
	insertOrder.reserve(insertOrder.size() + numPairs);
	size_t	lastCovered = 1;
	for (size_t k = 2; k < JacobsthalNumber.size() && lastCovered < numPairs; k++)
	{
		size_t	start = JacobsthalNumber[k - 1];
		size_t	end = JacobsthalNumber[k];
		if (end > numPairs)
			end = numPairs;
		if (start >= end)
			continue;
		for (size_t i = end; i > start; i--)
			insertOrder.push_back(i - 1);
		lastCovered = end;
	}
	for (size_t i = numPairs; i > lastCovered; i--)
		insertOrder.push_back(i - 1);
	insertOrder.push_back(0);
}

/*
	Jacobsthal Sequence Formula
	J0 = 0 , J1 = 1
	Jn = 2 * (Jn-2) + (Jn-1)
	Sequence: 0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341,...
*/
void	PmergeMe::makeJacobsthalNumbers(size_t limit, std::vector<size_t> &JacobsthalNumber)
{
	JacobsthalNumber.clear();
	JacobsthalNumber.push_back(0);
	if (limit >= 1)
		JacobsthalNumber.push_back(1);
	while (1)
	{
		if (JacobsthalNumber.size() < 2)
			break;
		size_t	a = JacobsthalNumber[JacobsthalNumber.size() - 1];
		size_t	b = JacobsthalNumber[JacobsthalNumber.size() - 2];
		size_t	nextValue = a + 2 * b;
		JacobsthalNumber.push_back(nextValue);
		if (nextValue >= limit)
			break;
	}
}

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
