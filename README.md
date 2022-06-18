# Chess in C++

Chess program that allows two players to play a game. The application has both a CLI and graphical interface toggled through an additional argument. I originally wrote this purely for CLI, but it's really hard to see moves effectively, so I updated this to use SDL as I'm familiar with that library.

At some point, I might try to write an AI so this can be fully 1-player.

![Chess GIF](https://media2.giphy.com/media/aA8bACmZSEhHIdxsfU/giphy.gif?cid=790b761186235a808b461eff10cc757c9a1679cbd330530d&rid=giphy.gif&ct=g)

## Build Instructions
1. Requires cmake, SDL2, and SDL2_Image: `sudo apt-get install cmake libsdl2-dev libsdl2-image-dev`
2. Clone repo
3. In the same directory as the cloned repo, create a build folder: `mkdir chess_build`
4. `cd chess_build`
5. `cmake ../chess`
6. `cmake --build .`
7. `./chess`

## Additional Options
The following options can be passed as additional arguments when running the executable, e.g. `./chess -l`.
* Passing `-l` will load a board state from the `load.fen` file under `inc`
* Passing `-c` will start a game with a computer player
* Passing `-v` will enable verbose/debugging mode
* Passing `--legacy` will enable legacy CLI mode with no SDL graphics (only supports 2-player)

## Remaining Work
* Investigate edge cases
* Update unit tests for Board
* FEN output of arbitrary board state
