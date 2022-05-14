# Chess in C++

Nothing fancy, just a naive implementation as a personal project.

## Build Instructions
1. Clone repo
2. In the same directory as the cloned repo, create a build folder: `mkdir chess_build`
3. `cd chess_build`
4. `cmake ../chess`
5. `cmake --build .`

## Remaining Work
* Detect checkmate
* Detect if a move blocks check/protects king
* Detect stalemate
* Observe half-move rule
* Update unit tests for Board
* FEN output of arbitrary board state (stretch goal)
* Graphics (stretch goal)
