#ifndef GENERAL_HPP
# define GENERAL_HPP

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>


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

 // Structure pour stocker les informations de la requête
    struct Request {
        std::string method;                          // méthode HTTP utilisée (GET, POST, etc.)
        std::string uri;                             // URI de la ressource demandée
        map<std::string, std::string> headers;  // en-têtes de la requête
        std::string body;                            // corps de la requête
		std::string raw_request;
    };

	struct Response {
		std::string full_response;
		std::string body;
		std::string cgi_path;
		std::string body_size;
		std::string content_type;
        std::map<int, std::string> status_code_list;
        int	status_code;
	};

#endif