SRC_PATH = ./src/
INC_PATH = ./include/
OBJ_PATH = ./obj/
EIGEN_PATH = $(HOME)/.eigen/

NAME = gomoku
CC = clang++
# CFLGS = -Werror -Wextra -Wall -Ofast -std=c++11
CFLGS = -std=c++11

SDLFLGS = -framework SDL2 -framework SDL2_image
SDL_INC = $(HOME)/Library/Frameworks/SDL2.framework/Headers/
SDL_IMG_INC = $(HOME)/Library/Frameworks/SDL2_image.framework/Headers/
SDL = -F $(HOME)/Library/Frameworks -I$(SDL_INC) -I$(SDL_IMG_INC) $(SDLFLGS)

SRC_NAME = main.cpp Game.cpp GameEngine.cpp Player.cpp Human.cpp Computer.cpp GraphicalInterface.cpp
OBJ_NAME = $(SRC_NAME:.cpp=.o)

SRC = $(addprefix $(SRC_PATH), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))
INC = $(addprefix -I,$(INC_PATH) $(EIGEN_PATH) $(GFX_PATH)include/)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLGS) $(INC) $(SDL) $(OBJ) -o $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLGS) $(INC) $(SDL) -o $@ -c $<

clean:
	rm -fv $(OBJ)
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -fv $(NAME)

re: fclean all
