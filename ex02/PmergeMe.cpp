#include "PmergeMe.hpp"
#include <limits>

// ==============================================================
// Ctors & Dtors & Copy Assignment Operator
// ==============================================================
PmergeMe::PmergeMe() :
	_vec(),
	_deq(),
	_vectorComparisonCount(0),
	_dequeComparisonCount(0)
{}

PmergeMe::~PmergeMe()
{}

PmergeMe::PmergeMe(const PmergeMe &other):
	_vec(other._vec),
	_deq(other._deq),
	_vectorComparisonCount(other._vectorComparisonCount),
	_dequeComparisonCount(other._dequeComparisonCount)
{}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	if (this != &other)
	{
		_vec = other._vec;
		_deq = other._deq;
		_vectorComparisonCount = other._vectorComparisonCount;
		_dequeComparisonCount = other._dequeComparisonCount;
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
	while (i < input.size())
	{
		_deq.push_back(input[i]);
		i++;
	}
}

void	PmergeMe::sortUsingVector(std::vector<unsigned int> &outSorted)
{
	outSorted = this->_vec;
	_vectorComparisonCount = 0;
	if (outSorted.size() <= 1)
		return ;
	fjSortVector(outSorted);
}

void	PmergeMe::sortUsingDeque(std::deque<unsigned int> &outSorted)
{
	outSorted = this->_deq;
	_dequeComparisonCount = 0;
	if (outSorted.size() <= 1)
		return ;
	fjSortDeque(outSorted);
}

size_t	PmergeMe::getVectorComparisonCount() const
{
	return (_vectorComparisonCount);
}

size_t	PmergeMe::getDequeComparisonCount() const
{
	return (_dequeComparisonCount);
}

size_t	PmergeMe::getNumberSize() const
{
	return (_vec.size());
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
	recursiveSortPairsVector(vectorPair.pairs);
	VectorChain	vectorChain;
	initialiseVectorChain(vectorChain);
	buildMainAndPendVector(vectorPair.pairs, vectorChain);
	buildJacobsthalOrderVector(vectorChain.pend.size(), vectorChain.order);
	insertPendtoMainChainVector(vectorChain);
	insertStragglerintoMainChainVector(vectorChain.mainChain, vectorPair);
	data.assign(vectorChain.mainChain.begin(), vectorChain.mainChain.end());
	this->_vec = data;
}

// Debug Version
// void PmergeMe::fjSortVector(std::vector<unsigned int> &data)
// {
// 	VectorPair vectorPair;
// 	initialiseVectorPair(vectorPair);

// 	size_t beforePairing = _vectorComparisonCount;
// 	makePairsVector(data, vectorPair);
// 	std::cout << "Pairing: " << (_vectorComparisonCount - beforePairing) << std::endl;

// 	size_t beforeMerge = _vectorComparisonCount;
// 	recursiveSortPairsVector(vectorPair.pairs);
// 	std::cout << "Merge: " << (_vectorComparisonCount - beforeMerge) << std::endl;

// 	VectorChain vectorChain;
// 	initialiseVectorChain(vectorChain);
// 	buildMainAndPendVector(vectorPair.pairs, vectorChain);
// 	buildJacobsthalOrderVector(vectorChain.pend.size(), vectorChain.order);

// 	size_t beforeInsertion = _vectorComparisonCount;
// 	insertPendtoMainChainVector(vectorChain);
// 	std::cout << "Insertion: " << (_vectorComparisonCount - beforeInsertion) << std::endl;

// 	size_t beforeStraggler = _vectorComparisonCount;
// 	insertStragglerintoMainChainVector(vectorChain.mainChain, vectorPair);
// 	std::cout << "Straggler: " << (_vectorComparisonCount - beforeStraggler) << std::endl;

// 	data.assign(vectorChain.mainChain.begin(), vectorChain.mainChain.end());
// 	this->_vec = data;
// }

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
		PairV	newPairs = compareNumberInPairVector(a, b);
		vectorPair.pairs.push_back(newPairs);
		i += 2;
	}
	if (i < data.size()) // means has straggler
	{
		vectorPair.hasStraggler = true;
		vectorPair.straggler = data[i];
	}
}

