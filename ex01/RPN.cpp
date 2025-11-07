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
// Calculation
// =============================================================================

bool	RPN::calculateExpression(const std::string &expression, long &output)
{
	cleanStack();

	std::istringstream	istream(expression);
	while (true)
	{
		Token	token;
		bool	readSuccess = readNextToken(istream, token);
		if (readSuccess == false) // stream errors
			return (false);
		if (token.type == TOKEN_END)
			break;
		classifyToken(token);
		if (token.type == TOKEN_NUMBER)
		{
			bool	numSuccess = handleNumberToken(token);
			if (numSuccess == false)
				return (false);
		}
		else if (token.type == TOKEN_OPERATOR)
		{
			bool	opSuccess = handleOperatorToken(token);
			if (opSuccess == false)
				return (false);
		}
		else // invalid token
			return (false);
	}
	bool	finish = finalizeResults(output);
	if (finish == false)
		return (false);
	return (true);
}

// =============================================================================
// Tokenisation Helper
// =============================================================================

bool	RPN::readNextToken(std::istringstream &istream, Token &outToken) const
{
	outToken.text.clear();
	outToken.type = TOKEN_INVALID; // set to default

	if (istream >> outToken.text)
		return (true);
	else
	{
		// Reached EOF, mark it as end-of-input
		if (istream.eof())
		{
			outToken.type = TOKEN_END;
			return (true);
		}
		else
			return (false); // Other stream error (badbit/failbit) which is not eof
	}
}

void	RPN::classifyToken(Token &token) const
{
	// We only accept single token as stated in subject
	if (token.text.size() == 1)
	{
		unsigned char character = static_cast<unsigned char>(token.text[0]);
		if (std::isdigit(character))
		{
			token.type = TOKEN_NUMBER;
			return ;
		}
		if (isOperatorChar(token.text[0]))
		{
			token.type = TOKEN_OPERATOR;
			return ;
		}
	}
}

bool	RPN::isOperatorChar(char c) const
{
	switch (c)
	{
		case '+':
			return (true);
		case '-':
			return (true);
		case '*':
			return (true);
		case '/':
			return (true);
		default:
			return (false);
	}
}

bool	RPN::isSingleDigitNumber(const std::string &str) const
{
	if (str.size() != 1)
		return (false);
	
	unsigned char	character = static_cast<unsigned char>(str[0]);
	if (std::isdigit(character))
		return (true);
	else
		return (false);
}

// =============================================================================
// Evaluation Helper
// =============================================================================
void	RPN::cleanStack()
{
	while (!_stack.empty())
		_stack.pop();
}

bool	RPN::handleNumberToken(const Token &token)
{
	bool	success = isSingleDigitNumber(token.text);
	if (success == false)
		return (false);
	unsigned char character = static_cast<unsigned char>(token.text[0]);
	long	number = static_cast<long>(character - '0');
	_stack.push(number);
	return (true);
}

bool	RPN::handleOperatorToken(const Token &token)
{
	// We needs two operands
	if (_stack.size() < 2)
		return (false);
	
	long	rhs = 0;
	long	lhs = 0;

	bool	popSuccess = popTwoOperands(lhs, rhs);
	if (popSuccess == false)
		return (false);
	long	results = 0;
	bool	applySuccess = applyOperator(lhs, rhs, token.text[0], results);
	if (applySuccess == false)
		return (false);
	_stack.push(results); // add the results back to the stack
	return (true);
}

/*
	_stack.top() access the "last added element"
	pop the first LAST element and put at right side 
	pop the second LAST element and put at left side

	9 1 /
	9 / 1
*/
bool	RPN::popTwoOperands(long &lhs, long &rhs)
{
	if (_stack.size() < 2)
		return (false);
	rhs = _stack.top();
	_stack.pop();
	lhs = _stack.top();
	_stack.pop();
	return (true);
}

bool	RPN::applyOperator(long lhs, long rhs, char op, long &results) const
{
	if (op == '+')
	{
		bool	ok = safeAdd(lhs, rhs, results);
		if (ok == false)
			return (false);
		return (true);
	}
	else if (op == '-')
	{
		bool	ok = safeSub(lhs, rhs, results);
		if (ok == false)
			return (false);
		return (true);
	}
	else if (op == '*')
	{
		bool	ok = safeMul(lhs, rhs, results);
		if (ok == false)
			return (false);
		return (true);
	}
	else if (op == '/')
	{
		bool	ok = safeDiv(lhs, rhs, results);
		if (ok == false)
			return (false);
		return (true);
	}
	else
		return (false);
}

bool	RPN::finalizeResults(long &finalOutput)
{
	if (_stack.size() != 1)
		return (false);
	finalOutput = _stack.top();
	cleanStack();
	return (true);
}

// =============================================================================
// Safe Arithmetic
// =============================================================================

bool RPN::safeAdd(long a, long b, long &out) const
{
	if (b > 0)
	{
		if (a > LONG_MAX - b)
			return (false);
	}
	else if (b < 0)
	{
		if (a < LONG_MIN - b)
			return (false);
	}
	out = a + b;
	return (true);
}

bool RPN::safeSub(long a, long b, long &out) const
{
	if (b > 0)
	{
		if (a < LONG_MIN + b)
			return (false);
	}
	else if (b < 0)
	{
		if (a > LONG_MAX + (-b))
			return (false);
	}
	out = a - b;
	return (true);
}

bool RPN::safeMul(long a, long b, long &out) const
{
	if (a == 0 || b == 0)
	{
		out = 0;
		return true;
	}
	// We need to ensure |a * b| <= LONG_MAX and respects LONG_MIN.
	if (a > 0)
	{
		if (b > 0)
		{
			if (a > LONG_MAX / b)
				return (false);
		}
		else // b < 0
		{
			// product negative, check against LONG_MIN
			if (b < LONG_MIN / a)
				return (false);
		}
	}
	else // a < 0
	{
		if (b > 0)
		{
			// product negative, check against LONG_MIN
			if (a < LONG_MIN / b)
				return (false);
		}
		else // b < 0, product positive
		{
			if (a < LONG_MAX / b)
				return (false); // Example: a = -X, b = -Y; if a < LONG_MAX / b, overflow
		}
	}
	out = a * b;
	return (true);
}

/*
	Computer write negative number
		Take the positive number’s bits, flip them, then add 1.
	
	Example with 8 bits:
		+5 → 0000 0101
		flip → 1111 1010
		+1 → 1111 1011 ← this pattern means −5
	There’s one more negative number than positive. That’s the key quirk.
	
	What range do 8 bits cover?
		Min: −128 (1000 0000)
		Max: +127 (0111 1111)
	
	Why is there a weird special case?
	Try to negate −128 in 8 bits:
		−128 is 1000 0000
		flip → 0111 1111
		+1 → 1000 0000 (you’re back to −128!)
	!! So +128 can’t be represented in 8 bits. It doesn’t fit. So Apply to Bigger Byte
*/
bool RPN::safeDiv(long a, long b, long &out) const
{
	if (b == 0)
		return (false);
	// Special overflow: LONG_MIN / -1 overflows on 2’s complement
	if (a == LONG_MIN && b == -1)
		return (false);
	out = a / b;
	return (true);
}
