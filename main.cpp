#include<iostream>
#include<vector>
#include<sstream>
#include<cstring>

using namespace std;

int BOARD_SIZE = 8;
int DEBUG = 1;

void Couter(string str){
    if(DEBUG) cout << str << endl;
}

enum Colour{
    empty = -1,
    white = 0,
    black = 1
};

Colour OtherColour(Colour c){
    if(c == white) return black;
    if(c == black) return white;
    if(c == empty) return empty;
    return empty;
}

enum GameResult{
    notEnded = 0,
    whiteWin = 1,
    blackWin = 2,
    drawWin = 3
};

struct Position{
    int x;
    int y;
};

class Board;

class Piece {
    public:
    Position pos; // [a-h][1-8]
    char type;
    bool hasMoved = false;
    Colour colour;
    
    Piece(){
    }
    virtual bool canMove(Position newPos) = 0; // If piece can move to square if empty board

    private:
    bool canAttack(Position newPos, Board board);
};

Piece* PieceGenerator(char ch, Position pos);

class Board{
    public:
    std::vector<Piece*> pieces;
    int move; // number of moves
    int hmClock; // halfmove clock
    Position enPass;
    bool whiteToPlay;
    bool wKcast,wQcast,bKcast,bQcast;
    Colour check;

    Board(){
        move = 1;
    }
    Board CopyBoard(){
        Board retBoard;
        retBoard.bKcast = bKcast;
        retBoard.bQcast = bQcast;
        retBoard.wKcast = wKcast;
        retBoard.wQcast = wQcast;
        retBoard.check = check;
        retBoard.hmClock = hmClock;
        retBoard.move = move;
        retBoard.whiteToPlay = whiteToPlay;
        retBoard.pieces.clear();

        for(size_t i=0;i<pieces.size();i++){
            Piece* tempP = PieceGenerator(pieces[i]->type, pieces[i]->pos);
            tempP->colour = pieces[i]->colour;
            tempP->hasMoved = pieces[i]->hasMoved;
            retBoard.pieces.push_back(tempP);
        }
        return retBoard;
    }
};

class GameHistory{
    public:
    vector<Board> history;
    int currentMove = 0;

    void AddBoard(Board board){
        Board tempB = board.CopyBoard();
        if(!history.size()){
            history.push_back(tempB); // if next move in normal game add to end of 
        }else{
            history.resize(tempB.move+1);
            history.push_back(tempB);
        }
        currentMove = tempB.move;
    }
    Board Undo(){
        cout << currentMove;
        history[currentMove].whiteToPlay = !history[currentMove].whiteToPlay;
        return history[currentMove];
    }
};

int IsSquareAttacked(Position square, Colour attackedBy, Board board);

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
    bool justDoubled = false;
    bool canTakeL = false;
    bool canTakeR = false;
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
        if(abs(ymove) <= maxMove && ymove > 0 && colour == white && !xmove) return true;
        if(abs(ymove) <= maxMove && ymove < 0 && colour == black && !xmove) return true;
        return false;
    }

    bool canAttack(Position newPos, Board board){
        int yDir = colour==white ? 1 : -1;
        if(abs(pos.x-newPos.x)==1 && yDir==(newPos.y-pos.y)){
            for(auto p : board.pieces){
                if(p->pos.x == newPos.x && p->pos.y == newPos.y) return true;
                if(p->type=='P' && p->pos.x == newPos.x && p->pos.y == newPos.y-yDir){
                    Pawn* pTake = (Pawn*)p;
                    if(pTake->justDoubled) return true;
                }
            }
        }
        return false;
    }
};

Piece* PieceGenerator(char ch, Position pos){
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
            cout << "piece allocation error: " << ch << endl;
            exit(EXIT_FAILURE);
    }
}

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

