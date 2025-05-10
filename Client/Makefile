PLATFORM := $(shell uname -s)

CC 		= 	g++

CFLAGS	= 	-std=c++17 -Wall -Wextra -g3

LDFLAGS	=	

INCLUDES= 	-I./include \

RM		=	rm -f

NAME	=	client

SRC		=	$(wildcard *.cpp) \
			$(wildcard source/*.cpp) \
			
OBJ		= 	$(SRC:.cpp=.o)

all		:	$(NAME)

$(NAME)	:	$(OBJ)
		$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean	:
		$(RM) $(OBJ)

fclean	:	clean
		$(RM) $(NAME)

re		:	fclean all

.PHONY: all clean fclean re