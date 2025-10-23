#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# define RED "\033[31m"
# define GREEN "\033[32m"
# define BLUE "\033[34m"
# define YELLOW "\033[33m"
# define CYAN "\033[36m"
# define MAGENTA "\033[35m"
# define WHITE "\033[37m"
# define RESET "\033[0m"

# include <iostream>
# include <string> // c_str()
# include <map>
# include <climits> // INT_MAX
# include <sstream>
# include <iomanip>
# include <stdexcept>
# include <fstream> // std::ifstream
# include <stdlib.h> // for strtod
# include <cerrno> // for errno/ERANGE


class	BitcoinExchange
{
	private:
		std::map<std::string, double>	_database;


	public:
		BitcoinExchange();
		~BitcoinExchange();
		BitcoinExchange(const BitcoinExchange &other);
		BitcoinExchange	&operator=(const BitcoinExchange &other);

		// CSV File : aim to get the rate/price
		bool	loadCSVFile(const std::string &filepath);
		bool	parseCSVLine(std::string &line, std::string &date, double &price);
		bool	parsePrice(std::string &price_str, double &price);

		// File Parser
		void	processInputFile(const std::string &filepath);
		bool	checkAndFetchRate(const std::string &rawLine,
								const std::string &date,
								const std::string &valueStr,
								double &value,
								double &rate);
		bool	parseValue(const std::string &valueStr, double &value);
		bool	checkValue(const double &value);
		bool	findRateOnOrBefore(const std::string &date, double &rate) const;
		std::string	formater(double x);

		// Helper
		std::string	trim(const std::string &str);
		bool	isValidDate(const std::string &str);
		bool	isDigits(const std::string &str);
		bool	isLeapYear(int year);
		bool	isValidMonth(const int month);
		bool	isValidYear(const int year);
		bool	isInputFileHeader(const std::string &line);


		// Exception
		class	CouldNotOpenFile: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};

#endif