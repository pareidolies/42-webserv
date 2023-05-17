#ifndef GENERAL_HPP
# define GENERAL_HPP

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

class General
{
	public:
		General();
		~General();
		
		static void log(const string &message)
		{
			cout << message << endl;
		}

		static void exitWithError(const string &errorMessage)
		{
			log("ERROR: " + errorMessage);
			exit(1);
		}

		static void free_tab(char **tab)
		{
			int i = 0;
			while (tab[i])
				free(tab[i++]);
			free(tab);
		}

		static string to_string(int n)
		{
			string s;
			if (n < 0)
			{
				s += '-';
				n = -n;
			}
			if (n / 10)
				s += to_string(n / 10);
			s += n % 10 + '0';
			return (s);
		}

		static string to_upper(string str)
		{
			for (size_t i = 0; i < str.length(); i++)
				str[i] = toupper(str[i]);
			return (str);
		}

		static string to_lower(string str)
		{
			for (size_t i = 0; i < str.length(); i++)
				str[i] = tolower(str[i]);
			return (str);
		}
		
	private:
		
};

#endif