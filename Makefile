SRC = srcs/main.cpp srcs/server.cpp srcs/utils.cpp srcs/request.cpp srcs/response.cpp srcs/parsingGet.cpp srcs/parsingPost.cpp
OBJ = $(SRC:.cpp=.o) 
CXX = c++
RM = rm -f
CXXFLAGS = -std=c++98 # -Wall -Wextra -Werror

NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re