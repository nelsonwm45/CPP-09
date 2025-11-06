#include "PmergeMe.hpp"
#include <algorithm> // std::sort
#include <limits>

template <typename PairType>
struct PmergeMe::PairIndexSorter
{
	struct IndexPair
	{
		size_t	small;
		size_t	large;
	};

	struct Chain
	{
		std::vector<size_t>	mainChain;
		std::vector<size_t>	pend;
		std::vector<size_t>	posOfLarge;
		std::vector<size_t>	order;
	};

	static void	sort(PmergeMe &owner,
						const std::vector<PairType> &pairData,
						std::vector<size_t> &indices,
						size_t &comparisonCounter)
	{
		if (indices.size() <= 1)
			return ;

		std::vector<IndexPair>	pairs;
		bool					hasStraggler = false;
		size_t					straggler = 0;
		makePairs(pairData, indices, pairs, hasStraggler, straggler, comparisonCounter);

		std::vector<size_t>	largeIndices;
		largeIndices.reserve(pairs.size());
		for (size_t i = 0; i < pairs.size(); ++i)
			largeIndices.push_back(pairs[i].large);

		sort(owner, pairData, largeIndices, comparisonCounter);

		std::vector<IndexPair>	sortedPairs;
		reorderPairs(pairs, largeIndices, sortedPairs);

		Chain	chain;
		buildMainAndPend(sortedPairs, chain);
		owner.buildJacobsthalOrderVector(chain.pend.size(), chain.order);
		insertPend(pairData, chain, comparisonCounter, owner);
		if (hasStraggler)
			insertStraggler(pairData, chain.mainChain, straggler, comparisonCounter);
		indices.swap(chain.mainChain);
	}

private:
	static IndexPair	compare(const std::vector<PairType> &pairData,
								size_t idxA,
								size_t idxB,
								size_t &comparisonCounter)
	{
		comparisonCounter++;
		if (pairData[idxA].large <= pairData[idxB].large)
		{
			IndexPair	result = { idxA, idxB };
			return (result);
		}
		IndexPair	result = { idxB, idxA };
		return (result);
	}

	static void	makePairs(const std::vector<PairType> &pairData,
							const std::vector<size_t> &data,
							std::vector<IndexPair> &outPairs,
							bool &hasStraggler,
							size_t &straggler,
							size_t &comparisonCounter)
	{
		outPairs.clear();
		hasStraggler = false;
		straggler = 0;
		size_t	i = 0;
		while (i + 1 < data.size())
		{
			IndexPair	pair = compare(pairData, data[i], data[i + 1], comparisonCounter);
			outPairs.push_back(pair);
			i += 2;
		}
		if (i < data.size())
		{
			hasStraggler = true;
			straggler = data[i];
		}
	}

	static void	reorderPairs(const std::vector<IndexPair> &originalPairs,
							const std::vector<size_t> &sortedLarge,
							std::vector<IndexPair> &out)
	{
		out.clear();
		out.reserve(sortedLarge.size());
		for (size_t i = 0; i < sortedLarge.size(); ++i)
		{
			size_t	target = sortedLarge[i];
			for (size_t j = 0; j < originalPairs.size(); ++j)
			{
				if (originalPairs[j].large == target)
				{
					out.push_back(originalPairs[j]);
					break;
				}
			}
		}
	}

	static void	buildMainAndPend(const std::vector<IndexPair> &sortedPairs,
								Chain &chain)
	{
		chain.mainChain.clear();
		chain.pend.clear();
		chain.posOfLarge.clear();
		chain.order.clear();
		for (size_t i = 0; i < sortedPairs.size(); ++i)
		{
			chain.mainChain.push_back(sortedPairs[i].large);
			chain.pend.push_back(sortedPairs[i].small);
			chain.posOfLarge.push_back(i);
		}
	}

	static size_t	binarySearch(const std::vector<PairType> &pairData,
								const std::vector<size_t> &mainChain,
								size_t valueIndex,
								size_t leftBound,
								size_t rightBound,
								size_t &comparisonCounter)
	{
		size_t	left = leftBound;
		size_t	right = rightBound;
		if (left == right)
			return (left);
		while (left + 1 < right)
		{
			size_t	midpoint = left + (right - left) / 2;
			size_t	midIndex = mainChain[midpoint];
			comparisonCounter++;
			if (pairData[valueIndex].large <= pairData[midIndex].large)
				right = midpoint;
			else
				left = midpoint + 1;
		}
		comparisonCounter++;
		if (pairData[valueIndex].large <= pairData[mainChain[left]].large)
			return (left);
		return (right);
	}

	static void	insertPend(const std::vector<PairType> &pairData,
							Chain &chain,
							size_t &comparisonCounter,
							PmergeMe &owner)
	{
		for (size_t i = 0; i < chain.order.size(); ++i)
		{
			size_t	insertIndex = chain.order[i];
			size_t	valueIndex = chain.pend[insertIndex];
			size_t	rightBound = chain.posOfLarge[insertIndex];
			size_t	insertPos = binarySearch(pairData, chain.mainChain, valueIndex, 0, rightBound, comparisonCounter);
			chain.mainChain.insert(chain.mainChain.begin() + static_cast<std::ptrdiff_t>(insertPos), valueIndex);
			owner.increasePosOfLargeVector(chain.posOfLarge, insertPos);
		}
	}

