#include "Game.h"

#include <cmath>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

Game::Game() {
    m_board = Board();
    tot_turns = 0;
    finished = false;

    Player* white = new Player("white");
    players.push(white);
    Player* black = new Player("black");
    players.push(black);

    std::set<Player*> player_set;
    player_set.insert(white);
    player_set.insert(black);
    m_board.setPlayers(player_set);

    history.push(m_board);
}

Game::~Game() {
    // iterate through players and deallocate memory
    while (!players.empty()) {
        Player * p = players.front();
        players.pop();
        delete(p);
    }
}

// acccessors

void Game::printBoard() {
    std::cout << "   A B C D E F G H\n";
    std::cout << "  ----------------\n";

    for (int row = 0; row < 8; row++) {
        std::cout << 8 - row << "| ";
        for (int col = 0; col < 8; col++) {
            auto coordinates = std::make_pair(row, col);

            // Check if there is a node at the specified row and column
            if (m_board.graph.nodes.find(coordinates) != m_board.graph.nodes.end()) {
                // Get the piece associated with the graph node
                Piece* currentPiece = m_board.graph.nodes[coordinates]->piece;

                // Check if there is a piece in the current graph node
                if (currentPiece != nullptr)
                    std::cout << currentPiece->getType() << " ";
                else
                    std::cout << "- ";
            } else {
                std::cout << "- ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int Game::getTurnNum() {
    return tot_turns;
}

bool Game::isFinished() {
    return finished;
}

Player* Game::getCurPlayer() {
    return players.front();
}

Player* Game::getOpponent() {
    return players.back();
}

bool Game::isValidMove(Player* player, int sx, int sy, int ex, int ey) {
    Piece* movingPiece = m_board.findPiece(sx, sy);
    // Check if the starting position has a piece
    if (movingPiece == nullptr || movingPiece->getType() == '-')
        return false;
    // Check if it's the player's own piece
    if (movingPiece->getType() >= 'a' && movingPiece->getType() <= 'z' && player->getColor() != "white")
        return false;
    if (movingPiece->getType() >= 'A' && movingPiece->getType() <= 'Z' && player->getColor() != "black")
        return false;
    // Find the piece at the destination
    Piece* destinationPiece = m_board.findPiece(ex, ey);
    //Check if the destination is occupied by the player's own piece
    if (destinationPiece != nullptr && destinationPiece->getType() != '-') {
        if ((destinationPiece->getType() >= 'a' && destinationPiece->getType() <= 'z' && player->getColor() == "white") ||
                (destinationPiece->getType() >= 'A' && destinationPiece->getType() <= 'Z' && player->getColor() == "black")) {
            return false;
        }
    }
    //pawn
    if (movingPiece->getType() == 'P' || movingPiece->getType() == 'p') {
        // Pawns capture diagonally
        if (std::abs(ex - sx) == 1 && std::abs(ey - sy) == 1) {
            // Look if the piece is trying to En Passant
            if (destinationPiece == nullptr) {
                if (enPassant(ex, ey)) {
                    // En Passant is valid
                    Player* opPlayer = getOpponent(); //This is the only time we need opponent so there is no reason to call it outside
                    if (opPlayer->getColor() == "black") {
                        // Force move the black pawn to where the white pawn is moving so it can capture
                        Piece* bPawn = m_board.findPiece(ex + 1, ey);
                        bPawn->setRow(ex);
                        bPawn->setCol(ey);
                        history.push(m_board);
                    } else if (opPlayer->getColor() == "white") {
                        // Force move the white pawn to where the black pawn is moving so it can capture
                        std::cout << ex << " " << ey << std::endl;
                        Piece* wPawn = m_board.findPiece(ex - 1, ey);
                        wPawn->setRow(ex);
                        wPawn->setCol(ey);
                        history.push(m_board);
                    }
                    return true; // En passant was successful
                }
                // If !(En Passant), pawn is just trying to do a diagonal capture, so return true
                return true;
            }
        }
        // Pawns move forward
        if (ey == sy) {
            if (((player->getColor() == "white") && (ex == sx - 1)) || ((player->getColor() == "black") && (ex == sx + 1))) {
                // Check if the destination square is unoccupied
                if (destinationPiece == nullptr) {
                    return true;
                } else {
                    std::cout << "Destination square is occupied\n";
                    return false;
                }
            }

            // Allow only one square move for subsequent moves
            if (movingPiece->getHasMoved() && std::abs(ex - sx) == 1) {
                if (destinationPiece == nullptr) {
                    return true;
                } else {
                    std::cout << "Destination square is occupied\n";
                    return false;
                }
            }
        }

        // Pawns move two squares on their first move
        if (!movingPiece->getHasMoved() && ey == sy && ((player->getColor() == "white" && ex == sx - 2) || (player->getColor() == "black" && ex == sx + 2))) {
            // Check if the squares in between are unoccupied
            int middleRow = (player->getColor() == "white") ? sx - 1 : sx + 1;
            return m_board.getBoardEntry(middleRow, ey) == nullptr && destinationPiece == nullptr;
        }

    }

    // Knight
    if (movingPiece->getType() == 'N' || movingPiece->getType() == 'n') {
        // Knights move in an L-shape (two squares in one direction and one square perpendicular)
        return (std::abs(ex - sx) == 2 && std::abs(ey - sy) == 1) || (std::abs(ex - sx) == 1 && std::abs(ey - sy) == 2);
    }
    // Only knight can jump -> all other piece need line of sight to where they are moving
    if (isInLineOfSight(sx, sy, ex, ey)) {
        // Rook
        if (movingPiece->getType() == 'R' || movingPiece->getType() == 'r') {
            // Rooks can move horizontally or vertically
            return (sx == ex || sy == ey);
        }
        // Bishop
        if (movingPiece->getType() == 'B' || movingPiece->getType() == 'b') {
            // Bishops move diagonally
            return std::abs(ex - sx) == std::abs(ey - sy);
        }
        // Queen
        if (movingPiece->getType() == 'Q' || movingPiece->getType() == 'q') {
            // Queens can move horizontally, vertically, or diagonally
            return (sx == ex || sy == ey || std::abs(ex - sx) == std::abs(ey - sy));
        }
        // King
        if (movingPiece->getType() == 'K' || movingPiece->getType() == 'k') {
            std::cout << sy << " " << ey << " " << sx << std::endl;
            // Hard coded castles
            // Black short
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 6 && sx == 0) {
                move(player, sx, 7, ex, 5);
                return true; //since move length > 1, just return true as this would give a invalid move otherwise
            }
            // White short
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 6 && sx == 7) {
                move(player, sx, 7, ex, 5);
                return true;
            }
            // Black long
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 1 && sx == 0) {
                move(player, sx, 0, ex, 2);
                return true;
            }
            // White long
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 1 && sx == 7) {
                move(player, sx, 0, ex, 2);
                return true;
            }

            // Kings can move one square in any direction
            return (std::abs(ex - sx) <= 1 && std::abs(ey - sy) <= 1);
        }
    }
    return false;
}

