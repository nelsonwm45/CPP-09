#include "BitcoinExchange.hpp"

// =============================================================================
// Ctors & Dtors & Copy Assignment Operators
// =============================================================================

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const std::string &filepath)
{
	try
	{
		if (loadFile(filepath) == false)
			throw (BitcoinExchange::FileNotExist());
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << "\n";
	}
}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other):
	_database(other._database)
{}

BitcoinExchange	&BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
	{
		this->_database = other._database;
	}
	return (*this);
}

// =============================================================================
// File Management
// =============================================================================
bool	BitcoinExchange::loadFile(const std::string &filepath)
{
	std::ifstream	file(filepath.c_str());
	if (!file)
		return (false);
	std::string	line;

	// Optional, Accept "date,exchange_rate"
	if (std::getline(file, line))
	{
		if (line.find("date") == std::string::npos)
		{
			// Parse if first line is data
			std::string	date;
			double	price;
			if (parseLine(line, date, price))
				this->_database[date] = price;
		}
	}
}


// =============================================================================
// Parser
// =============================================================================
bool	BitcoinExchange::parseLine(std::string &line, std::string &date, double &price)
{
	// Expected: YYYY-MM-DD,price
	std::string::size_type	comma = line.find(',');
	if (comma == std::string::npos)
		return (false);
	std::string	price_str;
	date = line.substr(0, comma);
	price_str = line.substr(comma + 1);
	date = BitcoinExchange::trim(date);
	price_str = BitcoinExchange::trim(price_str);
	if (checkValidDate(date) == false)
		return (false);

	// process pricee TODOOO
}


// =============================================================================
// Helper
// =============================================================================

/*
	trim leading and trailing spaces, tabs, carriage return, newline, and vertical tab (" \t\r\n\v").
*/
std::string	BitcoinExchange::trim(const std::string &str)
{
	size_t	start = str.find_first_not_of(" \t\n\r\f\v");
	size_t	end = str.find_last_not_of(" \t\n\r\f\v");
	std::string	newStr;

	if (start == std::string::npos)
		newStr = "";
	else
		newStr = str.substr(start, end - start + 1);
	return (newStr);
}

/*
	check the whole string, return true if all strings are digits
*/
bool	BitcoinExchange::isDigits(const std::string &str)
{
	if (str.size() <= 0)
		return (false);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return (false);
	}
	return (true);
}

bool	BitcoinExchange::isLeapYear(int year)
{
	if (year % 4 != 0)
		return (false); // not a multiple of 4 means not a leap year
	if (year % 100 != 0)
		return (true); // is a leap year (multiple of 4) but not century
	if (year % 400 == 0)
		return (true);   // divisible by 400 → leap century
	else
		return (false);  // not divisible by 400 → common century

}

/*
	The format must be YYYY-MM-DD
	Month must have two values
	Day also must have two values
*/
bool	BitcoinExchange::checkValidDate(const std::string &str)
{
	if (str.size() != 10 || str[4] != '-' || str[7] != '-')
		return (false);
	
	std::string	yearStr = str.substr(0, 4);
	std::string	monthStr = str.substr(5, 2);
	std::string	dayStr = str.substr(8, 2);
	if (!isDigits(yearStr) || !isDigits(monthStr) || !isDigits(dayStr))
		return (false);
	
	int	y = std::atoi(yearStr.c_str());
	int	m = std::atoi(monthStr.c_str());
	int	d = std::atoi(datStr.c_str());
	if (m < 1 || m > 12)
		return (false);
	static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int maxd = mdays[m - 1];
	if (m == 2 && isLeapYear(y))
		maxd = 29;
	if (day >= 1 && day <= maxd)
		return (true);
	else
		return (false);
}

// =============================================================================
// Exception
// =============================================================================
const char	*BitcoinExchange::FileNotExist::what() const throw ()
{
	return ("[BitcoinExchange]: File Does Not Exist");
}