#ifndef Board_h
#define Board_h

#include <vector>
#include <unordered_map>
#include <set>
#include "Player.h"
#include "Piece.h"

class GraphNode {
public:
    int row;
    int col;
    char pieceChar;
    Piece* piece;
    
    std::set<GraphNode*> neighbors;

   GraphNode(int r, int c, Piece* p);
};

struct PairHash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);

        // A simple hash function combining two hash values
        return hash1 ^ hash2;
    }
};

struct PairEqual {
    template <class T1, class T2>
    bool operator () (const std::pair<T1, T2>& p1, const std::pair<T1, T2>& p2) const {
        return p1.first == p2.first && p1.second == p2.second;
    }
};

class ChessGraph {
public:
    std::unordered_map<std::pair<int, int>, GraphNode*, PairHash, PairEqual> nodes;

    void addNode(int row, int col, char pieceChar, Piece* piece);
    void addEdge(int row1, int col1, int row2, int col2);
};

class Board {
public:
    ChessGraph graph;
    Board();
    Board(std::set<Player*> pl, int t);
    int getTurn();
    Piece* getBoardEntry(int r, int c);
    Piece* findPiece(int x, int y);
    void setPlayers(std::set<Player*> pl);
    void updateBoardAfterMove(Piece* piece, int newRow, int newCol);
    
    

private:    
    void updateGraphEdges(Piece* piece, int row, int col);
    int turn_num;
    std::set<Player*> players;
};
#endif /*Board_h*/