void Game::move(Player* player, int sx, int sy, int ex, int ey) {
    // Find piece at the starting position
    Piece* movingPiece = m_board.findPiece(sx, sy);

    // Capture the piece at the destination if it exists
    Piece* destinationPiece = m_board.findPiece(ex, ey);
    if (destinationPiece != nullptr) {
        player->updatePiece(destinationPiece);
    }

    // Update the position of the moving piece on the current board
    movingPiece->setRow(ex);
    movingPiece->setCol(ey);
    movingPiece->setHasMoved(true);

    // Push the current board state to the history stack
    history.push(m_board);

    std::queue<Player*> tempQueue = players; // Create a copy of the original queue
    std::set<Player*> playerSet;

    while (!tempQueue.empty()) {
        playerSet.insert(tempQueue.front());
        tempQueue.pop();
    }

    // Create a new Board, set m_board to this new board
    m_board = Board(playerSet, tot_turns);
    // Increment tot_turns at the end
    tot_turns++;

    // Display the updated board with the move
    printBoard();
}

void Game::switchTurn() {
    Player* just_moved = players.front();
    players.pop();
    players.push(just_moved);
}

void Game::setFinished() {
    finished = true;
}

void Game::undo() {
    if (history.size() < 3) {
        return;
    }
    history.pop();
    m_board = history.top();
    switchTurn();
}

