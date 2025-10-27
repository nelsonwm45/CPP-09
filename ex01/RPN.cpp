#include "RPN.hpp"

// =============================================================================
// Ctors & Dtors & Copy Assignment Operator
// =============================================================================

RPN::RPN()
{}

RPN::~RPN()
{}

RPN::RPN(const RPN &other):
	_stack(other._stack)
{}

RPN	&RPN::operator=(const RPN &other)
{
	if (this != &other)
		this->_stack = other._stack;
	return (*this);
}


// =============================================================================
// Ctors & Dtors & Copy Assignment Operator
// =============================================================================

