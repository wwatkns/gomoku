# gomoku

This is a **school project**. The goal is to make a Gomoku game and an AI based on minimax algorithm.

## Installation

Download the latest development library for MacOSX:

- SDL2: https://www.libsdl.org/download-2.0.php
- SDL_image: https://www.libsdl.org/projects/SDL_image/
- SDL_ttf: https://www.libsdl.org/projects/SDL_ttf/
- Boost 1.66.0 (on MacOs just do `brew install brew`)

Then put the `.framework` files in `HOME/Library/Frameworks`.

## Usage

```
Options:
  -h [ --help ]         Print help options
  -a [ --ai ] arg       Choose AI algorithm:
                        (1) default,
                        (2) MinMax,
                        (3) AlphaBeta,
                        (4) MTDf,
                        (5) MCTS
  -d [ --depth ] arg    Select the maximum depth
```
