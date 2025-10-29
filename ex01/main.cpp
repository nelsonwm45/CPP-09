#include "RPN.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error" << std::endl;
		return (1);
	}
	RPN	rpn;
	long	results = 0;
	bool	success = rpn.calculateExpression(av[1], results);
	if (success == false)
	{
		std::cerr << "Error" << std::endl;
		return (1);
	}
	else
	{
		std::cout << results << std::endl;
		return 0;
	}
}