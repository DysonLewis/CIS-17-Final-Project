# Chess Program

## Description
This chess program is a console-based application implementing the classic game of chess. It provides a two-player experience, where each player takes turns to make their moves on an 8x8 chessboard.

## Basic Inputs
- Players can input their moves using algebraic notation (e.g., "e2e4" to move a pawn from e2 to e4).
- The game follows the standard chess notation for moves.

## Rules of Chess
Chess is a two-player strategy board game. The objective is to checkmate the opponent's king, putting it into a position where it cannot escape capture.

### Key Chess Rules:
1. **Pawn Movement:**
   - Pawns move forward but capture diagonally.
   - On their first move, pawns can advance two squares.

2. **Rook Movement:**
   - Rooks move horizontally or vertically across the board.

3. **Knight Movement:**
   - Knights move in an L-shape, making two squares in one direction and one square perpendicular to it.

4. **Bishop Movement:**
   - Bishops move diagonally across the board.

5. **Queen Movement:**
   - Queens can move horizontally, vertically, or diagonally.

6. **King Movement:**
   - Kings move one square in any direction.

7. **Castling:**
   - Castling is a special move involving the king and a rook, allowing both to move simultaneously under certain conditions.

8. **En Passant:**
   - A pawn capturing move that occurs immediately after a pawn advances two squares from its starting position.

9. **Check and Checkmate:**
   - **Check:** A king is in check when it is under attack. The player must make a move to remove the threat.
   - **Checkmate:** The game ends when a king is in check, and there is no legal move to escape. This results in a victory for the attacking player.

10. **Stalemate:**
    - The game ends in a draw if a player has no legal moves but is not in check, or if there has a been a 3-fold repitition in moves (player has kept readting the same move over and over again)

## Data Structures and Algorithms
The program uses various data structures and algorithms to implement the chess logic efficiently.

1. **Graph Representation:**
   - The chessboard is represented as a graph, where each square is a node, and the legal moves between squares are edges.

2. **Move Validation:**
   - The program validates moves based on the rules of each chess piece, treating the board as a graph.
   - It checks for legal moves, including castling and en passant.

3. **Line of Sight and King Protection:**
   - The `isInLineOfSightOfKing` function checks if there is a clear line of sight between two pieces.
   - The `isKingSurrounded` function determines if the opponent's king is surrounded by pieces.

4. **Hashmaps for Turn History:**
   - Hashmaps are used to store the state of the chessboard at each turn.
   - The program utilizes merge sort to compare and check for stalemate conditions.

5. **En Passant Detection:**
   - The `enPassant` function detects if an en passant move is possible based on the history of pawn movements.

6. **Player Queue and Set:**
   - A queue and set are used to manage players and ensure unique players are present in the game.
   - The program starts by creating a copy of the player queue.

## Getting Started
1. Clone the repository.
2. Compile and run the program.
3. Follow the on-screen instructions to make your moves.

Enjoy playing chess!
