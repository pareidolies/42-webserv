#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <cstring>


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "General.hpp"
#include "TcpServer.hpp"
#include "Socket.hpp"

#include "Configuration.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "cgi.hpp"
#include "Response.hpp"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <csignal>

#include <arpa/inet.h>
#include <sys/epoll.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>

#include <algorithm>
#include <signal.h>


using namespace std;

#define FILEENAME_MAX 4096

#define MAX_EVENTS	128

/******************************************************************************
*                                 COLORS                                      *
******************************************************************************/

extern std::sig_atomic_t g_shutdown ;

# define ANSI_PURPLE		"\x1b[95m"
# define ANSI_BLUE			"\x1b[94m"
# define ANSI_YELLOW		"\x1b[93m"
# define ANSI_RED			"\x1b[91m"
# define ANSI_GREEN			"\x1b[92m"
# define ANSI_CYAN			"\x1b[96m"
# define ANSI_RESET			"\x1b[0m"

#endif