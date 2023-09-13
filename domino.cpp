//========================================================================//
//========================================================================//
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <deque>
using namespace std;

//========================================================================//
//========================================================================//

struct data_domino {
    int right, left, available;
};

//========================================================================//
//========================================================================//
class CRandom {
public:
    CRandom() {}  // constructor
    ~CRandom() {} // destructor

    int getRandomPublic(int rangeLow, int rangeHigh) {
        int myRand_scaled;

        myRand_scaled = getRandomPrivate(rangeLow, rangeHigh);

        return myRand_scaled;
    }
private:
    // Generates uniform distribution between rangeLow and rangeHigh
    int getRandomPrivate(int rangeLow, int rangeHigh) {
        double myRand = rand() / (1.0 + RAND_MAX);
        int range = rangeHigh - rangeLow + 1;
        int myRand_scaled = (myRand * range) + rangeLow;
        return myRand_scaled;
    }
protected:
    // Generates uniform distribution between rangeLow and rangeHigh
    int getRandomProtected(int rangeLow, int rangeHigh) {
        double myRand = rand() / (1.0 + RAND_MAX);
        int range = rangeHigh - rangeLow + 1;
        int myRand_scaled = (myRand * range) + rangeLow;
        return myRand_scaled;
    }

};

//========================================================================//
//========================================================================//
class CDomino {
public:
    CDomino() {}  // constructor
    ~CDomino() {} // destructor
    deque<data_domino> myDomino;

    void API(void) {
        init();
    }
    data_domino getPiece(int pieceID) {
        data_domino mypiece = myDomino.at(pieceID);
        cout << "[" << mypiece.left << "|" << mypiece.right << "]"
            << " available = " << mypiece.available << endl;

        myDomino.at(pieceID) = mypiece;
        return (mypiece);
    }


private:
    void init(void) {
        data_domino mypiece;
        for (int right = 0; right < 7; right++) {
            for (int left = right; left < 7; left++) {
                mypiece.right = right;
                mypiece.left = left;
                mypiece.available = 1;
                cout << "[" << mypiece.left << "|" << mypiece.right << "]" << "  ";
                myDomino.push_back(mypiece);
            }
            cout << endl;
        }
        cout << "myDomino stores " << (int)myDomino.size() << " pieces.\n";
    }
};

//========================================================================//
//========================================================================//
class CPlayer : public CRandom {
private:
// Initializing with an invalid value
    int tableHead = -1; 
    int tableTail = -1;
public:
    CPlayer() {}  // constructor
    ~CPlayer() {} // destructor
    deque<data_domino> gotHand;
    class CDomino* player_pDominoOBJ = NULL;

    int takePiece(int pieceNo) {
        int counter = 0;
        int number_wasAvailable = 0;

        data_domino takenPiece;
        takenPiece = player_pDominoOBJ->getPiece(pieceNo);

        if (takenPiece.available) {
            number_wasAvailable = takenPiece.available;
            takenPiece.available = 0; // no longer available from the dominoes pile
            player_pDominoOBJ->myDomino.at(pieceNo) = takenPiece;
            cout << " [" << takenPiece.left
                << "|" << takenPiece.right << "]"
                << " just taken - no longer available from pile = "
                << takenPiece.available << endl;
            takenPiece.available = 1; // available on Player's hand
            gotHand.push_back(takenPiece);

            if (matchPieceToTable(takenPiece)) {
                return number_wasAvailable;
            }

        }
        else {
            cout << "NOT AVAILABLE" << endl;
        }
        return (number_wasAvailable);
    }

    bool playPiece(int pieceNo);
    bool playPieceFromHand(int pieceNo);
    int getRandomAvailablePiece();
    bool matchPieceToTable(data_domino piece);
    int takePiecesUntilMatch();

    // PASSING OBJECT AS POINTER - FOR DIFFERENT CLASSES INTERFACE
    void API(CDomino* receive_dominoPointerOBJ) {
        player_pDominoOBJ = receive_dominoPointerOBJ;
    }
};

