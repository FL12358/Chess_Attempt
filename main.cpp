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
};

struct Board{
    std::vector<Piece*> pieces;

};

class Rook: public Piece {
    public: 
        Rook(int x, int y){
            pos[0] = x;
            pos[1] = y;
            type = 'R';
            repMoves = true;
            
        }
};

class Bishop: public Piece {
    public:
        Bishop(int x, int y){
            pos[0] = x;
            pos[1] = y;
            type = 'B';
            repMoves = true;
            //moveSet = {{-1,-1},{-1,1},{1,-1},{1,1}};
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
    cout << "CHESS  num: " << board.pieces.size() << endl;
    PrintHorLine();

    for(int i=0;i<BOARD_SIZE;i++){
        for(int j=0;j<BOARD_SIZE;j++){
            if(j==0) cout << "|";

            for(auto piece : board.pieces){
                if(piece->pos[0] == i && piece->pos[1] == j){
                    cout << " " << piece->type << " ";
                }else{
                    cout << "   ";
                }
            }
            cout << "|";
        }
        PrintHorLine();
    }

}


int main() { 
    std::cout<<"Hello World" << std::endl; 
    Board board;
    new Rook rook1(1,2);
    board.pieces.push_back(rook1);
    PrintBoard(board);
    return 0; 
} 

