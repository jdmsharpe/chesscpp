# Chess in C++

Chess program. Comes with three main modes:
1. Chess with two players (default)
2. Chess with one player and one computer player, accessed through `-c` argument
3. Legacy mode on CLI, accessed through `--legacy` argument

The first two modes use SDL for a graphical interface, which is a library I'm familiar with from past projects.

The computer player uses a basic minimax algorithm implementation with a hardcoded depth to evaluate future board positions. At some point, I might make the depth toggleable.

Users can also choose their desired color against the computer player with `-w`, `-b`, or `-r` (white, black, and random, respectively). Note that these options are not supported in two-player mode.

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
The following list describes all options can be passed as additional arguments when running the executable, e.g. `./chess -l`.
* `-b` - sets the player color to be black, and the computer player white
* `-c` - starts a game with a computer player (computer player's default color is black)
* `-l` - loads a board state from the `load.fen` file under `inc`
* `-r` - randomizes the player's and computer player's colors
* `-v` - enables verbose/debugging mode
* `-w` - sets the player color to be white, and the computer player black
* `--legacy` - enables legacy CLI mode with no SDL graphics (only supports two-player mode)

## Remaining Work
* Investigate edge cases - AI move generation #1 suspect
* Update pawn promotion to be graphical interaction instead of CLI
* Add en passant unit test
* Add checkmate unit test
* Add pawn promotion unit test (after update)
* Add AI unit tests in new test file
* FEN output of arbitrary board state
* Make AI search depth toggleable during runtime, probably via keyboard
* Web things...???
