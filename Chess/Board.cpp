#include "Board.h"
#include "Player.h"
#include <iostream>

Board::Board() {
    turn_num = 0;
}

GraphNode::GraphNode(int r, int c, Piece* p) : row(r), col(c), piece(p) {}

Board::Board(std::set<Player*> pls, int t) {
    turn_num = t;
    players = pls;
    // Iterate through the set of players
    for (const auto& player : pls) {
        // Iterate through the player's active pieces
        for (Piece* piece : player->getActivePieces()) {
            // Get the type, row, and column of the piece
            int row = piece->getRow();
            int col = piece->getCol();
            // Update the graph with the piece's type and piece
            graph.addNode(row, col, piece->getType(), piece);
        }
    }
}

void Board::updateGraphEdges(Piece* piece, int row, int col) {
    // For simplicity, consider all pieces can move to any adjacent square
    // Check adjacent squares
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int newRow = row + i;
            int newCol = col + j;
            // Check if the new position is within the board bounds
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                // Add an edge between the current node and the adjacent node
                graph.addEdge(row, col, newRow, newCol);
            }
        }
    }
}


int Board::getTurn()
{
    return turn_num;
}

Piece* Board::getBoardEntry(int r, int c) {
    auto it = graph.nodes.find(std::make_pair(r, c));
    if (it != graph.nodes.end()) {
        // Return the piece associated with the graph node
        return it->second->piece;
    }
    return nullptr;
}

Piece* Board::findPiece(int x, int y) {
    auto it = graph.nodes.find(std::make_pair(x, y));
    if (it != graph.nodes.end()) {
        // Return the piece associated with the graph node
        return it->second->piece;
    }
    return nullptr;
}

void Board::setPlayers(std::set<Player*> pl)
{
    players = pl;
}

void Board::updateBoardAfterMove(Piece* piece, int newRow, int newCol) {
    int oldRow = piece->getRow();
    int oldCol = piece->getCol();

    // Clear the old position on the graph
    graph.addEdge(oldRow, oldCol, newRow, newCol);
    graph.addEdge(newRow, newCol, oldRow, oldCol);

    // Update the piece's position
    piece->setRow(newRow);
    piece->setCol(newCol);
}

void ChessGraph::addNode(int row, int col, char pieceChar, Piece* piece) {
    auto coordinates = std::make_pair(row, col);
    if (nodes.find(coordinates) == nodes.end()) {
        GraphNode* newNode = new GraphNode(row, col, piece);
        nodes[coordinates] = newNode;

    }
}

void ChessGraph::addEdge(int row1, int col1, int row2, int col2) {
    auto it1 = nodes.find(std::make_pair(row1, col1));
    auto it2 = nodes.find(std::make_pair(row2, col2));

    if (it1 != nodes.end() && it2 != nodes.end()) {
        it1->second->neighbors.insert(it2->second);
        it2->second->neighbors.insert(it1->second);

    }
}
