# Chess in C++

Nothing fancy, just a naive implementation as a personal project.

![Chess GIF](https://media2.giphy.com/media/aA8bACmZSEhHIdxsfU/giphy.gif?cid=790b761186235a808b461eff10cc757c9a1679cbd330530d&rid=giphy.gif&ct=g)

## Build Instructions
1. Requires cmake: `sudo apt-get install cmake`
2. Clone repo
3. In the same directory as the cloned repo, create a build folder: `mkdir chess_build`
4. `cd chess_build`
5. `cmake ../chess`
6. `cmake --build .`
7. `./chess`

## Additional Options
The following options can be passed as an additional argument when running the executable, e.g. `./chess -l`.
* Passing `-l` will load a board state from the `load.fen` file under `inc`
* Passing `-v` will enable verbose/debugging mode
* Passing `--legacy` will enable legacy CLI mode with no SDL graphics

## Remaining Work
* Observe half-move rule
* Investigate edge cases
* Update unit tests for Board
* FEN output of arbitrary board state (stretch goal)
* AI? (stretch goal)
