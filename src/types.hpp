#ifndef TYPES
#define TYPES

#include <cstdint>

typedef uint64_t Bitboard;

// Move encoding based on https://www.chessprogramming.org/Encoding_Moves#Information_Required
// <0-5> bit for from square
// <6-11> bit for to square
// <12-16> bit for move type
typedef uint16_t Move;

typedef enum MoveType {
    NORMAL, CAPTURE, CASTELING, EN_PASSANT, PROMOTION, PROMOTION_CAPTURE
} MoveType;

typedef enum PieceType {
    wPAWN, wBISHOP, wKNIGHT, wROOK, wQUEEN, wKING,
    bPAWN, bBISHOP, bKNIGHT, bROOK, bQUEEN, bKING, NO_PIECE
} PieceType;
PieceType operator++(PieceType& pieceType,int); // Overload the ++ operator for PieceType

typedef enum PieceColor {
    WHITE, BLACK
} PieceColor;
PieceColor operator++(PieceColor& color,int); // Overload the ++ operator for PieceType
PieceColor operator--(PieceColor& color,int); // Overload the -- operator for PieceType

typedef enum CastelingRights {
    WHITE_KINGSIDE, WHITE_QUEENSIDE, BLACK_KINGSIDE, BLACK_QUEENSIDE
} CastelingRights;


/*
    Class representing a move, makes it easier to work with moves
*/
class Cmove {
public:
        Cmove(int from, int to, MoveType type) {move = (from | (to << 6) | (type << 12));};

        Move getFrom()   const {return move & 0x3F;};
        Move getTo()     const {return (move >> 6) & 0x3F;};
        PieceType getCapturedPiece() const {return capturedPiece;};
        MoveType getType()   const {return static_cast<MoveType>((move >> 12) & 0x1F);};
        Move getMove()  const {return move;};

        void setFrom(int from) {move = (move & 0xFFC0) | from;};
        void setTo(int to) {move = (move & 0xFC3F) | (to << 6);};
        void setType(MoveType type) {move = (move & 0x83FF) | (type << 12);};
        void setCapturedPiece(PieceType piece) {capturedPiece = piece;};

        void operator=(const Cmove& other) {move = other.move;};
        bool operator==(const Cmove& other) const {return move == other.move;};

private:
        Move move;
        PieceType capturedPiece;

};





#endif