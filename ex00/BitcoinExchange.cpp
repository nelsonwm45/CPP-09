#include "BitcoinExchange.hpp"

// =============================================================================
// Ctors & Dtors & Copy Assignment Operators
// =============================================================================

BitcoinExchange::BitcoinExchange() {}

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

	// Skip Header "date,exchange_rate"
	if (std::getline(file, line))
	{
		if (line.find("date") == std::string::npos)
		{
			// Parse if first line is data
			std::string	date;
			double	price;
			if (parseCSVLine(line, date, price))
				_database[date] = price;
		}
	}

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;
		std::string	date;
		double	price;
		if (parseCSVLine(line, date, price))
			_database[date] = price;
	}
	if (_database.empty())
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
	if (isValidDate(date) == false)
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
// File Parser
// =============================================================================

/*
	Accept int or float text
	Subject requires 0..1000 range checks elsewhere
*/
bool	BitcoinExchange::parseValue(const std::string &valueStr, double &value)
{
	char	*end = 0;
	errno = 0;

	value = std::strtod(valueStr.c_str(), &end);
	if (end == valueStr.c_str())
		return (false);
	if (*end != '\0')
		return (false);
	if (errno == ERANGE)
		return (false);
	return (true);
}

/*
	value : positive integer, between 0 and 1000.
*/
bool	BitcoinExchange::checkValue(const double &value)
{
	if (value < 0.0)
	{
		std::cerr << "Error: not a positive number." << std::endl;
		return (false);
	}

	if (value > 1000.0)
	{
		std::cerr << "Error: too large a number." << std::endl;
		return (false);
	}
	return (true);
}

/*
	std::lower_bound(k) returns an iterator to the first key that is not less than k
	- exact key found : return that key
	- no exact match : return to "FIRST BIGGER KEY"
	- if all keys smaller than k : return end();


	Map keys: { "2021-01-01", "2021-01-05", "2021-01-10" }

	Query "2021-01-05" → lower_bound points to "2021-01-05" (exact) → use it.
	Query "2021-01-07" → lower_bound points to "2021-01-10" (> query) → step back → "2021-01-05".
	Query "2021-01-11" → lower_bound is end() → step back → "2021-01-10".
	Query "2020-12-31" → lower_bound points to "2021-01-01" (the first), not equal → there’s no earlier key → false.
*/
bool	BitcoinExchange::findRateOnOrBefore(const std::string &date, double &rate) const
{
	if (_database.empty())
		return (false);
	std::map<std::string, double>::const_iterator it = _database.lower_bound(date);
	if (it == _database.end()) // when all date in container is lower than query date, get the latest date
		--it;
	else if (it->first != date) // found a key but not exact match
	{
		if (it == _database.begin()) // the query date is even smaller than the container smallest date
			return (false);
		--it; // get the nearest lower key
	}
	rate = it->second;
	return (true);
}

bool	BitcoinExchange::checkAndFetchRate(const std::string &rawLine,
											const std::string &date,
											const std::string &valueStr,
											double &value,
											double &rate)
{
	if (isValidDate(date) == false)
	{
		std::cerr << "Error: bad input => " << rawLine << std::endl;
		return (false);
	}
	if (parseValue(valueStr, value) == false)
	{
		std::cerr << "Error: bad input => " << rawLine << std::endl;
		return (false);
	}
	if (checkValue(value) == false)
		return (false);
	// exchange rate on / before the date
	if (findRateOnOrBefore(date, rate) == false)
	{
		std::cerr << "Error: bad input => " << rawLine << std::endl;
		return (false);
	}
	return (true);
}

/*
	Format double to at most 2 decimals, trimming trailing zeros.

	floatfield(a category, mask) - controls how floating-point numbers are printed, it has two modes
		fixed(a mode, flag) --------> 1234.56
		scientific(a mode, flag) ---> 1.23456e+03
	default float formatting - chooses fixed or scientific automatically based on value and precision
	setf(std::ios::fixed) --> set to fixed
	setf(std::ios::scientific) --> set to scientific
	unsetf(mask) ----> unsetf(std::ios::floatfield) --> clears both fixed & scientific


	OTHER COMMON MASK:
		std::ios::adjustfield → left | right | internal
		std::ios::basefield → dec | hex | oct

	oss.setf(std::ios::fixed)
		Tells the stream to use fixed-point notation (e.g., 123.45), not scientific (1.2345e+02).
		Internally this sets the stream’s floatfield to fixed.
	std::setprecision(2)
		With fixed or scientific set: precision = digits after the decimal point (e.g., 123.456 → 123.46). ---- I choose thissss since I set to "fixed"
		With default floatfield (neither fixed nor scientific): precision = significant digits (e.g., 123.456 → 1.2e+02 or 123 depending on magnitude/implementation).
*/
std::string	BitcoinExchange::formater(double x)
{
	std::ostringstream	oss;
	oss.setf(std::ios::fixed);
	oss << std::setprecision(2) << x; // e.g. "0.90"
	std::string	str = oss.str();

	std::string::size_type	dot = str.find('.');
	if (dot != std::string::npos)
	{
		while (!str.empty() && str[str.size() - 1] == '0')
			str.erase(str.size() - 1); // remove trailing zeros
		if (!str.empty() && str[str.size() - 1] == '.')
			str.erase(str.size() - 1); // remove lone '.'
	}
	return (str);
}

/*
	seekg moves the get pointer (the read cursor) of the input stream.
		f.seekg(0); moves the cursor to byte 0 — the beginning of the file.
		After this call, the next std::getline(f, line) will return the first line again.

	std::cout.precision(2)
		With fixed, precision = number of digits after the decimal (so 2 → xx.yy).
		Without fixed (default floatfield), precision = total significant digits.
*/
void	BitcoinExchange::processInputFile(const std::string &filepath)
{
	std::ifstream	file(filepath.c_str());
	if (!file)
	{
		std::cerr << "Error: could not open file." << std::endl;
		return ;
	}
	std::string	line;

	// Skip Header "date | value"
	if (std::getline(file, line))
	{
		if (isInputFileHeader(line) == false)
			file.seekg(0); // reset the cursor
	}

	while (std::getline(file, line))
	{
		if (line.empty())
			continue ;
		
		// Parse "data | value"
		std::string::size_type	bar = line.find('|');
		if (bar == std::string::npos)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		std::string	date = trim(line.substr(0, bar));
		std::string	valueStr = trim(line.substr(bar + 1));
		
		double	value = 0.0;
		double	rate = 0.0;
		if (checkAndFetchRate(line, date, valueStr, value, rate) == false)
			continue;
		double	result = value * rate;
		std::cout << date << " => " << valueStr << " = " << formater(result) << std::endl;
	}
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
bool	BitcoinExchange::isValidDate(const std::string &str)
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
	int	day = std::atoi(dayStr.c_str());
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

bool	BitcoinExchange::isInputFileHeader(const std::string &line)
{
	std::string	trimmed = trim(line);

	if (trimmed == "date | value")
		return (true);
	else
		return (false);
}
// =============================================================================
// Exception
// =============================================================================
const char	*BitcoinExchange::CouldNotOpenFile::what() const throw ()
{
	return ("Error: could not open file.");
}