PmergeMe::PairV	PmergeMe::compareNumberInPairVector(const unsigned int &a, const unsigned int &b)
{
	this->_vectorComparisonCount++;
	PairV	temp;
	if (a <= b)
	{
		temp.small = a;
		temp.large = b;
	}
	else
	{
		temp.small = b;
		temp.large = a;
	}
	return (temp);
}

void PmergeMe::recursiveSortPairsVector(std::vector<PairV> &pairs)
{
	if (pairs.size() <= 1)
		return;

	// Split into two halves
	size_t mid = pairs.size() / 2;
	std::vector<PairV> left(pairs.begin(), pairs.begin() + mid);
	std::vector<PairV> right(pairs.begin() + mid, pairs.end());

	// Recursively sort both halves
	recursiveSortPairsVector(left);
	recursiveSortPairsVector(right);

	// Merge the sorted halves
	mergePairsVector(left, right, pairs);
}


void PmergeMe::mergePairsVector(const std::vector<PairV> &left, 
								const std::vector<PairV> &right,
								std::vector<PairV> &result)
{
	result.clear();
	size_t i = 0, j = 0;

	while (i < left.size() && j < right.size())
	{
		_vectorComparisonCount++;
		if (left[i].large < right[j].large)
		{
			result.push_back(left[i]);
			i++;
		}
		else
		{
			result.push_back(right[j]);
			j++;
		}
	}
	while (i < left.size())
	{
		result.push_back(left[i]);
		i++;
	}
	while (j < right.size())
	{
		result.push_back(right[j]);
		j++;
	}
}

/*
	A function to determine which pair come earlier : left/right side
	std::sort will determine swapping/keep according to its algorithm
*/

// bool	PmergeMe::pairVLessByLarge(const PairV &lhs, const PairV &rhs)
// {
// 	if (lhs.large < rhs.large)
// 		return (true);
// 	else
// 		return (false);
// }

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
	if (sortedPairs.empty())
		return;

	// First, insert b₁ (first small) into main chain
	vectorChain.mainChain.push_back(sortedPairs[0].small);

	// Then insert all large elements
	size_t i = 0;
	while (i < sortedPairs.size())
	{
		vectorChain.mainChain.push_back(sortedPairs[i].large);
		i++;
	}

	// Build pend array (b₂, b₃, b₄, ... - skipping b₁ since it's already in main)
	i = 1;  // Start from index 1, skip the first pair's small element
	while (i < sortedPairs.size())
	{
		vectorChain.pend.push_back(sortedPairs[i].small);
		// Position of corresponding large element in main chain
		// +1 because b₁ is at position 0
		vectorChain.posOfLarge.push_back(i + 1);
		i++;
	}
}

/*
	Inserting Pend value to Main Chain
	rightBound indicate the position of max value to be compared in Main Chain
*/
void	PmergeMe::insertPendtoMainChainVector(VectorChain &vectorChain)
{
	size_t	i = 0;
	while (i < vectorChain.order.size())
	{
		size_t	insertIndex = vectorChain.order[i];
		unsigned int value = vectorChain.pend[insertIndex];
		size_t	rightBound = vectorChain.posOfLarge[insertIndex];
		size_t	insertPos = boundedBinarySearchVector(vectorChain.mainChain, value, 0, rightBound);
		vectorChain.mainChain.insert(vectorChain.mainChain.begin() + static_cast<std::ptrdiff_t>(insertPos), value);
		increasePosOfLargeVector(vectorChain.posOfLarge, insertPos);
		i++;
	}
}