bool Game::isValidCheckMove(Player* player, int sx, int sy, int ex, int ey) {
    int count = 1;
    Piece* movingPiece = m_board.findPiece(sx, sy);
    std::cout <<count << std::endl; count++;
    // Check if the starting position has a piece
    if (movingPiece == nullptr || movingPiece->getType() == '-')
        return false;
    std::cout <<count << std::endl; count++;
    // Check if it's the player's own piece
    if (movingPiece->getType() >= 'a' && movingPiece->getType() <= 'z' && player->getColor() != "black")
        return false;
    if (movingPiece->getType() >= 'A' && movingPiece->getType() <= 'Z' && player->getColor() != "white")
        return false;
    std::cout <<count << std::endl; count++;
    // Find the piece at the destination
    Piece* destinationPiece = m_board.findPiece(ex, ey);
    //Check if the destination is occupied by the player's own piece
    if (destinationPiece != nullptr && destinationPiece->getType() != '-') {
        if ((destinationPiece->getType() >= 'a' && destinationPiece->getType() <= 'z' && player->getColor() == "white") ||
                (destinationPiece->getType() >= 'A' && destinationPiece->getType() <= 'Z' && player->getColor() == "black")) {
            return false;
        }
    }
    std::cout <<count << std::endl; count++;
    //pawn
    if (movingPiece->getType() == 'P' || movingPiece->getType() == 'p') {
        // Pawns capture diagonally, since this is the only move a pawn can capture the king, don't look at other moves
        if (std::abs(ex - sx) == 1 && std::abs(ey - sy) == 1) {
            if (destinationPiece == nullptr) {
                return true;
            }
        }
    }
    // Knight
    if (movingPiece->getType() == 'N' || movingPiece->getType() == 'n') {
        // Knights move in an L-shape (two squares in one direction and one square perpendicular)
        return (std::abs(ex - sx) == 2 && std::abs(ey - sy) == 1) || (std::abs(ex - sx) == 1 && std::abs(ey - sy) == 2);
    }
    // Don't need to check for inLineOfSight, since we have a specific function for king already
    // Rook
    if (movingPiece->getType() == 'R' || movingPiece->getType() == 'r') {
        // Rooks can move horizontally or vertically
        return (sx == ex || sy == ey);
    }
    // Bishop
    if (movingPiece->getType() == 'B' || movingPiece->getType() == 'b') {
        // Bishops move diagonally
        return std::abs(ex - sx) == std::abs(ey - sy);
    }
    // Queen
    if (movingPiece->getType() == 'Q' || movingPiece->getType() == 'q') {
        // Queens can move horizontally, vertically, or diagonally
        return (sx == ex || sy == ey || std::abs(ex - sx) == std::abs(ey - sy));
    }
    // King can never capture the other king, don't bother looking

    return false;
}

bool Game::inCheckMate() {
    Player* curPlayer = getCurPlayer();
    Player* opPlayer = getOpponent();
    Piece* opKing = opPlayer->getKing();

    if (opKing == nullptr) {
        // Handle the case where the king is not found
        return false;
    }

    int kingRow = opKing->getRow();
    int kingCol = opKing->getCol();
    int checkCount = 0, validMoves = 0;

    // Iterate through the squares around the king
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                // Skip the center square (king's position)
                continue;
            }

            int newRow = kingRow + i;
            int newCol = kingCol + j;


            // Check if the new position is within the board boundaries
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                Piece* destinationPiece = m_board.findPiece(newRow, newCol);

                // Check if the square is unoccupied
                if (destinationPiece == nullptr) {
                    // Simulate moving the king to the new position
                    m_board.updateBoardAfterMove(opKing, newRow, newCol);
                    printBoard();
                    validMoves++;

                    // Call simulateCheck with the coordinates of the simulated move
                    if (simulateCheck(opPlayer, newRow, newCol)) {
                        checkCount++; //Since king is still in check, iterate the count
                    }
                    // Undo the simulated move
                    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                }
            }
        }
    }
    if (checkCount == validMoves) {
        return true;
    } else {
        return false;
    }
}

