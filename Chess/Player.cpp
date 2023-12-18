
#include "Player.h"
#include <iostream>

Player::Player(std::string c)
{
    bool black_piece = c.compare("black") == 0;
    int pawn_row = (black_piece) ? 1 : 6;
    int piece_row = (black_piece) ? 0 : 7;
    // create 8 pawns
    for (int i = 0; i < 8; i++)
    {
        Piece* pawn = (black_piece)? new Piece('P', pawn_row, i) : new Piece('p', pawn_row, i);
        pieces.push_back(pawn);
    }
    // create pieces
    if (black_piece) {
        Piece* rook1 = new Piece('R', piece_row, 0);
        Piece* knight1 = new Piece('N', piece_row, 1);
        Piece* bishop1 = new Piece('B', piece_row, 2);
        Piece* queen = new Piece('Q', piece_row, 3);
        Piece* king = new Piece('K', piece_row, 4);
        Piece* bishop2 = new Piece('B', piece_row, 5);
        Piece* knight2 = new Piece('N', piece_row, 6);
        Piece* rook2 = new Piece('R', piece_row, 7);
        pieces.push_back(rook1);
        pieces.push_back(knight1);
        pieces.push_back(bishop1);
        pieces.push_back(queen);
        pieces.push_back(king);
        pieces.push_back(bishop2);
        pieces.push_back(knight2);
        pieces.push_back(rook2);
    } else {
        Piece* rook1 = new Piece('r', piece_row, 0);
        Piece* knight1 = new Piece('n', piece_row, 1);
        Piece* bishop1 = new Piece('b', piece_row, 2);
        Piece* queen = new Piece('q', piece_row, 3);
        Piece* king = new Piece('k', piece_row, 4);
        Piece* bishop2 = new Piece('b', piece_row, 5);
        Piece* knight2 = new Piece('n', piece_row, 6);
        Piece* rook2 = new Piece('r', piece_row, 7);
        pieces.push_back(rook1);
        pieces.push_back(knight1);
        pieces.push_back(bishop1);
        pieces.push_back(queen);
        pieces.push_back(king);
        pieces.push_back(bishop2);
        pieces.push_back(knight2);
        pieces.push_back(rook2);
    }
    color = c;
}

Player::~Player()
{
    std::list<Piece*>::iterator it = pieces.begin();
    while (it != pieces.end())
    {
        Piece* cur = *it;
        delete cur;
        it++;
    }
}

std::string Player::getColor()
{
    return color;
}

std::list<Piece*> Player::getActivePieces()
{
    // iterate through list of pieces, if piece is captured, add to return_list
    std::list<Piece*> return_list;
    for (const auto& piece : pieces)
    {
        if (!piece->isCaptured())
            return_list.push_back(piece);
    }
    return return_list;
}

Piece* Player::getKing() const {
    for (Piece* piece : pieces) {
        if (piece->getType() == 'K' || piece->getType() == 'k') {
            return piece;
        }
    }
    return nullptr; // Handle the case where the king is not found
}

Piece* Player::getRook() const {
    for (Piece* piece : pieces) {
        if (piece->getType() == 'R' || piece->getType() == 'r') {
            return piece;
        }
    }
    return nullptr; // Handle the case where the king is not found
}


void Player::updatePiece(Piece* p)
{
     p->setCaptured(true);
    //update the position of the captured piece to an invalid position
    p->setRow(-1);
    p->setCol(-1);
}