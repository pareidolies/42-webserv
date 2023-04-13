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

	private:
		
};

#endif