bool Game::simulateCheck(Player* player, int newRow, int newCol) {
    Player* curPlayer = getCurPlayer();
    Player* opPlayer = getOpponent();

    Piece* opKing = opPlayer->getKing();

    if (opKing == nullptr) {
        // Handle the case where the king is not found
        std::cout << "King not found." << std::endl;
        return false;
    }

    int kingRow = opKing->getRow();
    int kingCol = opKing->getCol();

    // Simulate moving the opponent's king to the new position
    m_board.updateBoardAfterMove(opKing, newRow, newCol);

    //Look if simulated move puts king in check
    int unprotectedSquares = 0;

    // Check if the opponent's pieces can attack the king
    if (!isKingSurrounded(opPlayer, unprotectedSquares)) {
        // Check for line of sight for each unprotected square
        for (int i = 0; i < 8; ++i) {
            int row = newRow;
            int col = newCol;

            // Check the entire column for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                Piece* curPiece = m_board.findPiece(r, col);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, opPlayer->getKing())) {
                    // Undo the simulated move
                    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                    return true; // The king is in check
                }
            }

            // Check the entire row for opponent pieces that can attack the king
            for (int c = 0; c < 8; ++c) {
                Piece* curPiece = m_board.findPiece(row, c);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                    // Undo the simulated move
                    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                    return true; // The king is in check
                }
            }

            // Check the diagonal for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                int c = col + (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        // Undo the simulated move
                        m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                        return true; // The king is in check
                    }
                }
            }
            for (int r = 0; r < 8; ++r) {
                int c = col - (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        return true; // The king is in check
                    }
                }
            }
        }
    }



    // Undo the simulated move
    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
    return false;
}

bool Game::inCheck() {
    Player* curPlayer = getCurPlayer();
    Player* opPlayer = getOpponent();

    Piece* opKing = opPlayer->getKing();

    if (opKing == nullptr) {
        // Handle the case where the king is not found
        std::cout << "King not found." << std::endl;
        return false;
    }

    int kingRow = opKing->getRow();
    int kingCol = opKing->getCol();

    int unprotectedSquares = 0;
    // Check if the opponent's pieces can attack the king
    if (!isKingSurrounded(opPlayer, unprotectedSquares)) {
        // Check for line of sight for each unprotected square
        for (int i = 0; i < 8; ++i) {
            int row = kingRow;
            int col = kingCol;

            // Check the entire column for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                Piece* curPiece = m_board.findPiece(r, col);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                    return true; // The king is in check
                }
            }

            // Check the entire row for opponent pieces that can attack the king
            for (int c = 0; c < 8; ++c) {
                Piece* curPiece = m_board.findPiece(row, c);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                    return true; // The king is in check
                }
            }

            // Check the y = x diagonal for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                int c = col + (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        return true; // The king is in check
                    }
                }
            }
            // Check the y = x diagonal for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                int c = col - (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        return true; // The king is in check
                    }
                }
            }
        }
    }

    return false; // The king is not in check
}

bool Game::isInLineOfSight(int sx, int sy, int ex, int ey) {
    int startY = sy;
    int startX = sx;
    int endY = ey;
    int endX = ex;
    // Check if the pieces are in the same row, column, or diagonal
    if (startX == endX || startY == endY || std::abs(startX - endX) == std::abs(startY - endY)) {
        // Check if there are no pieces in between
        int stepX = (startX < endX) ? 1 : ((startX > endX) ? -1 : 0);
        int stepY = (startY < endY) ? 1 : ((startY > endY) ? -1 : 0);
        int currentX = startX + stepX;
        int currentY = startY + stepY;
        while (currentX != endX || currentY != endY) {
            // Check if there is a piece in the way
            if (m_board.getBoardEntry(currentX, currentY) != nullptr) {
                return false; // There is an obstacle in the path
            }
            // Move to the next position
            currentX += stepX;
            currentY += stepY;
        }
        return true; // No obstacles found, there is line of sight
    }
    return false; // Pieces are not in the same row, column, or diagonal
}

bool Game::isInLineOfSightOfKing(Piece* startPiece, Piece * endPiece) {
    int startY = startPiece->getRow();
    int startX = startPiece->getCol();
    int endY = endPiece->getRow();
    int endX = endPiece->getCol();
    // Check if the pieces are in the same row, column, or diagonal
    if (startX == endX || startY == endY || std::abs(startX - endX) == std::abs(startY - endY)) {
        // Check if there are no pieces in between
        int stepX = (startX < endX) ? 1 : ((startX > endX) ? -1 : 0);
        int stepY = (startY < endY) ? 1 : ((startY > endY) ? -1 : 0);

        int currentX = startX + stepX;
        int currentY = startY + stepY;

        while (currentX != endX || currentY != endY) {
            // Check if there is a piece in the way
            if (m_board.getBoardEntry(currentX, currentY) != nullptr) {
                return false; // There is an obstacle in the path
            }
            // Move to the next position
            currentX += stepX;
            currentY += stepY;
        }
        return true; // No obstacles found, there is line of sight
    }
    return false; // Pieces are not in the same row, column, or diagonal
}