bool CPlayer::playPiece(int pieceNo) {
    int counter = 0;
    int number_wasAvailable = 0;

    data_domino playedPiece;
    playedPiece = player_pDominoOBJ->getPiece(pieceNo);

    if (playedPiece.available) {
        number_wasAvailable = playedPiece.available;
        playedPiece.available = 0; // no longer available from the dominoes pile
        player_pDominoOBJ->myDomino.at(pieceNo) = playedPiece;
        cout << "[" << playedPiece.left << "|" << playedPiece.right << "]"
            << " just played - no longer available from pile = "
            << playedPiece.available << endl;

        if (matchPieceToTable(playedPiece)) {
            // Update the head or tail of the table based on the played piece
            if (playedPiece.left == tableHead) {
                tableHead = playedPiece.right;
            }
            else if (playedPiece.right == tableHead) {
                tableHead = playedPiece.left;
            }
            else if (playedPiece.left == tableTail) {
                tableTail = playedPiece.right;
            }
            else if (playedPiece.right == tableTail) {
                tableTail = playedPiece.left;
            }
            return true; // Successful match and update
        }
    }
    else {
        cout << "NOT AVAILABLE" << endl;
    }
    return false; // Unsuccessful match
}

bool CPlayer::playPieceFromHand(int pieceNo) {
    data_domino playedPiece;
    playedPiece = gotHand.at(pieceNo);

    // Check if the piece can be matched to the head or tail of the table
    if (matchPieceToTable(playedPiece)) {
        // update the head/tail of the table based on the piece played
        if (playedPiece.left == tableHead) {
            tableHead = playedPiece.right;
        }
        else if (playedPiece.right == tableHead) {
            tableHead = playedPiece.left;
        }
        else if (playedPiece.left == tableTail) {
            tableTail = playedPiece.right;
        }
        else if (playedPiece.right == tableTail) {
            tableTail = playedPiece.left;
        }

        // Remove the piece played from the player's hand
        gotHand.erase(gotHand.begin() + pieceNo);

        return true; // successful move
    }

    return false; // unsuccessful move
}

int CPlayer::getRandomAvailablePiece() {
    int pieceNo = getRandomPublic(0, gotHand.size() - 1);
    data_domino piece = gotHand.at(pieceNo);
    while (!piece.available) {
        pieceNo = getRandomPublic(0, gotHand.size() - 1);
        piece = gotHand.at(pieceNo);
    }
    return pieceNo;
}

bool CPlayer::matchPieceToTable(data_domino piece) {
    // Check if the piece can be matched to the head of the dominoes match
    if (piece.left == tableHead || piece.right == tableHead) {
        cout << "Matched piece " << "[" << piece.left << "|" << piece.right << "] to the head." << endl;
        return true;
    }
    // Check if the piece can be matched to the tail of the dominoes match
    else if (piece.left == tableTail || piece.right == tableTail) {
        cout << "Matched piece " << "[" << piece.left << "|" << piece.right << "] to the tail." << endl;
        return true;
    }
    return false; // The piece doesn't match either head or tail
}

int CPlayer::takePiecesUntilMatch() {
    int pieceNo;
    while (true) {
        pieceNo = getRandomAvailablePiece();
        cout << "Taking another piece: " << pieceNo << endl;
        int piece_wasAvailable = takePiece(pieceNo);
        if (piece_wasAvailable) {
            data_domino piece = gotHand.at(pieceNo);
            if (matchPieceToTable(piece)) {
                // Player successfully matched the piece, continue the game
                return piece_wasAvailable;
            }
        }
    }
}

//========================================================================//
//========================================================================//
class CTable {
public:
    CTable() {}  // constructor
    ~CTable() {} // destructor
    class CPlayer* playerOBJ = NULL;

    void selecting_pieces() {
        int pieceNO, piece_wasAvailable, totalPlayer = 2;
        cout << "selecting pieces and giving 10 pieces to each player" << endl;

        for (int playerID = 0; playerID < totalPlayer; playerID++) {
            for (int i = 0; i < 10; i++) {
                pieceNO = playerOBJ[playerID].getRandomPublic(0, 27);
                cout << " pieceNO : " << pieceNO << endl;
                piece_wasAvailable = playerOBJ[playerID].takePiece(pieceNO);

                if (piece_wasAvailable) {
                    cout << "piece available" << endl;
                }
                else {
                    cout << "////////////////////////////////////////////////" << endl;
                    cout << "piece not available - try to take a piece again" << endl;
                    i--;
                }
            }
        }
    }

    void showPlayerHand(void) {
        data_domino showpiece;
        int totalPlayer = 2;
        for (int playerID = 0; playerID < totalPlayer; playerID++) {
            cout << "Player " << playerID + 1 << " stores " <<
                playerOBJ[playerID].gotHand.size() << " pieces.\n";
            for (int pieceNo = 0; pieceNo < playerOBJ[playerID].gotHand.size(); pieceNo++) {
                showpiece = playerOBJ[playerID].gotHand.at(pieceNo);
                cout << "[" << showpiece.left << "|" << showpiece.right << "]"
                    << " available = " << showpiece.available << endl;
            }
        }
    }