	static void	insertStraggler(const std::vector<PairType> &pairData,
								std::vector<size_t> &mainChain,
								size_t stragglerIndex,
								size_t &comparisonCounter)
	{
		size_t	insertPos = binarySearch(pairData, mainChain, stragglerIndex, 0, mainChain.size(), comparisonCounter);
		mainChain.insert(mainChain.begin() + static_cast<std::ptrdiff_t>(insertPos), stragglerIndex);
	}
};
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
	buildJacobsthalOrderVector(vectorChain.pend.size(), vectorChain.order);
	insertPendtoMainChainVector(vectorChain);
	insertStragglerintoMainChainVector(vectorChain.mainChain, vectorPair);
	data.assign(vectorChain.mainChain.begin(), vectorChain.mainChain.end());
	this->_vec = data;
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
	_vectorComparisonCount++;
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

void	PmergeMe::sortPairsByLargeVector(std::vector<PairV> &pairs)
{
	if (pairs.size() <= 1)
		return ;
	std::vector<PairV>	original(pairs);
	std::vector<size_t>	indices(pairs.size());
	for (size_t i = 0; i < indices.size(); ++i)
		indices[i] = i;
	size_t	beforeCount = _vectorComparisonCount;
	PairIndexSorter<PairV>::sort(*this, original, indices, _vectorComparisonCount);
	size_t	afterCount = _vectorComparisonCount;
	std::cerr << "Pair sort comparisons: " << (afterCount - beforeCount) << std::endl;
	for (size_t i = 0; i < indices.size(); ++i)
		pairs[i] = original[indices[i]];
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

/*
	Inserting Pend value to Main Chain
	rightBound indicate the position of max value to be compared in Main Chain
*/
void	PmergeMe::insertPendtoMainChainVector(VectorChain &vectorChain)
{
	size_t	i = 0;
	size_t	startCount = _vectorComparisonCount;
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
	std::cerr << "Pend insert comparisons: " << (_vectorComparisonCount - startCount) << std::endl;
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
	if (left == right)
		return (left);
	while (left + 1 < right)
	{
		size_t	midpoint = left + (right - left) / 2;
		_vectorComparisonCount++;
		if (insertValue <= mainChain[midpoint])
			right = midpoint;
		else
			left = midpoint + 1;
	}
	_vectorComparisonCount++;
	if (insertValue <= mainChain[left])
		return (left);
	return (right);
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
		size_t	startCount = _vectorComparisonCount;
		size_t	insertPos = boundedBinarySearchVector(mainChain, straggler, 0, mainChain.size());
		mainChain.insert(mainChain.begin() + static_cast<std::ptrdiff_t>(insertPos), straggler);
		std::cerr << "Straggler insert comparisons: " << (_vectorComparisonCount - startCount) << std::endl;
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
	sortPairsByLargeDeque(dequePair.pairs);
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
	_dequeComparisonCount++;
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

/*
	std::sort is a function template.
	One of its overloads takes comparison predicate 
		---> bool comp(const T& a, const T& b);
*/
void	PmergeMe::sortPairsByLargeDeque(std::deque<PairD> &pairs)
{
	if (pairs.size() <= 1)
		return ;
	std::vector<PairD>	original(pairs.begin(), pairs.end());
	std::vector<size_t>	indices(pairs.size());
	for (size_t i = 0; i < indices.size(); ++i)
		indices[i] = i;
	PairIndexSorter<PairD>::sort(*this, original, indices, _dequeComparisonCount);
	std::deque<PairD>	sorted;
	for (size_t i = 0; i < indices.size(); ++i)
		sorted.push_back(original[indices[i]]);
	pairs.swap(sorted);
}

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
	mainChain.clear();
	pend.clear();
	posOfLarge.clear();
	size_t	i = 0;
	while (i < sortedPairs.size())
	{
		mainChain.push_back(sortedPairs[i].large);
		pend.push_back(sortedPairs[i].small);
		posOfLarge.push_back(i);
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
		_dequeComparisonCount++;
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
	if (numPairs == 1)
	{
		insertOrder.push_back(0);
		return ;
	}
	std::vector<size_t>	JacobsthalNumber;
	makeJacobsthalNumbersVector(numPairs, JacobsthalNumber);
	assignInsertOrderVector(numPairs, JacobsthalNumber, insertOrder);
}

/**
 * Double check
 */
void	PmergeMe::assignInsertOrderVector(size_t numPairs, std::vector<size_t> &JacobsthalNumber, std::vector<size_t> &insertOrder)
{
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
	if (numPairs == 1)
	{
		insertOrder.push_back(0);
		return ;
	}
	std::deque<size_t>	JacobsthalNumber;
	makeJacobsthalNumbersDeque(numPairs, JacobsthalNumber);
	assignInsertOrderDeque(numPairs, JacobsthalNumber, insertOrder);
}

void	PmergeMe::assignInsertOrderDeque(size_t numPairs, std::deque<size_t> &JacobsthalNumber, std::deque<size_t> &insertOrder)
{
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
