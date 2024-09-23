NAME = webserv

CC = c++

CFFAGS = -Wall -Werror -Wextra -std=c=+98

SRC = ./srcs/*.cpp

OBJ = $(SRC:.cpp=.o)

RM = rm -rf

$(NAME) : $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

all : $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(NAME)

re : fclean all