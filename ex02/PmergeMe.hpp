#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <deque>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>

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

	private:
		// Stored containers
		std::vector<unsigned int> _vec;
		std::deque<unsigned int> _deq;

		// Ford-Johnson (vector)
		struct PairV
		{
			unsigned int small;
			unsigned int large;
		};

		struct VectorPair
		{
			std::vector<PairV> pairs;
			bool	hasStraggler;
			unsigned int	straggler;
		};

		struct VectorChain
		{
			std::vector<unsigned int> mainChain;
			std::vector<unsigned int> pend;
			std::vector<size_t>	posOfLarge; // max search when inserting pend to main
			std::vector<size_t>	order;
		}

		void fjSortVector(std::vector<unsigned int> &data);
		void initialiseVectorPair(VectorPair &vectorPair);

		void makePairsVector(const std::vector<unsigned int> &data,
								VectorPair &vectorPair);
		PairV	compareNumberInPair(const unsigned int &a, const unsigned int &b);

		void sortPairsByLargeVector(std::vector<PairV> &pairs);
		
		void	initialiseVectorChain(VectorChain &vectorChain);
		void buildMainAndPendVector(const std::vector<PairV> &sortedPairs,
									VectorChain &vectorChain);

		size_t boundedBinarySearchVector(const std::vector<unsigned int> &mainChain,
											unsigned int value,
											size_t leftBound,
											size_t rightBound);

		// Ford-Johnson (Deque)
		struct PairD
		{
			unsigned int small;
			unsigned int large;
		};

		void fjSortDeque(std::deque<unsigned int> &data);

		void makePairsDeque(const std::deque<unsigned int> &data,
							std::vector<PairD> &pairs,
							bool &hasStraggler,
							unsigned int &straggler);

		void sortPairsByLargeDeque(std::vector<PairD> &pairs);

		void buildMainAndPendDeque(const std::vector<PairD> &pairs,
									std::deque<unsigned int> &mainChain,
									std::vector<unsigned int> &pend,
									std::vector<size_t> &posOfLarge);

		size_t boundedBinarySearchDeque(const std::deque<unsigned int> &mainChain,
										unsigned int value,
										size_t leftBound,
										size_t rightBound);
		
		// Helper
		void	assignInsertOrder(size_t numPairs, std::vector<size_t> &JacobsthalNumber, std::vector<size_t> &insertOrder);
		void makeJacobsthalNumbers(size_t limit, std::vector<size_t> &JacobsthalNumber);
		void buildJacobsthalOrder(size_t numPairs, std::vector<size_t> &insertOrder);

		static bool pairVLessByLarge(const PairV &lhs, const PairV &rhs);
		static bool pairDLessByLarge(const PairD &lhs, const PairD &rhs);
};

// Main : Parser && Helper
bool	parseArgs(int ac, char **av, std::vector<unsigned int> &out);
bool	strIsAllDigits(const std::string &str);
bool	addIntoContainer(const std::string &str, std::vector<unsigned int> &out);


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


#endif