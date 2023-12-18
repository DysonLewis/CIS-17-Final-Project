#ifndef Piece_h
#define Piece_h

#include <stdio.h>
#include <string>

class Piece
{
public:
        
    Piece(char t, int r, int c);
    virtual ~Piece();
    
    // accessors
    bool isCaptured();
    char getType();
    int getRow();
    int getCol();
    bool getHasMoved();
    
    // mutators
    void setCaptured(bool b);
    void setType(char c);
    void setRow(int r);
    void setCol(int c);
    void setHasMoved(bool moved);
    std::string getColor() const {
        return (isupper(type) ? "white" : "black");
    }
    
private:
    bool captured;
    char type;
    int row;
    int col;
    bool hasMoved;
};

#endif /* Piece_h */