bool Game::isKingSurrounded(Player* curPlayer, int unprotectedSquares) {
    Piece* curKing = curPlayer->getKing();
    if (curKing == nullptr) {
        //no king found
        return false;
    }

    int kingRow = curKing->getRow();
    int kingCol = curKing->getCol();


    int protection = 0; // counter to see how many squares are protecting the king
    int protecIndex = 0; //index to store unprotected squares

    // Check each of the 8 squares around the king
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                // Skip the center square (king's position)
                continue;
            }

            int adjacentRow = kingRow + i;
            int adjacentCol = kingCol + j;

            // Check if the adjacent square is within the board boundaries
            if (adjacentRow >= 0 && adjacentRow < 8 && adjacentCol >= 0 && adjacentCol < 8) {
                Piece* adjacentPiece = m_board.findPiece(adjacentRow, adjacentCol);
                if (adjacentPiece != nullptr) {
                    // Piece found around the king
                    protection++;
                } else {
                    protecIndex++;
                }
            } else {
                // The adjacent square is outside the board boundaries
                protection++;
            }
        }
    }
    if (protection == 8) //king fully surrounded so there is no point to look further
        return true;
    else {
        return false;
    }
}

void Game::storeTurnToMap() {
    int turns = getTurnNum();
    // Create new hashmaps for white and black pieces
    std::unordered_map<std::string, char> whiteChessHashMap;
    std::unordered_map<std::string, char> blackChessHashMap;
    // Populate the hashmaps with chess board positions
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            // i if for row, j is for col
            std::string key = std::to_string(i) + std::to_string(j);
            Piece* piece = m_board.findPiece(i, j);

            if (piece != nullptr) {
                char type = piece->getType();
                std::string color = piece->getColor();

                // Separate pieces for white and black
                std::unordered_map<std::string, char>& currentHashMap = (color == "black" ? whiteChessHashMap : blackChessHashMap);
                currentHashMap[key] = type;
            }
        }
    }

    // Add the hashmaps to the vectors
    if (turns % 2 == 0) {
        blackTurnHashmaps.push_back(blackChessHashMap);
        // Ensure that there are enough turns to delete
        if (blackTurnHashmaps.size() > turnsToKeep) {
            // Delete and turn history older than 5 to save memory
            blackTurnHashmaps.erase(blackTurnHashmaps.begin(), blackTurnHashmaps.begin() + (blackTurnHashmaps.size() - turnsToKeep));
        }
    } else { // Do the same for white hashmap
        whiteTurnHashmaps.push_back(whiteChessHashMap);
        if (whiteTurnHashmaps.size() > turnsToKeep) {
            whiteTurnHashmaps.erase(whiteTurnHashmaps.begin(), whiteTurnHashmaps.begin() + (whiteTurnHashmaps.size() - turnsToKeep));
        }
    }
}

void Game::merge(std::vector<std::pair<std::string, char>>&arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<std::pair < std::string, char>> leftHalf(arr.begin() + left, arr.begin() + left + n1);
    std::vector<std::pair < std::string, char>> rightHalf(arr.begin() + mid + 1, arr.begin() + mid + 1 + n2);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftHalf[i].first <= rightHalf[j].first) {
            arr[k++] = leftHalf[i++];
        } else {
            arr[k++] = rightHalf[j++];
        }
    }
    while (i < n1) {
        arr[k++] = leftHalf[i++];
    }
    while (j < n2) {
        arr[k++] = rightHalf[j++];
    }
}

