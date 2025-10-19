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
// CSV File
// =============================================================================
bool	BitcoinExchange::loadCSVFile(const std::string &filepath)
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

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;
		std::string	date;
		double	price;
		if (parseLine(line, date, price))
			this->_database[date] = price;
	}
	if (this->_database.empty())
		return (false);
	else
		return (true);
}

bool	BitcoinExchange::parseCSVLine(std::string &line, std::string &date, double &price)
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

	// process pricee
	if (parsePrice(price_str, price) == false)
		return (false);
	return (true);
}

bool	BitcoinExchange::parsePrice(std::string &price_str, double &price)
{
	char	*end = 0;
	errno = 0;

	price = std::strtod(price_str.c_str(), &end);
	if (end == price_str.c_str())
		return (false);
	if (*end != '\0')
		return (false);
	if (errno == ERANGE)
		return (false);
	return (true);
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
	// multiple of 4 → maybe leap; check century rule
	if (year % 4 == 0)
	{
		if (year % 100 == 0)
		{
			// century → leap only if divisible by 400
			if (year % 400 == 0)
				return (true); // leap century (e.g., 2000)
			else
				return (false); // common century (e.g., 1900)
		}
		else
			return (true); // not a century → leap (e.g., 2024)
	}
	else
		return (false); // not multiple of 4 → common year

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
	
	int	year = std::atoi(yearStr.c_str());
	int	month = std::atoi(monthStr.c_str());
	int	day = std::atoi(datStr.c_str());
	if (isValidMonth(month) == false || isValidYear(year) == false)
		return (false);
	static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int maxd = mdays[month - 1];
	if (month == 2 && isLeapYear(year))
		maxd = 29;
	if (day >= 1 && day <= maxd)
		return (true);
	else
		return (false);
}

bool	BitcoinExchange::isValidMonth(const int month)
{
	if (month < 1 || month > 12)
		return (false);
	return (true);
}

bool	BitcoinExchange::isValidYear(const int year)
{
	if (year <= 0 || year >= INT_MAX)
		return (false);
	return (true);
}
// =============================================================================
// Exception
// =============================================================================
const char	*BitcoinExchange::FileNotExist::what() const throw ()
{
	return ("[BitcoinExchange]: File Does Not Exist");
}