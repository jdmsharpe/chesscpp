# Chess in C++

Nothing fancy, just a naive implementation as a personal project.

## Build Instructions
1. Requires cmake: `sudo apt-get install cmake`
2. Clone repo
3. In the same directory as the cloned repo, create a build folder: `mkdir chess_build`
4. `cd chess_build`
5. `cmake ../chess`
6. `cmake --build .`

## Remaining Work
* Detect stalemate
* Observe half-move rule
* Investigate edge cases
* Update unit tests for Board
* FEN output of arbitrary board state (stretch goal)
* Graphics (stretch goal)
