SRC_PATH = ./src/
INC_PATH = ./include/
OBJ_PATH = ./obj/
EIGEN_PATH = $(HOME)/.eigen/

NAME = gomoku
CC = clang++
# CFLGS = -Werror -Wextra -Wall -Ofast -std=c++11
CFLGS = -std=c++11

SRC_NAME = main.cpp Game.cpp GameEngine.cpp Player.cpp Human.cpp Computer.cpp
OBJ_NAME = $(SRC_NAME:.cpp=.o)

SRC = $(addprefix $(SRC_PATH), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))
INC = $(addprefix -I,$(INC_PATH) $(EIGEN_PATH))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLGS) $(INC) $(OBJ) -o $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLGS) $(INC) -o $@ -c $<

clean:
	rm -fv $(OBJ)
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -fv $(NAME)

re: fclean all
