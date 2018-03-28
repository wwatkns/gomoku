SRC_PATH = ./src/
INC_PATH = ./include/
OBJ_PATH = ./obj/
EIGEN_PATH = $(HOME)/.eigen/
BOOST_PATH = $(HOME)/.brew/Cellar/boost/1.66.0/include
BOOST_LIB = $(HOME)/.brew/Cellar/boost/1.66.0/lib/libboost_program_options.a

NAME = gomoku
CC = clang++
CFLGS = -Werror -Wextra -Wall -std=c++11 -Ofast

SDLFLGS = -framework SDL2 -framework SDL2_image -framework SDL2_ttf
SDL_INC = $(HOME)/Library/Frameworks/SDL2.framework/Headers/
SDL_IMG_INC = $(HOME)/Library/Frameworks/SDL2_image.framework/Headers/
SDL_TTF_INC = $(HOME)/Library/Frameworks/SDL2_ttf.framework/Headers/

SDL = -F $(HOME)/Library/Frameworks -I$(SDL_INC) -I$(SDL_IMG_INC) -I$(SDL_TTF_INC)

SRC_NAME = main.cpp Human.cpp Computer.cpp AIPlayer.cpp AIAlgorithms.cpp Game.cpp GameEngine.cpp GraphicalInterface.cpp \
		   BitBoard.cpp Chronometer.cpp Button.cpp ButtonSwitch.cpp \
		   ButtonSelect.cpp FontHandler.cpp FontText.cpp Analytics.cpp \
		   Player.cpp
OBJ_NAME = $(SRC_NAME:.cpp=.o)

SRC = $(addprefix $(SRC_PATH), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))
INC = $(addprefix -I,$(INC_PATH) $(EIGEN_PATH) $(BOOST_PATH))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLGS) $(INC) $(SDL) $(BOOST_LIB) $(SDLFLGS) $(OBJ) -o $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLGS) $(INC) $(SDL) -o $@ -c $<

clean:
	rm -fv $(OBJ)
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -fv $(NAME)

re: fclean all