vector<Position> InBetweenCoords(Position p1, Position p2){
    vector<Position> retVec;
    int xDir = p1.x>p2.x ? -1 : 1;
    int yDir = p1.y>p2.y ? -1 : 1;
    if(p1.x == p2.x) xDir = 0;
    if(p1.y == p2.y) yDir = 0;
    
    for(int i=1; i < max(abs(p1.x-p2.x),abs(p1.y-p2.y));i++){
        Position tempPos;
        tempPos.x = xDir*i+p1.x;
        tempPos.y = yDir*i+p1.y;
        retVec.push_back(tempPos);
    }
    return retVec;
}

bool IsPathClear(Position newPos, int pIdx, Board board){
    // check squres between piece.pos and newPos for all pieces
    Piece* piece = board.pieces[pIdx];

    if(piece->type == 'N') return true;
    vector<Position> path = InBetweenCoords(newPos, board.pieces[pIdx]->pos);

    for(auto sq : path){
        for(auto p : board.pieces){
            if(sq.x == p->pos.x && sq.y == p->pos.y){
                return false;
            }
        }
    }
    return true;
}

int CheckTargetSquare(Position newPos, int pIdx, Board board){ 
    // checks target square for other pieces 
    // same colour: -1, empty: 0, other colour: 1
    for(auto p : board.pieces){
        if(newPos.x == p->pos.x && newPos.y == p->pos.y){
            if(board.pieces[pIdx]->colour == p->colour){
                return -1;
            }else return 1;
        }
    }
    return 0;
}

Board PieceMove(Position newPos, int pIdx, Board board){ // Simply moves a piece / return new board state
    board.pieces[pIdx]->pos = newPos;
    return board;
}

Board PieceCapture(Position capPos, int pIdx, Board board){ // deletes a piece in certain position
    for(size_t i=0;i<board.pieces.size();i++){
        if(capPos.x == board.pieces[i]->pos.x && capPos.y == board.pieces[i]->pos.y && (int)i != pIdx){
            board.pieces.erase(board.pieces.begin()+i);
            break;
        }
    }
    return board;
}

Pawn* EnPassantFlags(Pawn* p1, Pawn* p2){
    if(p2->justDoubled && p2->colour != p1->colour && p2->pos.y == p1->pos.y){
        // check if p2 has just jumped -> go 
        if(p2->pos.x == p1->pos.x-1){
            p1->canTakeL = true;
            cout << "en pass L\n";
        }
        if(p2->pos.x == p1->pos.x+1){
            p1->canTakeR = true;
            cout << "en pass R\n";
        }
    }
    return p1;
}

Piece* SetPawnCaptureFlags(Pawn* pawn, Board board){
    int yMove = pawn->colour ? -1 : 1;

    for(auto i : board.pieces){ // for each piece on board
        if(i->colour == OtherColour(pawn->colour) && i->pos.y == pawn->pos.y + yMove){
            // check if pawn "i" is 1-diagonal to "pawn" -> set pawn.flag correctly
            if(i->pos.x == pawn->pos.x-1){
                pawn->canTakeL = true;
            }
            if(i->pos.x == pawn->pos.x+1){
                pawn->canTakeR = true;
            }
        }
        if(i->type == 'P'){
            pawn = EnPassantFlags(pawn, (Pawn*)i);
        }
    }
    return pawn;
}

Colour CheckChecker(Board board){
    Position wPos;
    Position bPos;
    for(auto p : board.pieces){
        if(p->type == 'K'){
            if(p->colour == white) wPos = p->pos;
            if(p->colour == black) bPos = p->pos;
        }
    }

    if(IsSquareAttacked(wPos, black, board)) return white;
    if(IsSquareAttacked(bPos, white, board)) return black;
    return empty;
}

bool DoesMoveCorrectCheck(Position newPos, int pIdx, Board board){
    bool retVal = true;
    Board tempB = board.CopyBoard();
    
    tempB = PieceMove(newPos, pIdx, tempB);
    tempB = PieceCapture(newPos, pIdx, tempB);
    tempB.check = CheckChecker(tempB);
    if(tempB.check == white && board.whiteToPlay){
        retVal = false;
    }
    if(tempB.check == black && !board.whiteToPlay){
        retVal = false;
    }
    return retVal;
}

