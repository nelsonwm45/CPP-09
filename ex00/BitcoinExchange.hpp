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
# include <map>
# include <sstream>
# include <stdexcept>


class	BitcoinExchange
{
	private:
		std::map<std::string, double>	_database;

		BitcoinExchange();
	public:
		BitcoinExchange(const std::string &filepath)
		~BitcoinExchange();
		BitcoinExchange(const BitcoinExchange &other);
		BitcointExchange	&operator=(const BitcoinExchange &other);

		bool	loadFile(const std::string &filepath);

		bool	parseLine(std::string &line, std::string &date, double &price);

		// Helper
		std::string	trim(const std::string &str);
		bool	checkValidDate(const std::string &str);
		bool	isDigits(const std::string &str);
		bool	isLeapYear(int year);


		// Exception
		class	FileNotExist: public std::exception
		{
			public:
				virtual const char *what() const throw();
		}
};

#endif