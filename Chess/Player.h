#ifndef Player_h
#define Player_h

#include <stdio.h>
#include <list>
#include <string>
#include <set>
#include <queue>

#include "Piece.h"

class Player
{
public:
    Player(std::string c);
    ~Player();
    std::string getColor();
    std::list<Piece*> getActivePieces();
    void updatePiece(Piece* p);
    Piece* getKing() const;
    Piece* getRook() const;

private:
    std::list<Piece*> pieces;
    std::set<Piece*> activePieces;
    std::string color;
};

#endif /* Player_h */
