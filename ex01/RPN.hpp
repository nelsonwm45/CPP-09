#ifndef RPN_HPP
# define RPN_HPP

#include <iostream>
#include <string>
#include <stack>
#include <list>
#include <sstream>
#include <climits>  // LONG_MAX, LONG_MIN
#include <cctype> // std::isdigit

/*
	Infix (usual mathematical operation) : 3 + 4 * 2
	Postfix (Reverse Polish Notation):  3 4 2 * +
		Read as : push 3, push 4, push 2, multiply (4*2), then add (3+8)
	
	Use a stack:
		1. Scan tokens left→right.
		2. If you see a number, push it.
		3. If you see an operator (+ - * /), pop the top two numbers b then a, compute a op b, push the result.
		4. At the end, the stack’s single value is the answer.
	
	Example: RPN 5 1 2 + 4 * + 3 -
		Push 5 → [5]
		Push 1 → [5,1]
		Push 2 → [5,1,2]
		+ → pop 2,1 → 1+2=3 → [5,3]
		Push 4 → [5,3,4]
		* → pop 4,3 → 3*4=12 → [5,12]
		+ → pop 12,5 → 5+12=17 → [17]
		Push 3 → [17,3]
		- → pop 3,17 → 17-3=14 → 14
	
	RPN is LIFO, so use stack container(LIFO)
	Shunting-Yard algorithm converts infix → postfix (RPN)
*/

class	RPN
{
	public:
		RPN();
		~RPN();
		RPN(const RPN &other);
		RPN	&operator=(const RPN &other);

		// Token
		enum TokenType
		{
			TOKEN_NUMBER,
			TOKEN_OPERATOR,
			TOKEN_INVALID,
			TOKEN_END
		};

		struct Token
		{
			std::string	text;
			TokenType	type;
		};

		// API
		bool	calculateExpression(const std::string &expression, long &output);

	private:
		std::stack<long, std::list<long> > _stack; // C++98 require a space

		// Tokenization Helper
		bool	readNextToken(std::istringstream &istream, Token &outToken) const;
		void	classifyToken(Token &token) const;
		bool	isOperatorChar(char c) const;
		bool	isSingleDigitNumber(const std::string &str) const;

		// Evaluation Helper
		void	cleanStack();
		bool	handleNumberToken(const Token &token);
		bool	handleOperatorToken(const Token &token);
		bool	popTwoOperands(long &lhs, long &rhs);
		bool	applyOperator(long lhs, long rhs, char op, long &results) const;
		bool	finalizeResults(long &finalOutput);

		// Arithmetic
		bool	safeAdd(long a, long b, long &out) const;
		bool	safeSub(long a, long b, long &out) const;
		bool	safeMul(long a, long b, long &out) const;
		bool	safeDiv(long a, long b, long &out) const;


};

#endif