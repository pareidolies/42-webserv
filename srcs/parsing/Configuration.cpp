# include "Configuration.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Configuration::Configuration(void)
{

}

Configuration::Configuration(std::string file) : _file(file)
{
	
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Configuration::Configuration(Configuration const & copy) : _vector(copy._vector)
{

}

Configuration	&Configuration::operator=(Configuration const & rhs)
{
	if (this != &rhs)
	{
		_vector = rhs._vector;
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Configuration::~Configuration(void)
{

}

/******************************************************************************
*                                  GETTERS                                    *
******************************************************************************/


/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/