/*
	Bounded Binary Search
	determine the midpoint of window, mid = L + (R -L)/2, e.g. : window = {3,  8, 10, 14, 18, 25, 32, 35, 49}, mid = 18
	if insert number, x > mid; then go right window, x < mid, go left window
		e.g. : x = 2; x < 18(mid); go left window; left window = { 3, 8, 10, 14}
	repeat again until the window has no more items.

	Imagine it's trying to shrink the window of searching by adjusting left and right bound until
	left == right
*/
size_t	PmergeMe::boundedBinarySearchVector(const std::vector<unsigned int> &mainChain,
											unsigned int insertValue,
											size_t leftBound,
											size_t rightBound)
{
	size_t	left = leftBound;
	size_t	right = rightBound;
	size_t	insertPos = 0;
	while (left < right)
	{
		size_t	midpoint = left + (right - left) / 2;
		this->_vectorComparisonCount++;
		if (insertValue <= mainChain[midpoint])
			right = midpoint;
		else
			left = midpoint + 1;
	}
	insertPos = left;
	return (insertPos);
}

void	PmergeMe::increasePosOfLargeVector(std::vector<size_t> &posOfLarge, size_t insertPos)
{
	size_t	i = 0;
	while (i < posOfLarge.size())
	{
		if (posOfLarge[i] >= insertPos)
			posOfLarge[i]++;
		i++;
	}
}

void	PmergeMe::insertStragglerintoMainChainVector(std::vector<unsigned int> &mainChain,
													VectorPair &vectorPair)
{
	bool	hasStraggler = vectorPair.hasStraggler;
	unsigned int	straggler = vectorPair.straggler;
	if (hasStraggler == true)
	{
		size_t	insertPos = boundedBinarySearchVector(mainChain, straggler, 0, mainChain.size());
		mainChain.insert(mainChain.begin() + static_cast<std::ptrdiff_t>(insertPos), straggler);
	}
}

// ==============================================================
// Ford-Johnson (Deque)
// ==============================================================

/*
	Straggler -> leftover single element
*/
void	PmergeMe::fjSortDeque(std::deque<unsigned int> &data)
{
	DequePair	dequePair;
	initialiseDequePair(dequePair);
	makePairsDeque(data, dequePair.pairs, dequePair.hasStraggler, dequePair.straggler);
	recursiveSortPairsDeque(dequePair.pairs);
	DequeChain	dequeChain;
	initialiseDequeChain(dequeChain);
	buildMainAndPendDeque(dequePair.pairs, dequeChain.mainChain, dequeChain.pend, dequeChain.posOfLarge);
	buildJacobsthalOrderDeque(dequeChain.pend.size(), dequeChain.order);
	insertPendtoMainChainDeque(dequeChain);
	insertStragglerintoMainChainDeque(dequeChain.mainChain, dequePair);
	data.assign(dequeChain.mainChain.begin(), dequeChain.mainChain.end());
	this->_deq = data;
}

void	PmergeMe::initialiseDequePair(DequePair &dequePair)
{
	dequePair.pairs.clear();
	dequePair.hasStraggler = false;
	dequePair.straggler = 0;
}

void	PmergeMe::makePairsDeque(const std::deque<unsigned int> &data,
							std::deque<PairD> &pairs,
							bool &hasStraggler,
							unsigned int &straggler)
{
	pairs.clear();
	hasStraggler = false;
	straggler = 0;
	size_t	i = 0;
	while (i + 1 < data.size()) // because we access two elements at a time
	{
		unsigned int	a = data[i];
		unsigned int	b = data[i + 1];
		PairD	newPairs = compareNumberInPairDeque(a, b);
		pairs.push_back(newPairs);
		i += 2;
	}
	if (i < data.size()) // means has straggler
	{
		hasStraggler = true;
		straggler = data[i];
	}
}

PmergeMe::PairD	PmergeMe::compareNumberInPairDeque(const unsigned int &a, const unsigned int &b)
{
	this->_dequeComparisonCount++;
	PairD	temp;
	if (a <= b)
	{
		temp.small = a;
		temp.large = b;
	}
	else
	{
		temp.small = b;
		temp.large = a;
	}
	return (temp);
}

void PmergeMe::recursiveSortPairsDeque(std::deque<PairD> &pairs)
{
	if (pairs.size() <= 1)
		return;

	// Split into two halves
	size_t mid = pairs.size() / 2;
	std::deque<PairD> left(pairs.begin(), pairs.begin() + mid);
	std::deque<PairD> right(pairs.begin() + mid, pairs.end());

	// Recursively sort both halves
	recursiveSortPairsDeque(left);
	recursiveSortPairsDeque(right);

	// Merge the sorted halves
	mergePairsDeque(left, right, pairs);
}

