SRC = srcs/main.cpp \
		srcs/Server.cpp \
		srcs/Utils.cpp \
		srcs/Request.cpp \
		srcs/Parsing.cpp \
		srcs/Response.cpp \
		srcs/Webserv.cpp \
		srcs/ParsingGet.cpp \
		srcs/ParsingPost.cpp \
		srcs/ParsingDelete.cpp \
		srcs/ParsingPut.cpp \
		srcs/ResponseGet.cpp \
		srcs/ResponsePost.cpp

OBJ_DIR = obj
OBJ = $(SRC:srcs/%.cpp=$(OBJ_DIR)/%.o) 
CXX = c++
RM = rm -f
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror

NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: srcs/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re