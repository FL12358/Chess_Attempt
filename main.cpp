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

    }
};

struct Board{
    std::vector<Piece*> pieces;
    int turn;
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

class King: public Piece {
    public:
    King(Position setPos, Colour pieceColour){
        pos.x = setPos.x;
        pos.y = setPos.y;
        type = 'K';
        colour = pieceColour;
    }
    bool canMove(Position newPos){
        if(pos.x == newPos.x && pos.y == newPos.y) return false;
        int changeX = abs(pos.x-newPos.x);
        int changeY = abs(pos.y-newPos.y);
        if((changeX == 0 || changeX == 1) && (changeY == 0 || changeY == 1)) return true;
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
    if(pos.x < BOARD_SIZE && pos.x >= 0 && pos.y < BOARD_SIZE && pos.y >= 0) return true;
    return false;
}

bool IsPathClear(Position newPos, Piece* piece, Board board){
    // check squres between piece.pos and newPos for all pieces
    if(piece->type == 'N') return true;
    vector<Position> path;
    int xDir = piece->pos.x>newPos.x ? -1 : 1;
    int yDir = piece->pos.y>newPos.y ? -1 : 1;
    
    if(newPos.x == piece->pos.x || newPos.y == piece->pos.y){ //Rook
        if(newPos.x == piece->pos.x){ //up/down
            for(int i=1;i<abs(piece->pos.y-newPos.y);i++){
                for(auto p : board.pieces){
                    if(p->pos.y == piece->pos.y + yDir*i) return false;
                }
            }
        }else{  // left/right
            for(int i=1;i<abs(piece->pos.x-newPos.x);i++){
                for(auto p : board.pieces){
                    if(p->pos.x == piece->pos.x + xDir*i) return false;
                }
            }
        }
    }else{  // Bishop
        for(int i=1;i<abs(piece->pos.x-newPos.x);i++){
            for(auto p : board.pieces){
                if(p->pos.x == piece->pos.x + xDir*i && p->pos.y == piece->pos.y + yDir*i) 
                    return false;
            }
        }
    }
    return true;
}

int CheckTargetSquare(Position newPos, Piece* piece, Board board){ 
    // checks target square for other pieces 
    // same colour: -1, empty: 0, other colour: 1
    for(auto p : board.pieces){
        if(newPos.x == p->pos.x && newPos.y == p->pos.y){
            if(piece->colour == p->colour){
                return -1;
            }else return 1;
        }
    }
    return 0;
}

Board PieceMove(Position newPos, Piece* piece, Board board){ // Simply moves a piece / return new board state
    for(auto i : board.pieces){
        if(piece->pos.x == i->pos.x && piece->pos.y == i->pos.y){
            i->pos.x = newPos.x;
            i->pos.y = newPos.y;
            i->hasMoved = true;
        }
    }
    board.turn++;
    return board;
}

Board PieceCapture(Position capPos, Board board){
    for(int i=0;i<board.pieces.size();i++){
        if(capPos.x == board.pieces[i]->pos.x && capPos.y == board.pieces[i]->pos.y){
            board.pieces.erase(board.pieces.begin()+i);
        }
    }
    return board;
}

Board AttemptMove(Position newPos, Piece* piece, Board board){
    if(IsPosOnBoard(newPos) && IsPathClear(newPos, piece, board) && piece->canMove(newPos)){
        switch(CheckTargetSquare(newPos, piece, board)){
            case -1:
                cout << "take own piece error\n";
                break;
            case 0:
                board = PieceMove(newPos, piece, board);
                break;
            case 1:
                board = PieceCapture(newPos, board);
                board = PieceMove(newPos, piece, board);
                break;
        }
    }else{
        cout << "Move not good\n";
    }
    return board;
}

int main() { 
    Board board;
    board.turn = 0;

    Position egPos;

    egPos.x = 3;
    egPos.y = 5;
    Rook* rook1 = new Rook(egPos, white);
    board.pieces.push_back(rook1);

    egPos.x = 0;
    egPos.y = 5;
    Queen* queen1 = new Queen(egPos, black);
    board.pieces.push_back(queen1);
    egPos.x = 3;
    egPos.y = 5;

    
    PrintBoard(board);
    board = AttemptMove(egPos, queen1, board);
    PrintBoard(board);
    egPos.y = 2;
    board = AttemptMove(egPos, queen1, board);
    PrintBoard(board);
    
    return 0;
}

