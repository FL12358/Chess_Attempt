#include<iostream>
#include<vector>

using namespace std;

int BOARD_SIZE = 8;

enum Colour{
    empty = 0,
    white = -1,
    black = 1
};

struct Position{
    int x;
    int y;
};

class Piece {
    public:
    Position pos; // [a-h][1-8]
    char type;
    bool hasMoved;
    Colour colour;
    
    Piece(){
        hasMoved = false;
    }
    bool canMove(Position newPos){
        return false;
    }
};

struct Board{
    std::vector<Piece*> pieces;

};

class Rook: public Piece {
    public: 
    Rook(Position setPos, Colour pieceColour){
        colour = pieceColour;
        pos.x = setPos.x;
        pos.y = setPos.y;
        type = 'R';
        } 

    bool canMove(Position newPos){
        if(pos.x == newPos.x && pos.y == newPos.y) return false;    
        if(pos.x == newPos.x || pos.y == newPos.y) return true;
        return false;
    }
};



class Bishop: public Piece {
    public:
    Bishop(Position setPos, Colour pieceColour){
        pos.x = setPos.x;
        pos.y = setPos.y;
        type = 'B';
        colour = pieceColour;
    }
    bool canMove(Position newPos){
        if(pos.x == newPos.x && pos.y == newPos.y) return false;
        if(abs(pos.x-newPos.x) == abs(pos.y-newPos.y)) return true;
        return false;
    }
};

class Queen: public Piece {
    public:
    Queen(Position setPos, Colour pieceColour){
        pos.x = setPos.x;
        pos.y = setPos.y;
        type = 'Q';
        colour = pieceColour;
    }
    bool canMove(Position newPos){
        if(pos.x == newPos.x && pos.y == newPos.y) return false;
        if(abs(pos.x-newPos.x) == abs(pos.y-newPos.y)) return true;
        if(pos.x == newPos.x || pos.y == newPos.y) return true;
        return false;
    }
};

class Knight: public Piece {
    public:
    Knight(Position setPos, Colour pieceColour){
        pos.x = setPos.x;
        pos.y = setPos.y;
        type = 'N';
        colour = pieceColour;
    }
    bool canMove(Position newPos){
        if(abs(pos.x-newPos.x) == 1 && abs(pos.y-newPos.y) == 2) return true;
        if(abs(pos.x-newPos.x) == 2 && abs(pos.y-newPos.y) == 1) return true;
        return false;
    }
};

class Pawn: public Piece {
    public:
    Pawn(Position setPos, Colour pieceColour){
        pos.x = setPos.x;
        pos.y = setPos.y;
        type = 'P';
        colour = pieceColour;
        bool justDoubled = false;
    }
    bool canMove(Position newPos){
        int maxMove;
        if(hasMoved) maxMove = 1;
        if(!hasMoved) maxMove = 2;
        int move = newPos.y - pos.y;
        cout << move << endl;

        if(pos.x != newPos.x) return false;
        if(abs(move) <= maxMove && move > 0 && colour == white) return true;
        if(abs(move) <= maxMove && move < 0 && colour == black) return true;
        return false;
    }
};


void PrintHorLine(){
    cout << endl;
    for(int i=0;i<=4*BOARD_SIZE;i++){
        if(i%4){
            cout << "-";
        }else{
            cout << " ";
        }
    }
    cout << endl;
}

void PrintBoard(Board board){
    cout << "CHESS  |   pieces =  " << board.pieces.size() << endl;
    PrintHorLine();

    for(int i=BOARD_SIZE-1;i>=0;i--){
        for(int j=0;j<BOARD_SIZE;j++){
            if(j==0) cout << "|";
            std::string square = "   ";

            for(auto piece : board.pieces){
                if(piece->pos.x == j && piece->pos.y == i){
                    string pad = " ";
                    if(piece->colour == 1) pad = "*";
                    square = pad + piece->type + pad;
                }
            }
            cout << square << "|";
        }
        PrintHorLine();
    }
}

bool IsPosOnBoard(Position pos){
    if(pos.x < 8 && pos.x >= 0 && pos.y < 8 && pos.y >= 0) return true;
    return false;
}

bool IsPathClear(Position newPos, Piece piece, Board board){
    // check squres between piece.pos and newPos for pieces
    vector<Position> path;
    int xDir = piece.pos.x>newPos.x ? 1 : -1;
    int yDir = piece.pos.y>newPos.y ? 1 : -1;

    if(newPos.x == piece.pos.x || newPos.y == piece.pos.y){ //Rook
        if(newPos.x == piece.pos.x){ //up/down
            for(int i=1;i<abs(piece.pos.y-newPos.y);i++){
                for(auto p : board.pieces){
                    if(p->pos.y == piece.pos.y + yDir*i) return false;
                }
            }
        }else{  // left/right
            for(int i=1;i<abs(piece.pos.x-newPos.x);i++){
                for(auto p : board.pieces){
                    if(p->pos.x == piece.pos.x + xDir*i) return false;
                }
            }
        }
    }else{  // Bishop
        int xDir = piece.pos.x>newPos.x ? 1 : -1;
        int yDir = piece.pos.y>newPos.y ? 1 : -1;

        for(int i=1;i<abs(piece.pos.x-newPos.x);i++){
            for(auto p : board.pieces){
                if(p->pos.x == piece.pos.x + xDir*i && p->pos.y == piece.pos.y + yDir*i) 
                    return false;
            }
        }
    }
    return true;
}

int CheckTargetSquare(Position newPos, Piece piece, Board board){ 
    // checks target square for other pieces 
    // same colour: -1, empty: 0, other colour: 1
    for(auto p : board.pieces){
        if(newPos.x == p->pos.x && newPos.y == p->pos.y){
            if(piece.colour == p->colour){
                return -1;
            }else return 1;
        }
    }
    return 0;
}

Board PieceMove(Board board, Piece piece, Position newPos){
    
}

int main() { 
    Board board;
    Position egPos;



    egPos.x = 0;
    egPos.y = 5;
    Queen* queen1 = new Queen(egPos, black);
    board.pieces.push_back(queen1);

    
    PrintBoard(board);

    egPos.x = 5;
    egPos.y = 0;
    if(queen1->canMove(egPos)){
        cout << "valid move\n";
    }else{
        cout << "invalid move\n";
    }
    return 0;
}

