#ifndef Game_h
#define Game_h

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

#include "Board.h"
#include "Player.h"
#include "Piece.h"

class Game
{
public:
    Game();
    ~Game();
    
    // acccessors
    void printBoard();
    int getTurnNum();
    bool isFinished();
    Player* getCurPlayer();
    Player* getOpponent();
    bool isKingSurrounded(Player* opponentPlayer, int unprotectedSquares);
    bool simulateCheck(Player* player, int newRow, int newCol);
    
    
    bool isValidMove(Player* player, int sx, int sy, int ex, int ey);
    bool isInLineOfSight(int sx, int sy, int ex, int ey);
    void move(Player* player, int sx, int sy, int ex, int ey);
    void switchTurn();
    void setFinished();
    void undo();
    
    bool isValidCheckMove(Player* player, int sx, int sy, int ex, int ey);
    bool inCheckMate();
    bool inCheck();
    void storeTurnToMap();
    bool enPassant(int ex, int ey);
    bool stalemate();
    void displayPreviousTurns();
    
    void start();
    
private:
    Board m_board;
    int tot_turns;
    bool finished;
    std::queue<Player*> players;
    std::stack<Board> history;
    bool isInLineOfSightOfKing(Piece* startPiece, Piece* endPiece);
    
    
    
    // Define vectors to store hashmaps for white and black pieces
    std::vector<std::unordered_map<std::string, char>> whiteTurnHashmaps;
    std::vector<std::unordered_map<std::string, char>> blackTurnHashmaps;
    int turnsToKeep = 5;
    /*Stalemate occurs after 3 repeated position
     However we need every other to to see if the piece is in the same spot*/
    void merge(std::vector<std::pair<std::string, char>>& arr, int left, int mid, int right);
    void mergeSort(std::vector<std::pair<std::string, char>>& arr, int left, int right);
    bool areMapsEqual(const std::unordered_map<std::string, char>& map1, const std::unordered_map<std::string, char>& map2);
    
    // helper
    Piece* findPiece(int x, int y);
};

#endif /* Game_h */