void Game::mergeSort(std::vector<std::pair<std::string, char>>&arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

bool Game::areMapsEqual(const std::unordered_map<std::string, char>& map1, const std::unordered_map<std::string, char>& map2) {
    // Check if the size of the hashmaps is the same
    if (map1.size() != map2.size()) {
        return false;
    }
    // Create a lambda function to compare key-value pairs
    auto pairComparator = [](const std::pair<std::string, char>& p1, const std::pair<std::string, char>& p2) {
        return p1.first < p2.first || (p1.first == p2.first && p1.second < p2.second);
    };

    // Sort the key-value pairs in map1 and map2
    std::vector<std::pair < std::string, char>> sorted1(map1.begin(), map1.end());
    std::vector<std::pair < std::string, char>> sorted2(map2.begin(), map2.end());
    mergeSort(sorted1, 0, sorted1.size() - 1);
    mergeSort(sorted1, 0, sorted1.size() - 1);

    // Compare the sorted key-value pairs
    return sorted1 == sorted2;
}

bool Game::stalemate() {
    int turns = getTurnNum();
    int turnsToCheck = turnsToKeep;
    int total;

    // If turns even check black
    if (turns % 2 == 0) {
        total = blackTurnHashmaps.size();
        if (total >= turnsToCheck) {
            // Only look at every other map
            for (int i = 2; i < total; i += 2) {
                std::cout << "checking maps: " << i << " " << i - 2 << std::endl;
                const auto& currentHashmap = blackTurnHashmaps[i];
                const auto& previousHashmap = blackTurnHashmaps[i - 2];

                if (!areMapsEqual(currentHashmap, previousHashmap)) {
                    return false; // Exit the loop early if inequality is found since there won't be a stalemate
                }
            }
            return true; // Stalemate condition
        }

    } else {
        total = whiteTurnHashmaps.size();
        if (total >= turnsToCheck) {
            // Only look at every other map
            for (int i = 2; i < total; i += 2) {
                std::cout << "checking maps: " << i << " " << i - 2 << std::endl;
                const auto& currentHashmap = whiteTurnHashmaps[i];
                const auto& previousHashmap = whiteTurnHashmaps[i - 2];

                if (!areMapsEqual(currentHashmap, previousHashmap)) {
                    return false; // Exit the loop early if inequality is found since there won't be a stalemate
                }
            }
            return true; // Stalemate condition
        }

    }
    return false; // Default return, indicating no stalemate
}

void Game::displayPreviousTurns() {
    int totalTurns = getTurnNum();
    // Iterate over the vectors containing hashmaps for white and black pieces
    if (totalTurns % 2 == 0) {
        for (int i = 0; i < blackTurnHashmaps.size(); ++i) {
            std::cout << "Black pieces hashmap " << i << ": " << std::endl;
            for (const auto& pair : blackTurnHashmaps[i]) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }

            std::cout << std::endl;
        }
    } else {
        for (int i = 0; i < whiteTurnHashmaps.size(); ++i) {

            std::cout << "White piece hashmap " << i << ": " << std::endl;
            for (const auto& pair : whiteTurnHashmaps[i]) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

bool Game::enPassant(int ex, int ey) {
    Player* curPlayer = getCurPlayer();
    Player* opPlayer = getOpponent();
    int size = 0;

    // White is moving
    if (opPlayer->getColor() == "black") {
        size = blackTurnHashmaps.size();

        // Map1 black pawn should be rank 7, map2 black pawn needs to be rank 5 for En passant
        const std::unordered_map<std::string, char>& map1 = blackTurnHashmaps[size - 2]; // Second most recent hashmap
        const std::unordered_map<std::string, char>& map2 = blackTurnHashmaps[size - 1]; // Most recent hashmap
        // By only looking at most recent, 2nd most recent, we know the pawn just moved an is able to be En Passanted

        // Try and find a pawn at each location in the hashmap history
        auto pawn1 = map1.find(std::to_string(ex - 1) + std::to_string(ey));
        auto pawn2 = map2.find(std::to_string(ex + 1) + std::to_string(ey));

        if (pawn1->second == 'P') {
            if (pawn2->second == 'P') {
                return true;
            }
        } else
            return false;
    }// Black is moving
    else {
        size = whiteTurnHashmaps.size();

        // Map1 black pawn should be rank 7, map2 black pawn needs to be rank 5 for En passant
        const std::unordered_map<std::string, char>& map1 = whiteTurnHashmaps[size - 2]; // Second most recent hashmap
        const std::unordered_map<std::string, char>& map2 = whiteTurnHashmaps[size - 1]; // Most recent hashmap
        // By only looking at most recent, 2nd most recent, we know the pawn just moved an is able to be En Passanted


        // Try and find a pawn at each location in the hashmap history
        auto pawn1 = map1.find(std::to_string(ex + 1) + std::to_string(ey));
        auto pawn2 = map2.find(std::to_string(ex - 1) + std::to_string(ey));
        if (pawn1->second == 'p') {
            if (pawn2->second == 'p') {
                return true;
            }
        } else
            return false;
    }
}

void Game::start() {
    std::queue<Player*> tempQueue = players; // Create a copy of the original queue
    std::set<Player*> playerSet;

    while (!tempQueue.empty()) {
        playerSet.insert(tempQueue.front());
        tempQueue.pop();
    }
    m_board = Board(playerSet, 0);
}