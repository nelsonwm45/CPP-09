#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <deque>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <cmath> // Required for std::log2 and std::ceil

# ifndef CONVERSION
#  define CONVERSION 0.000001
# endif

// # ifndef CONVERSION
// #  define CONVERSION 1
// # endif

class PmergeMe
{
	public:
		// OCF
		PmergeMe();
		~PmergeMe();
		PmergeMe(const PmergeMe &other);
		PmergeMe &operator=(const PmergeMe &other);

		// Public API
		// Load input once; both containers are stored privately (vector + deque).
		void setInput(const std::vector<unsigned int> &input);
		void sortUsingVector(std::vector<unsigned int> &outSorted);
		void sortUsingDeque(std::deque<unsigned int> &outSorted);
		size_t	getVectorComparisonCount() const;
		size_t	getDequeComparisonCount() const;
		size_t	getNumberSize() const;

				struct PairV
		{
			unsigned int small;
			unsigned int large;
		};

		struct VectorPair
		{
			std::vector<PairV>	pairs;
			bool				hasStraggler;
			unsigned int		straggler;
		};

		struct VectorChain
		{
			std::vector<unsigned int>	mainChain;
			std::vector<unsigned int>	pend;
			std::vector<size_t>			posOfLarge; // max search when inserting pend to main
			std::vector<size_t>			order;
		};

				struct PairD
		{
			unsigned int small;
			unsigned int large;
		};

		struct DequePair
		{
			std::deque<PairD> pairs;
			bool	hasStraggler;
			unsigned int	straggler;
		};

		struct DequeChain
		{
			std::deque<unsigned int>	mainChain;
			std::deque<unsigned int>	pend;
			std::deque<size_t>	posOfLarge; // max search when inserting pend to main
			std::deque<size_t>	order;
		};

	private:
		// Stored containers
		std::vector<unsigned int> _vec;
		std::deque<unsigned int> _deq;
		size_t	_vectorComparisonCount;
		size_t	_dequeComparisonCount;

		// Ford-Johnson (vector)

		void	fjSortVector(std::vector<unsigned int> &data);
		void	fjSortVectorInternal(std::vector<unsigned int> &data);
		void	initialiseVectorPair(VectorPair &vectorPair);

		void	makePairsVector(const std::vector<unsigned int> &data,
								VectorPair &vectorPair);
		PairV	compareNumberInPairVector(const unsigned int &a, const unsigned int &b);

		// Recursive Sorting for large
		void	recursiveSortPairsVector(std::vector<PairV> &pairs);
		
		void	initialiseVectorChain(VectorChain &vectorChain);
		void	buildMainAndPendVector(const std::vector<PairV> &sortedPairs,
									VectorChain &vectorChain);
		void	insertStragglerIntoPendVector(VectorPair &vectorPair, VectorChain &vectorChain);

		void	assignInsertOrderVector(size_t numPairs, std::vector<size_t> &JacobsthalNumber, std::vector<size_t> &insertOrder);
		void	makeJacobsthalNumbersVector(size_t limit, std::vector<size_t> &JacobsthalNumber);
		void	buildJacobsthalOrderVector(size_t numPairs, std::vector<size_t> &insertOrder);
		
		void	insertPendtoMainChainVector(VectorChain &vectorChain);
		size_t	boundedBinarySearchVector(const std::vector<unsigned int> &mainChain,
											unsigned int insertValue,
											size_t leftBound,
											size_t rightBound);
		void	increasePosOfLargeVector(std::vector<size_t> &posOfLarge, size_t insertPos);
		
		void	insertStragglerintoMainChainVector(std::vector<unsigned int> &mainChain,
													VectorPair &vectorPair);
		
		// Ford-Johnson (Deque)
		void	fjSortDeque(std::deque<unsigned int> &data);
		void	fjSortDequeInternal(std::deque<unsigned int> &data);
		void	initialiseDequePair(DequePair &dequePair);

		void	makePairsDeque(const std::deque<unsigned int> &data,
							std::deque<PairD> &pairs,
							bool &hasStraggler,
							unsigned int &straggler);
		PairD	compareNumberInPairDeque(const unsigned int &a, const unsigned int &b);

		// Recursive call
		void	recursiveSortPairsDeque(std::deque<PairD> &pairs);

		void	initialiseDequeChain(DequeChain &dequeChain);
		void	buildMainAndPendDeque(const std::deque<PairD> &pairs,
									std::deque<unsigned int> &mainChain,
									std::deque<unsigned int> &pend,
									std::deque<size_t> &posOfLarge);
		void	insertStragglerIntoPendDeque(DequePair &dequePair, DequeChain &dequeChain);

		void	assignInsertOrderDeque(size_t numPairs, std::deque<size_t> &JacobsthalNumber, std::deque<size_t> &insertOrder);
		void	makeJacobsthalNumbersDeque(size_t limit, std::deque<size_t> &JacobsthalNumber);
		void	buildJacobsthalOrderDeque(size_t numPairs, std::deque<size_t> &insertOrder);

		void	insertPendtoMainChainDeque(DequeChain &dequeChain);
		size_t	boundedBinarySearchDeque(const std::deque<unsigned int> &mainChain,
										unsigned int value,
										size_t leftBound,
										size_t rightBound);
		void	increasePosOfLargeDeque(std::deque<size_t> &posOfLarge, size_t insertPos);
		
		void	insertStragglerintoMainChainDeque(std::deque<unsigned int> &mainChain,
													DequePair &dequePair);

	};

// Main : Parser && Helper
bool	parseArgs(int ac, char **av, std::vector<unsigned int> &out);
bool	strIsAllDigits(const std::string &str);
bool	addIntoContainer(const std::string &str, std::vector<unsigned int> &out);
std::ostream& operator<<(std::ostream& os, const PmergeMe::PairV& pair);
std::ostream& operator<<(std::ostream& os, const PmergeMe::PairD& pair);

// Template
template <typename Container>
void	printLineWithCap(const std::string &label, const Container &c, size_t maxToShow)
{
	std::cout << label;
	size_t	shown = 0;
	typename	Container::const_iterator it = c.begin();
	typename	Container::const_iterator itEnd = c.end();
	while (it != itEnd)
	{
		if (shown == maxToShow)
		{
			std::cout << " [...]";
			break;
		}
		std::cout << *it;
		shown++;
		typename Container::const_iterator itNext = it;
		itNext++;
		if (itNext != itEnd)
			std::cout << " ";
		it = itNext;
	}
	std::cout << std::endl;
}

template <typename Container>
void	printContainerElements(const Container &c)
{
	typename	Container::const_iterator it = c.begin();
	std::cout << "Container has: [";
	while (it != c.end())
	{
		std::cout << *it;
		typename Container::const_iterator itNext = it;
		itNext++;
		if (itNext != c.end())
			std::cout << " ";
		it = itNext;
	}
	std::cout << "]";
	std::cout << std::endl;
}

#endif