void PmergeMe::mergePairsDeque(const std::deque<PairD> &left, 
								const std::deque<PairD> &right,
								std::deque<PairD> &result)
{
	result.clear();
	size_t i = 0, j = 0;

	while (i < left.size() && j < right.size())
	{
		_dequeComparisonCount++;
		if (left[i].large < right[j].large)
		{
			result.push_back(left[i]);
			i++;
		}
		else
		{
			result.push_back(right[j]);
			j++;
		}
	}

	while (i < left.size())
	{
		result.push_back(left[i]);
		i++;
	}

	while (j < right.size())
	{
		result.push_back(right[j]);
		j++;
	}
}

/*
	A function to determine which pair come earlier : left/right side
	std::sort will determine swapping/keep according to its algorithm
*/

// bool	PmergeMe::pairDLessByLarge(const PairD &lhs, const PairD &rhs)
// {
// 	if (lhs.large < rhs.large)
// 		return (true);
// 	else
// 		return (false);
// }

void	PmergeMe::initialiseDequeChain(DequeChain &dequeChain)
{
	dequeChain.mainChain.clear();
	dequeChain.pend.clear();
	dequeChain.posOfLarge.clear();
	dequeChain.order.clear();
}

void	PmergeMe::buildMainAndPendDeque(const std::deque<PairD> &sortedPairs,
										std::deque<unsigned int> &mainChain,
										std::deque<unsigned int> &pend,
										std::deque<size_t> &posOfLarge)
{
	if (sortedPairs.empty())
		return;

	// First, insert b₁ (first small) into main chain
	mainChain.push_back(sortedPairs[0].small);

	// Then insert all large elements
	size_t i = 0;
	while (i < sortedPairs.size())
	{
		mainChain.push_back(sortedPairs[i].large);
		i++;
	}

	// Build pend array (b₂, b₃, b₄, ... - skipping b₁)
	i = 1;
	while (i < sortedPairs.size())
	{
		pend.push_back(sortedPairs[i].small);
		// +1 because b₁ is at position 0
		posOfLarge.push_back(i + 1);
		i++;
	}
}

/*
	Inserting Pend value to Main Chain
	rightBound indicate the position of max value to be compared in Main Chain
*/
void	PmergeMe::insertPendtoMainChainDeque(DequeChain &dequeChain)
{
	size_t	i = 0;
	while (i < dequeChain.order.size())
	{
		size_t	insertIndex = dequeChain.order[i];
		unsigned int value = dequeChain.pend[insertIndex];
		size_t	rightBound = dequeChain.posOfLarge[insertIndex];
		size_t	insertPos = boundedBinarySearchDeque(dequeChain.mainChain, value, 0, rightBound);
		dequeChain.mainChain.insert(dequeChain.mainChain.begin() + static_cast<std::ptrdiff_t>(insertPos), value);
		increasePosOfLargeDeque(dequeChain.posOfLarge, insertPos);
		i++;
	}
}

/*
	Bounded Binary Search
	determine the midpoint of window, mid = L + (R -L)/2, e.g. : window = {3,  8, 10, 14, 18, 25, 32, 35, 49}, mid = 18
	if insert number, x > mid; then go right window, x < mid, go left window
		e.g. : x = 2; x < 18(mid); go left window; left window = { 3, 8, 10, 14}
	repeat again until the window has no more items.

	Imagine it's trying to shrink the window of searching by adjusting left and right bound until
	left == right
*/
size_t	PmergeMe::boundedBinarySearchDeque(const std::deque<unsigned int> &mainChain,
											unsigned int insertValue,
											size_t leftBound,
											size_t rightBound)
{
	size_t	left = leftBound;
	size_t	right = rightBound;
	size_t	insertPos = 0;
	while (left < right)
	{
		size_t	midpoint = left + (right - left) / 2;
		this->_dequeComparisonCount++;
		if (insertValue <= mainChain[midpoint])
			right = midpoint;
		else
			left = midpoint + 1;
	}
	insertPos = left;
	return (insertPos);
}

