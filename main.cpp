#include<iostream>
#include<vector>
#include<sstream>

using namespace std;

int BOARD_SIZE = 8;

enum Colour{
    empty = -1,
    white = 0,
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
    virtual bool canMove(Position newPos) = 0; // If peice can move to square if empty board
};

struct Board{
    std::vector<Piece*> pieces;
    int move; // number of moves
    int hmClock; // halfmove clock
    Position enPass;
    bool whiteToPlay;
    bool wKcast;
    bool wQcast;
    bool bKcast;
    bool bQcast;

};

int IsSquareAttacked(Position square, Colour attackedBy, Board board);

Colour OtherColour(Colour c){
    if(c == white) return black;
    if(c == black) return white;
    if(c == empty) return empty;
}

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
    bool inCheck(Board board){
        for(auto p : board.pieces){
            return IsSquareAttacked(pos, OtherColour(colour), board);
        }
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
    bool justDoubled;
    bool canTakeL;
    bool canTakeR;
    Pawn(Position setPos, Colour pieceColour){
        pos.x = setPos.x;
        pos.y = setPos.y;
        type = 'P';
        colour = pieceColour;
        justDoubled = false;
        canTakeL = false;
        canTakeR = false;
    }
    bool canMove(Position newPos){
        int maxMove = hasMoved ? 1 : 2;
        int ymove = newPos.y - pos.y;
        int xmove = newPos.x - pos.x;
        if(pos.x != newPos.x && !(canTakeL || canTakeR)) return false;
        if(canTakeL && xmove == -1 && abs(ymove) == 1) return true; // diagonal captures
        if(canTakeR && xmove == 1 && abs(ymove) == 1) return true;
        if(abs(ymove) <= maxMove && ymove > 0 && colour == white && !xmove) return true;    // forwards moves
        if(abs(ymove) <= maxMove && ymove < 0 && colour == black && !xmove) return true;
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

void PrintHorFiles(){
    cout << "  a   b   c   d   e   f   g   h  "<< endl;
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
            if(j == 7) cout << " " << i+1;
        }
        PrintHorLine();
    }
    PrintHorFiles();
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
                    if(p->pos.y == piece->pos.y + yDir*i && p->pos.x == piece->pos.x) return false;
                }
            }
        }else{  // left/right
            for(int i=1;i<abs(piece->pos.x-newPos.x);i++){
                for(auto p : board.pieces){
                    if(p->pos.x == piece->pos.x + xDir*i && p->pos.y == piece->pos.y) return false;
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
    board.move++;
    return board;
}

Board PieceCapture(Position capPos, Board board){ // deletes a piece in certain position
    for(int i=0;i<board.pieces.size();i++){
        if(capPos.x == board.pieces[i]->pos.x && capPos.y == board.pieces[i]->pos.y){
            board.pieces.erase(board.pieces.begin()+i);
        }
    }
    return board;
}

Piece* SetPawnCaptureFlags(Pawn* pawn, Board board){
    int yMove = pawn->colour ? -1 : 1;
    bool lMove = false;
    bool rMove = false;
    for(auto i : board.pieces){ // for each piece on board
        if(i->colour == OtherColour(pawn->colour) && i->pos.y == pawn->pos.y + yMove){
            // check if pawn "i" is 1-diagonal to "pawn" -> set pawn.flag correctly
            if(i->pos.x == pawn->pos.x-1){
                lMove = true;
            }
            if(i->pos.x == pawn->pos.x+1){
                rMove = true;
            }
        }
    }
    pawn->canTakeL = lMove;
    pawn->canTakeR = rMove;
    return pawn;
}

bool IsMoveValid(Position newPos, Piece* piece, Board board){
    if(piece->type == 'P'){ // If moving a pawn
        piece = SetPawnCaptureFlags((Pawn*)piece, board);
        for(auto p : board.pieces){
            if(p->pos.x == newPos.x && p->pos.y == newPos.y && piece->pos.x == newPos.x) return false;
        }
    }

    if(IsPosOnBoard(newPos) && IsPathClear(newPos, piece, board) && piece->canMove(newPos)){
        switch(CheckTargetSquare(newPos, piece, board)){
            case -1:
                cout << "take own piece error\n";
                return false;
                break;
            case 0:
                return true;
                break;
            case 1:
                return true;
                break;
        }
    }
    return false;
}

Board AttemptMove(Position newPos, Piece* piece, Board board){
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
    return board;
}

int IsSquareAttacked(Position square, Colour attackedBy, Board board){ 
    // Returns number of attackers of Colour "attackedBy" aiming at an input square
    int retVal = 0;
    for(auto p : board.pieces){
        if(IsPathClear(square, p, board) && p->canMove(square) && p->colour == attackedBy){
            ++retVal;
        }
    }
    return retVal; 
}

Position NotationCoordToPos(string input){
    Position pos;
    if(input[0] > 90) input[0] -= 32;
    pos.x = input[0] - 65;
    pos.y = input[1] - 49;

    return pos;
}

char NotationPeice(string input){
    if(input.size() == 2) return 'P';
    return toupper(input[0]);
}

Position NotationToPos(string input){
    string coord = input.substr(input.size()-2);
    return NotationCoordToPos(coord);
}

Piece* FENCharToPiece(char ch, Position pos){
    Colour colour = white;
    if(ch > 90){
        ch -= 32;
        colour = black;
    }
    switch (ch){
        case 'R':{
            return new Rook(pos, colour);
            break;
            }
        case 'N':{
            return new Knight(pos, colour);
            break;
            }
        case 'B':{
            return new Bishop(pos, colour);
            break;
            }
        case 'Q':{
            return new Queen(pos, colour);
            break;
            }
        case 'K':{
            return new King(pos, colour);
            break;
            }
        case 'P':{
            return new Pawn(pos, colour);
            break;
            }
        default:
            cout << "FEN piece error: " << ch << endl;
            exit(EXIT_FAILURE);
    }
}

Board FENToBoard(string fen){
    Board board;
    int fenSplit = fen.find_first_of(" ");
    string temp = fen.substr(0, fenSplit);
    vector<string>rows;

    while(temp.size()){
        int index = temp.find("/");
        if(index!=string::npos){
            rows.push_back(temp.substr(0,index));
            temp = temp.substr(index+1);
            if(temp.size()==0)rows.push_back(temp);
        }else{
            rows.push_back(temp);
            temp = "";
        }
    }
    
    for(int i=rows.size()-1;i>=0;i--){ // Piece setup
        int jCoord = 0;
        for(int j=0;j<rows[i].size();j++){
            
            if(isdigit(rows[i][j])){ // if number (1-8) found
                jCoord += rows[i][j] - 48 -1; // ascii to number
            }else{
                Position pos;
                pos.x = jCoord;
                pos.y = BOARD_SIZE - i - 1;
                board.pieces.push_back(FENCharToPiece(rows[i][j], pos));
            }
            jCoord++;
        }
    }

    string props = fen.substr(fenSplit);
    vector<string> propsVec;
    while(props.size()){
        int index = props.find(" ");
        if(index!=string::npos){
            propsVec.push_back(props.substr(0,index));
            props = props.substr(index+1);
            if(props.size()==0)propsVec.push_back(props);
        }else{
            propsVec.push_back(props);
            props = "";
        }
    }
    board.whiteToPlay = (propsVec[1][0] == 'w');

    board.wKcast = false;
    board.wQcast = false;
    board.bKcast = false;
    board.bQcast = false;
    for(char c : propsVec[2]){
        if(c == 'K') board.wKcast = true;
        if(c == 'Q') board.wQcast = true;
        if(c == 'k') board.bKcast = true;
        if(c == 'q') board.bQcast = true;
    }

    // board.enPass = algebraToPos(propsVec[3]);

    board.hmClock = stoi(propsVec[4]);
    board.move = stoi(propsVec[5]);
    
    return board;
}

void SimplePlay(Board board){ // simple loop to take turns attempting moves
    int oldMoveNum = board.move;
    while(1){
        PrintBoard(board);
        if(board.whiteToPlay){
            cout << "White: ";
            
        }else{
            cout << "Black: ";
        }
        

        string input;
        cin >> input;
        Position pos = NotationToPos(input);
        char type = NotationPeice(input);
        for(auto i : board.pieces){
            if(i->type == type && board.whiteToPlay != i->colour && IsMoveValid(pos, i, board)){
                board = AttemptMove(pos, i, board);
            }
        }
        if(board.move != oldMoveNum){
            cout << "Move " << board.move << " sucess\n";
            board.whiteToPlay = !board.whiteToPlay;
            oldMoveNum = board.move;
            // Check for Check
        }
    }
}

int main() { 
    Board board;
    board.move = 1;
    string startFEN;
    startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    //startFEN = "3p4/8/2Q5/8/8/5n2/8/4P3 w KQkq - 0 2";
    board = FENToBoard(startFEN);
    SimplePlay(board);

    PrintBoard(board);

    Position egPos;
    

    return 0;
}

