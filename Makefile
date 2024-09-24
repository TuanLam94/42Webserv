NAME = webserv

CC = c++

CFFAGS = -Wall -Werror -Wextra -std=c=+98 -g3

SRC = $(wildcard ./srcs/*.cpp)

OBJ_DIR = obj
OBJ = $(patsubst ./srcs/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

RM = rm -rf

$(NAME) : $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

all : $(NAME)

$(OBJ_DIR)/%.o: ./srcs/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(NAME)

re : fclean all