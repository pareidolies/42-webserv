###############################################################################
#                                                                             #
#                              Basic Config                                   #
#                                                                             #
###############################################################################

NAME		=	webserv

RM			=	rm -rf

CC			=	c++

# CFLAGS		=	-Wall -Werror -Wextra 
CFLAGS		=	-std=c++98
CFLAGS		+=	-I $(INCS_DIR) -I $(PARSING_DIR)
CFLAGS		+=	-g3 -MMD

Y			= "\033[33m"
R			= "\033[31m"
GREEN		= "\033[92m"
BLUE		= "\033[94m"
X			= "\033[0m"
UP			= "\033[A"
CUT			= "\033[K"

###############################################################################
#                                                                             #
#                                  Variable                                   #
#                                                                             #
###############################################################################

SRCS_DIR	=	srcs

INCS_DIR	=	incs 

PARSING_DIR =	incs/parsing

OBJS_DIR	=	objs

SRCS_SOCKET	=	TcpServer.cpp

SRCS_PARSER	=	parsing/Configuration.cpp \
				parsing/Server.cpp \
				parsing/Location.cpp

SRCS		=	$(SRCS_SOCKET)
SRCS		+=	$(SRCS_PARSER)
SRCS		+=	main.cpp
				
OBJS		=	$(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

DEPS		=	$(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.d))

###############################################################################
#                                                                             #
#                                   COMMAND                                   #
#                                                                             #
###############################################################################

all:			$(NAME)

$(NAME):		$(OBJS)
				${CC} ${OBJS} ${CFLAGS} -o $@
				@echo ${GREEN}"webserv Compile Successfully"${X}
				@echo ${BLUE}"Command: ./webserv {configuration file}.conf\n"${X}

$(OBJS_DIR)/%.o:$(SRCS_DIR)/%.cpp
				@mkdir -p $(dir $@)
				$(CC) $(CFLAGS) -c $< -o $@

clean:
				$(RM) $(OBJS_DIR)

fclean:			clean
				$(RM) $(NAME)
				@echo ${GREEN}"webserv Clean Successfully\n"${X}

re:				fclean all

-include $(DEPS)

.PHONY:			all clean fclean re
