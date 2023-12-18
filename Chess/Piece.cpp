#include "Piece.h"

Piece::Piece(char t, int r, int c)
{
    type = t;
    captured = false;
    row = r;
    col = c;
    hasMoved = false;  // Initialize the hasMoved flag to false
}

Piece::~Piece() {}

bool Piece::isCaptured()
{
    return captured;
}

char Piece::getType()
{
    return type;
}

int Piece::getRow()
{
    return row;
}

int Piece::getCol()
{
    return col;
}

void Piece::setCaptured(bool b)
{
    captured = b;
}

void Piece::setType(char c)
{
    type = c;
}

void Piece::setRow(int r)
{
    row = r;
}

void Piece::setCol(int c)
{
    col = c;
}

bool Piece::getHasMoved() {
    return hasMoved;
}

void Piece::setHasMoved(bool moved) {
    hasMoved = moved;
}