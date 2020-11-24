#include<iostream>
#include<vector>

using namespace std;

int BOARD_SIZE = 8;


class Piece {
    public:
        int pos[2]; // [a-h][1-8]
        char type;
        bool hasMoved;
        bool isWhite;
        std::vector<int[2]> moveSet;
        bool repMoves; // if piece can move multiple values of "moveSet"
        
        Piece(){
            hasMoved = false;
        }
        bool canMove(int x, int y){
            for(auto i : moveSet){
                if((x-pos[0] == i[0]) && (y-pos[1] == i[1])){
                    return true;
                }
            }
            return false;
        }
};

struct Board{
    std::vector<Piece*> pieces;

};

class Rook: public Piece {
    public: 
        Rook(int x, int y, bool colour){
            isWhite = colour;
            pos[0] = x;
            pos[1] = y;
            type = 'R';
            repMoves = true;
        } 
        bool canMove(int x, int y){
            if(pos[0] == x && pos[1] == y) return false;
                
            if(pos[0] == x || pos[1] == y){
                return true;
            }
            return false;
        }
};



class Bishop: public Piece {
    public:
        Bishop(int x, int y, bool colour){
            pos[0] = x;
            pos[1] = y;
            type = 'B';
            repMoves = true;
            isWhite = colour;
        }
        bool canMove(int x, int y){
            if(pos[0] == x && pos[1] == y) return false;

            if(abs(pos[0]-x) == abs(pos[1]-y)){
                return true;
            }
            return false;
        }
};

void PrintHorLine(){
    cout << endl;
    for(int i=0;i<=4*BOARD_SIZE;i++){
        cout << "-";
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
                if(piece->pos[0] == i && piece->pos[1] == j){
                    string pad = " ";
                    if(!piece->isWhite) pad = "*";
                    square = pad + piece->type + pad;
                }
            }
            cout << square << "|";
        }
        PrintHorLine();
    }

}


int main() { 
    std::cout<<"Hello World" << std::endl; 
    Board board;
    Rook* rook1 = new Rook(1,2, false);
    Bishop* bish1 = new Bishop(4,4,true);
    board.pieces.push_back(rook1);
    board.pieces.push_back(bish1);
    cout << bish1->isWhite << " " << rook1->isWhite << endl;
    PrintBoard(board);

    if(rook1->canMove(2,1)){
        cout << "valid move\n";
    }else{
        cout << "invalid move\n";
    }
    return 0; 
} 