    void API(CPlayer* receive_playersOBJ) {
        playerOBJ = receive_playersOBJ;

        selecting_pieces();
        showPlayerHand();
    }

}; // END CTable

bool winner(CPlayer& player) {
    return player.gotHand.empty();
}

//========================================================================//
//========================================================================//
int main(void) {

    struct timeval time;
    int pieceID;

    gettimeofday(&time, NULL);
    srand((unsigned int)time.tv_usec);

    CDomino dominoOBJ, * dominoPointer;
    dominoOBJ.API();
    dominoPointer = &dominoOBJ;

 
    CPlayer* playerOBJ;
    playerOBJ = new CPlayer[2];
    playerOBJ[0].API(dominoPointer);
    playerOBJ[1].API(dominoPointer);

    CTable myTableObj;
    myTableObj.API(playerOBJ);

    cout << "check pointer effect on dominoOBJ" << endl;
    for (pieceID = 0; pieceID < 28; pieceID++) {
        dominoOBJ.getPiece(pieceID);
    }

    int maxRounds = 10;
    int currentRound = 0;
    bool gameOver = false;
    int currentPlayer = rand() % 2;
    int maxPieces = 28;

    // ASCII Art for Domino Pieces
    const char* dominoAscii[7][7] = {
        { "[ ][ ]" },
        { " [1|1] ", " [1][]"},
        { " [2][2] ", " [2][1] ", "[2][]" },
        { " [3][3] ", " [3][2] ", " [3][1] ", "[3][]" },
        { " [4][4] ", " [4][3] ", " [4|2] ", "[4][1]","[4][]" },
        { "[5][5] ", " [5][4] ","[5][3] ", " [5] [2] ", "[5][1]", "[5][]"  },
        { "[6|6]", "[6][5]","[6][4]","[6][3]", "[6][2]","[6][1]","[6][]" }
    };

    while (!gameOver && currentRound < maxRounds) {
        int pieceToPlay = playerOBJ[currentPlayer].getRandomAvailablePiece();

        if (playerOBJ[currentPlayer].playPieceFromHand(pieceToPlay)) {
            cout << "Player " << currentPlayer + 1 << " played piece " <<
                dominoAscii[playerOBJ[currentPlayer].gotHand[pieceToPlay].left][playerOBJ[currentPlayer].gotHand[pieceToPlay].right] << endl;

            // Check if the player has won
            if (winner(playerOBJ[currentPlayer])) {
                cout << "Player " << currentPlayer + 1 << " has won!" << endl;
                gameOver = true;
            }

            // Switch to the next player
            currentPlayer = (currentPlayer + 1) % 2;
        }
        else {
            cout << "Player " << currentPlayer + 1 << " cannot play a piece. Skipping turn." << endl;
            currentPlayer = (currentPlayer + 1) % 2;
        }

        // Check if there are no more available pieces in the domino set
        int totalAvailablePieces = 0;
        for (int i = 0; i < maxPieces; i++) {
            if (dominoOBJ.myDomino[i].available) {
                totalAvailablePieces++;
            }
        }

        if (totalAvailablePieces == 0) {
            gameOver = true;
        }

        currentRound++;
    }

    if (!gameOver) {
        cout << "Game ended after " << maxRounds << " rounds." << endl;
        int secondPlayer = (currentPlayer + 1) % 2;
        cout << "Player " << currentPlayer + 1 << " has won!" << endl;
        cout << "Player " << secondPlayer + 1 << " has " << playerOBJ[secondPlayer].gotHand.size() << " pieces left:" << endl;
        for (const data_domino& piece : playerOBJ[secondPlayer].gotHand) {
            cout << dominoAscii[piece.left][piece.right] << " ";
        }
        cout << endl;

        cout << "Remaining dominoes on the table:" << endl;
        for (int i = 0; i < maxPieces; i++) {
            if (dominoOBJ.myDomino[i].available) {
                cout << dominoAscii[dominoOBJ.myDomino[i].left][dominoOBJ.myDomino[i].right] << " ";
            }
        }
        cout << endl;
    }

    delete[] playerOBJ;
    return EXIT_SUCCESS;
}

//========================================================================//
//=====================================================================