bool IsMoveValid(Position newPos, int pIdx, Board board, bool realMove){
    if(board.pieces[pIdx]->type == 'P'){ // If moving a pawn
        SetPawnCaptureFlags((Pawn*)board.pieces[pIdx], board);
        for(auto p : board.pieces){
            if(p->pos.x == newPos.x && p->pos.y == newPos.y && board.pieces[pIdx]->pos.x == newPos.x){
                return false; // if path blocked
            }
        }
    }
    
    if(board.pieces[pIdx]->type == 'K'){ // If moving a king
        if(IsSquareAttacked(newPos, OtherColour(board.pieces[pIdx]->colour), board)){
            return false;
        }
    }
    bool check = realMove ? DoesMoveCorrectCheck(newPos, pIdx, board) : 1;
    if( IsPosOnBoard(newPos) && 
        IsPathClear(newPos, pIdx, board) && 
        board.pieces[pIdx]->canMove(newPos) && 
        check){
            switch(CheckTargetSquare(newPos, pIdx, board)){
            case -1:
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

GameResult IsCheckMate(Board board){
    Colour inCheck = CheckChecker(board);
    int kIdx = -1;
    vector<int> attIdx;
    if(inCheck == -1){
        return notEnded;
    }
    // determine king location
    for(size_t i=0;i< board.pieces.size();i++){
        if(board.pieces[i]->type == 'K' && inCheck == board.pieces[i]->colour){ // if p is checked king
            kIdx = i;
        }
    }
    
    King* king = (King*)board.pieces[kIdx];
    // determine attackers
    for(size_t i=0;i<board.pieces.size();i++){
        if(board.pieces[i]->colour != inCheck && IsMoveValid(king->pos, i, board, false)){ 
            // if other color and can attack kinge;
            attIdx.push_back(i);
        }
    }

    if(!attIdx.size()) return notEnded;
    for(int i=-1;i<=1;i++){ // if king can move out of check
        for(int j=-1;i<=1;i++){
            Position testPos;
            testPos.x = king->pos.x + i;
            testPos.y = king->pos.y + j;
            if(IsMoveValid(testPos, kIdx, board, false)){
                cout << "King can move" << endl;
                return notEnded;
            }
        }
    }

    if(attIdx.size() > 1){ // if king cant move & >1 attacker = checkmate
        return board.pieces[attIdx[0]]->colour == white ? whiteWin : blackWin;
    }

    for(size_t i=0;i<board.pieces.size();i++){ // check if piece can be captured
        if(board.pieces[i]->colour == inCheck && IsMoveValid(board.pieces[attIdx[0]]->pos, i, board, false)){
            cout << "Attacker can be captured" << endl;
            return notEnded;
        }
    }

    vector<Position> blockPos = InBetweenCoords(board.pieces[attIdx[0]]->pos, board.pieces[kIdx]->pos);
    for(Position pos : blockPos){
        for(size_t i=0;i<board.pieces.size();i++){ // check if piece can be captured
            if(board.pieces[i]->colour == inCheck && IsMoveValid(pos, i, board, false)){
                cout << "Attack can be blocked" << endl;
                return notEnded;
            }
        }
    }
    if(inCheck == white) return blackWin;
    if(inCheck == black) return whiteWin;
    return drawWin;
}

Pawn* SetPawnDoubleMove(Pawn* pawn, bool val){
    pawn->justDoubled = val;
    return pawn;
}

bool EnPassantMove(Position movePos, int pIdx, Board board){ // has pawn just moved enPass?
    Pawn* pawn = (Pawn*)board.pieces[pIdx];
    if(pawn->pos.x == movePos.x) return false;
    for(auto p : board.pieces){
        if(p->pos.x == movePos.x && p->pos.y == movePos.y){
            return false;
        }
    }
    return true;
}

bool CanCastle(Colour colour, bool kSide, Board board){
    Position rookPos;
    rookPos.x = kSide ? 7 : 0;
    rookPos.y = colour==white ? 0 : 7;
    King* king;
    Rook* rook;
    for(auto p : board.pieces){
        if(p->colour == colour){
            if(p->type == 'K') king = (King*)p;
            if(p->pos.x == rookPos.x && p->pos.y == rookPos.y) rook = (Rook*)p; 
        }
    }
    if(king->hasMoved || rook->hasMoved){
        Couter("Pieces have moved\n");
        cout << king->hasMoved << rook->hasMoved << endl;
        return false;
    }

    vector<Position> blockPos = InBetweenCoords(rook->pos, king->pos);
    for(auto p : board.pieces){
        for(auto pos : blockPos){
            if(p->pos.x == pos.x && p->pos.y == pos.y){
                cout << pos.x << pos.y << endl;
                return false;
            }
        }
    }
    return true;
}

Board CastleMove(Colour colour, bool kSide, Board board){
    Position rookPos;
    rookPos.x = kSide ? 7 : 0;
    rookPos.y = colour==white ? 0 : 7;
    int kIdx, rIdx;

    for(size_t i=0;i<board.pieces.size();i++){
        if(board.pieces[i]->colour == colour){
            if(board.pieces[i]->type == 'K') kIdx = i;
            if(board.pieces[i]->pos.x == rookPos.x && board.pieces[i]->pos.y == rookPos.y) rIdx = i; 
        }
    }

    int xDir = kSide ? 1 : -1;

    Position newKPos = board.pieces[kIdx]->pos;
    newKPos.x += 2*xDir;
    board = PieceMove(newKPos, kIdx, board);

    Position newRPos = board.pieces[kIdx]->pos; // NEW king pos
    newRPos.x -= xDir;
    board = PieceMove(newRPos, rIdx, board);

    board.move++;
    return board;
}

Board MakeMove(Position movePos, int pIdx, Board board){
    Position capPos;
    capPos.x = movePos.x;
    capPos.y = movePos.y;

    for(auto p : board.pieces){ // pieces no longer allowed to be captured en pass
        if(p->type == 'P'){
            p = SetPawnDoubleMove((Pawn*)p, false);
        }
    }
    if(board.pieces[pIdx]->type == 'P' && abs(movePos.y - board.pieces[pIdx]->pos.y) == 2){
        // If piece has made itself open to en passant
        board.pieces[pIdx] = SetPawnDoubleMove((Pawn*)board.pieces[pIdx], true);
    }

    if(board.pieces[pIdx]->type == 'P' && EnPassantMove(movePos, pIdx, board)){
        capPos.y += board.pieces[pIdx]->colour==white ? -1 : 1;
    }

    board.pieces[pIdx]->hasMoved = true;
    board.move++;
    board = PieceMove(movePos, pIdx, board); // pIdx loses meaning when num pieces changed
    board = PieceCapture(capPos, pIdx, board);
    
    return board;
}

int IsSquareAttacked(Position square, Colour attackedBy, Board board){ 
    // Returns number of attackers of Colour "attackedBy" aiming at an input square
    int retVal = 0;
    bool canMove;
    for(size_t i=0;i<board.pieces.size();i++){
        Piece* p = board.pieces[i];
        if(p->type == 'P'){ // Pawns have different move/attack patterns
            Pawn* pawn = (Pawn*)p;
            canMove = pawn->canAttack(square, board);
        }else{
            canMove = p->canMove(square);
        }

        if(IsPathClear(square, i, board) && canMove && p->colour == attackedBy){
            if(!(p->type == 'P' && square.x == p->pos.x)){
                ++retVal;
            }
        }
    }
    return retVal; 
}

string PosToNotation(Position pos){
    string retStr = "";
    retStr += (pos.x+97);
    retStr += (pos.y+49);
    return retStr;
}

Position NotationCoordToPos(string input){
    Position pos;
    if(input[0] > 90) input[0] -= 32;
    pos.x = input[0] - 65; //ascii conversion numbers
    pos.y = input[1] - 49;
    return pos;
}

char NotationToPiece(string input){
    if(input.size() == 2) return 'P';
    return toupper(input[0]);
}

Position NotationToPos(string input){
    string coord = input.substr(input.size()-2);
    return NotationCoordToPos(coord);
}

Board FENToBoard(string fen){
    Board board;
    size_t fenSplit = fen.find_first_of(" ");
    string temp = fen.substr(0, fenSplit);
    vector<string>rows;
    while(temp.size()){
        size_t index = temp.find("/");
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
        for(size_t j=0;j<rows[i].size();j++){
            if(isdigit(rows[i][j])){ // if number (1-8) found
                //skip that number of columns
                jCoord += rows[i][j] - 48 -1; // ascii to number
            }else{
                // Add piece to board
                Position pos;
                pos.x = jCoord;
                pos.y = BOARD_SIZE - i - 1;
                board.pieces.push_back(PieceGenerator(rows[i][j], pos));
            }
            jCoord++;
        }
    }
    string props = fen.substr(fenSplit); // properties setup
    vector<string> propsVec;
    while(props.size()){
        size_t index = props.find(" ");
        if(index != string::npos){
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

    board.enPass = NotationCoordToPos(propsVec[3]);

    board.hmClock = stoi(propsVec[4]);
    board.move = stoi(propsVec[5]);
    return board;
}


void SimplePlay(Board board){ // simple loop to take turns attempting moves
    int oldMoveNum = board.move;
    GameHistory history;
    history.AddBoard(board);
    GameResult result = notEnded;

    while(result == notEnded){
        PrintBoard(board);
        if(board.whiteToPlay){
            cout << board.move << " White: ";
        }else{
            cout << board.move << " Black: ";
        }

        string input;
        cin >> input;
        if(input == "undo"){
            board = history.Undo();
        }
        Colour whoTurn = board.whiteToPlay ? white : black;
        if(input == "OO"){
            if(CanCastle(whoTurn, true, board)) board = CastleMove(whoTurn, true, board);
        }
        if(input == "OOO"){
            if(CanCastle(whoTurn, false, board)) board = CastleMove(whoTurn, false, board);
        }
            
        Position pos = NotationToPos(input);
        char type = NotationToPiece(input);

        

        for(size_t i=0;i<board.pieces.size();i++){
            Piece* p = board.pieces[i];
            if(p->type == type && board.whiteToPlay != p->colour && IsMoveValid(pos, i, board, true)){

                cout << "Making move: \a" << PosToNotation(pos);
                board = MakeMove(pos, i, board);
                if(IsCheckMate(board) == whiteWin){
                    cout << "#";
                    result = whiteWin;
                }else if(IsCheckMate(board) == blackWin){
                    cout << "#";
                    result = blackWin;
                }else if(CheckChecker(board) != -1){
                    cout << "+";
                }
                cout << endl;
            }
        }
        if(board.move != oldMoveNum){
            oldMoveNum = board.move;
            history.AddBoard(board);
            board.whiteToPlay = !board.whiteToPlay;
        }else{
            cout << "Invalid Move" << endl;
        }

        board.check = CheckChecker(board);
        if(board.check == white) cout << "white in check\n";
        if(board.check == black) cout << "black in check\n";
    }
}

int main() {
    string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    startFEN = "2r3k1/p4p1p/4p1p1/Q2n4/8/8/Pq1BKPPP/7R w - - 0 2";

    SimplePlay(FENToBoard(startFEN));

    return 0;
}