void	PmergeMe::increasePosOfLargeDeque(std::deque<size_t> &posOfLarge, size_t insertPos)
{
	size_t	i = 0;
	while (i < posOfLarge.size())
	{
		if (posOfLarge[i] >= insertPos)
			posOfLarge[i]++;
		i++;
	}
}

void	PmergeMe::insertStragglerintoMainChainDeque(std::deque<unsigned int> &mainChain,
													DequePair &dequePair)
{
	bool	hasStraggler = dequePair.hasStraggler;
	unsigned int	straggler = dequePair.straggler;
	if (hasStraggler == true)
	{
		size_t	insertPos = boundedBinarySearchDeque(mainChain, straggler, 0, mainChain.size());
		mainChain.insert(mainChain.begin() + static_cast<std::ptrdiff_t>(insertPos), straggler);
	}
}

// ==============================================================
// Helper (Vector)
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
void	PmergeMe::buildJacobsthalOrderVector(size_t numPairs, std::vector<size_t> &insertOrder)
{
	if (numPairs == 0)
		return ;
	std::vector<size_t>	JacobsthalNumber;
	makeJacobsthalNumbersVector(numPairs, JacobsthalNumber);
	assignInsertOrderVector(numPairs, JacobsthalNumber, insertOrder);
}

/**
 * Double check
 */
void	PmergeMe::assignInsertOrderVector(size_t numPairs, std::vector<size_t> &JacobsthalNumber, std::vector<size_t> &insertOrder)
{
	if (numPairs == 0)
		return;
	size_t lastCovered = 0;  // Start from 0 since we're now working with adjusted indices
	for (size_t k = 1; k < JacobsthalNumber.size() && lastCovered < numPairs; k++)
	{
		size_t start = JacobsthalNumber[k - 1];
		size_t end = JacobsthalNumber[k];
		if (end > numPairs)
			end = numPairs;
		if (start >= end)
			continue;
		
		// Insert in descending order from end to start (exclusive)
		for (size_t i = end; i > start; i--)
			insertOrder.push_back(i - 1);
		
		lastCovered = end;
	}
	// Handle any remaining elements
	for (size_t i = numPairs; i > lastCovered; i--)
		insertOrder.push_back(i - 1);
}

/*
	Jacobsthal Sequence Formula
	J0 = 0 , J1 = 1
	Jn = 2 * (Jn-2) + (Jn-1)
	Sequence: 0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341,...
*/
void	PmergeMe::makeJacobsthalNumbersVector(size_t limit, std::vector<size_t> &JacobsthalNumber)
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
// Helper (Deque)
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
void	PmergeMe::buildJacobsthalOrderDeque(size_t numPairs, std::deque<size_t> &insertOrder)
{
	if (numPairs == 0)
		return ;
	std::deque<size_t>	JacobsthalNumber;
	makeJacobsthalNumbersDeque(numPairs, JacobsthalNumber);
	assignInsertOrderDeque(numPairs, JacobsthalNumber, insertOrder);
}

void	PmergeMe::assignInsertOrderDeque(size_t numPairs, std::deque<size_t> &JacobsthalNumber, std::deque<size_t> &insertOrder)
{
	if (numPairs == 0)
		return;

	size_t lastCovered = 0;

	for (size_t k = 1; k < JacobsthalNumber.size() && lastCovered < numPairs; k++)
	{
		size_t start = JacobsthalNumber[k - 1];
		size_t end = JacobsthalNumber[k];
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
}

/*
	Jacobsthal Sequence Formula
	J0 = 0 , J1 = 1
	Jn = 2 * (Jn-2) + (Jn-1)
	Sequence: 0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341,...
*/
void	PmergeMe::makeJacobsthalNumbersDeque(size_t limit, std::deque<size_t> &JacobsthalNumber)
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
	return (true);
}

bool	parseArgs(int ac, char **av, std::vector<unsigned int> &out)
{
	out.clear();
	int	i = 1;
	while (i < ac)
	{
		std::string	str = av[i];
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
