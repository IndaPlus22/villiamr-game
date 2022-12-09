#include "position.hpp"

/*
    PAWN MOVES
*/

template <PieceColor color>
Bitboard pawnSinglePush (Bitboard pieces, Bitboard occupied) {
    Bitboard moves = 0;
    moves = color == WHITE ? pieces >> 8 : pieces << 8;
    moves &= ~occupied;
    return moves;
}


template <PieceColor color>
Bitboard pawnAttacksEast (Bitboard pieces, Bitboard enemy) {
    Bitboard moves = 0;
    pieces &= ~FILE_H;
    moves = color == WHITE ? pieces >> 7 : pieces << 9;
    moves &= enemy;
    return moves;
}

template <PieceColor color>
Bitboard pawnAttacksWest (Bitboard pieces, Bitboard enemy) {
    Bitboard moves = 0;
    pieces &= ~FILE_A;
    moves = color == WHITE ? pieces >> 9 : pieces << 7;
    moves &= enemy;
    return moves;
}

std::vector<Cmove> Position::pawnMoves(){ //TODO : FUNCTION IS NOT FINISHED ORIGIN SQUARES ARE NOT CORRECT
    std::vector<Cmove> moves;
    Bitboard singlePush = 0;
    Bitboard dubblePush = 0;
    Bitboard attacksEast = 0;
    Bitboard attacksWest = 0;

    // Generate pawn moves
    if ( sideToMove == WHITE){
        singlePush = pawnSinglePush<WHITE>(pieceBitboards[wPAWN], allBitboard);
        dubblePush = pawnSinglePush<WHITE>(singlePush, allBitboard);
        attacksEast = pawnAttacksEast<WHITE>(pieceBitboards[wPAWN], colorBitboards[BLACK]);
        attacksWest = pawnAttacksWest<WHITE>(pieceBitboards[wPAWN], colorBitboards[BLACK]);
    }
    else{
        singlePush = pawnSinglePush<BLACK>(pieceBitboards[bPAWN], allBitboard);
        dubblePush = pawnSinglePush<BLACK>(singlePush, allBitboard);
        attacksEast = pawnAttacksEast<BLACK>(pieceBitboards[bPAWN], colorBitboards[WHITE]);
        attacksWest = pawnAttacksWest<BLACK>(pieceBitboards[bPAWN], colorBitboards[WHITE]);
    }

    if ( enPassantSquare != -1){ 
        Bitboard enPassant = 1ULL << enPassantSquare; 
        if ( getPiceceAtSquare(enPassantSquare + 7) == wPAWN || getPiceceAtSquare(enPassantSquare - 7) == bPAWN){
            moves.push_back(Cmove(sideToMove == WHITE ? enPassantSquare + 7 : enPassantSquare - 7, enPassantSquare, EN_PASSANT));
        }
        if(getPiceceAtSquare(enPassantSquare + 9) == wPAWN || getPiceceAtSquare(enPassantSquare - 9) == bPAWN){
            moves.push_back(Cmove(sideToMove == WHITE ? enPassantSquare + 9 : enPassantSquare - 9, enPassantSquare, EN_PASSANT));
        }
    }

    attackboard |= attacksEast | attacksWest;

    Bitboard originSQ = 0;
    int from;
    int to;

    // Generate single push moves       NOTE: THIS WORKS
    if (sideToMove == WHITE){
        originSQ = pawnSinglePush<BLACK>(singlePush, 0);
    }else {
        originSQ = pawnSinglePush<WHITE>(singlePush, 0);
    }
    while (singlePush && originSQ){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(singlePush);
        singlePush ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, sideToMove == WHITE ? (to < 8 ? PROMOTION : NORMAL) : (to > 55 ? PROMOTION : NORMAL)));
    }

    // Generate dubble push moves
    originSQ = 0;
    if (sideToMove == WHITE){ // NOTE: THIS WORKS
        originSQ = dubblePush << 16; // UGLY ASF but i have no time
        originSQ &= RANK_7;
    }else {
        originSQ = dubblePush >> 16;
        originSQ &= RANK_2;
    }
    while (originSQ && dubblePush){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(dubblePush);
        dubblePush ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, NORMAL));
    }

    // Generate attacks east moves
    originSQ = 0;
    if (sideToMove == WHITE){
        originSQ = attacksEast << 7; // NOTE: PROBLEM IS HERE
    }else {
        originSQ = attacksEast >> 9;
    }
    while (originSQ && attacksEast){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(attacksEast);
        attacksEast ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, sideToMove == WHITE ? (to < 8 ? PROMOTION_CAPTURE : CAPTURE) : (to > 55 ? PROMOTION_CAPTURE : CAPTURE)));
    }

    // Generate attacks west moves
    originSQ = 0;
    if (sideToMove == WHITE){
        originSQ = attacksWest << 9; // NOTE: PROBLEM IS HERE
    }else {
        originSQ = attacksWest >> 7;
    }
    while (originSQ && attacksWest){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(attacksWest);
        attacksWest ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, sideToMove == WHITE ? (to < 8 ? PROMOTION_CAPTURE : CAPTURE) : (to > 55 ? PROMOTION_CAPTURE : CAPTURE)));
    }

    return moves;
}

/* 
    KNIGTS MOVES
*/
const Bitboard KNIGHT_ATTACKS[64] = {
  0x20400, 0x50800, 0xA1100, 0x142200, 0x284400, 0x508800, 0xA01000, 0x402000,
  0x2040004, 0x5080008, 0xA110011, 0x14220022, 0x28440044, 0x50880088, 0xA0100010, 0x40200020,
  0x204000402, 0x508000805, 0xA1100110A, 0x1422002214, 0x2844004428, 0x5088008850, 0xA0100010A0, 0x4020002040,
  0x20400040200, 0x50800080500, 0xA1100110A00, 0x142200221400, 0x284400442800, 0x508800885000, 0xA0100010A000, 0x402000204000,
  0x2040004020000, 0x5080008050000, 0xA1100110A0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xA0100010A00000, 0x40200020400000,
  0x204000402000000, 0x508000805000000, 0xA1100110A000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xA0100010A0000000, 0x4020002040000000,
  0x400040200000000, 0x800080500000000, 0x1100110A00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010A000000000, 0x2000204000000000,
  0x4020000000000, 0x8050000000000, 0x110A0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10A00000000000, 0x20400000000000
};

std::vector<Cmove> Position::knightMoves(){
    std::vector<Cmove> moves;
    Bitboard knights = pieceBitboards[sideToMove == WHITE ? wKNIGHT : bKNIGHT];

    // Generate moves for all knights
    while (knights){
        int from = std::countr_zero(knights);
        Bitboard attacks = KNIGHT_ATTACKS[from] & ~colorBitboards[sideToMove];
        attackboard |= attacks;
        while (attacks){
            int to = std::countr_zero(attacks);
            attacks ^= 1ULL << to;
            moves.push_back(Cmove(from, to, getPiceceAtSquare(to) == NO_PIECE ? NORMAL : CAPTURE));
        }
        knights ^= 1ULL << from;
    }

    return moves;
}

/*
    SLIDING PIECES MOVES
*/
Bitboard bishopAttacks[64][512]; // HUGE LOOKUP TABLES for bishop and rook attacks
Bitboard rookAttacks[64][4096];

Bitboard bishopMasks[64];
Bitboard rookMasks[64];

const int bishopBits[64] = { // RELERANT BITS FOR LOOKUP TABLES
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6
};

const int rookBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12
};

// MAGIC NUMBERS FOR LOOKUP TABLES 
const Bitboard rookMagics[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL,
};
const Bitboard bishopMagics[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL,
};


// FUNCTION Used to initialize lookup tables 
Bitboard bishopAttacksInit(int square, Bitboard block){
    // result attacks bitboard
    Bitboard attacks = 0ULL;
    
    // init ranks & files
    int r, f;
    
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    
    // generate bishop atacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    // return attack map
    return attacks;
}

Bitboard rookAttacksInit(int square, Bitboard block){
    // result attacks bitboard
    Bitboard attacks = 0ULL;
    
    // init ranks & files
    int r, f;
    
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    
    // generate rook attacks
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    
    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    
    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    
    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    
    // return attack map
    return attacks;
}

Bitboard set_occupancy(int index, int bits_in_mask, Bitboard attack_mask){
    // occupancy map
    Bitboard occupancy = 0ULL;
    
    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // get LS1B index of attacks mask
        int square = std::countr_zero(attack_mask);
        
        // pop LS1B in attack map
        attack_mask ^= (1ULL << square);
        
        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }
    
    // return occupancy map
    return occupancy;
}

Bitboard maskBishopAttacks(int square){
    // attack bitboard
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
}

Bitboard maskRookAttacks(int square){
    // attacks bitboard
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
}
void Position::initSlidersAttacks(bool is_bishop){
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishopMasks[square] = maskBishopAttacks(square);
        rookMasks[square] = maskRookAttacks(square);
        
        // init current mask
        Bitboard mask = is_bishop ? maskBishopAttacks(square) : maskRookAttacks(square);
        
        // count attack mask bits
        int bit_count = std::popcount(mask);
        
        // occupancy variations count
        int occupancy_variations = 1 << bit_count;
        
        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++)
        {
            // bishop
            if (is_bishop)
            {
                // init occupancies, magic index & attacks
                Bitboard occupancy = set_occupancy(count, bit_count, mask);
                Bitboard magic_index = occupancy * bishopMagics[square] >> 64 - bishopBits[square];
                bishopAttacks[square][magic_index] = bishopAttacksInit(square, occupancy);                
            }
            
            // rook
            else
            {
                // init occupancies, magic index & attacks
                Bitboard occupancy = set_occupancy(count, bit_count, mask);
                Bitboard magic_index = occupancy * rookMagics[square] >> 64 - rookBits[square];
                rookAttacks[square][magic_index] = rookAttacksInit(square, occupancy);                
            }
        }
    }
}

// HASH FUNCTIONS FOR ACTUAL attacbaoard lookup
Bitboard getBishopAttacks(int square, Bitboard occupancy){
    // get bishop attacks assuming current board occupancy
    occupancy &= bishopMasks[square];
    occupancy *= bishopMagics[square];
    occupancy >>= 64 - bishopBits[square];
    
    // return bishop attacks
    return bishopAttacks[square][occupancy];
}

Bitboard getRookAttacks(int square, Bitboard occupancy){
    // get bishop attacks assuming current board occupancy
    occupancy &= rookMasks[square];
    occupancy *= rookMagics[square];
    occupancy >>= 64 - rookBits[square];
    
    // return rook attacks
    return rookAttacks[square][occupancy];
}

std::vector<Cmove> Position::bishopMoves(){
    Bitboard bishops = pieceBitboards[sideToMove == WHITE ? wBISHOP : bBISHOP];
    std::vector<Cmove> moves;

    while (bishops) {
        int square = std::countr_zero(bishops);
        bishops ^= (1ULL << square);
        Bitboard attacks = getBishopAttacks(square, allBitboard);
        attacks &= ~colorBitboards[sideToMove];
        attackboard |= attacks;
        while (attacks) {
            int to = std::countr_zero(attacks);
            attacks ^= (1ULL << to);

            moves.push_back(Cmove(square, to, getPiceceAtSquare(to) == NO_PIECE ? NORMAL : CAPTURE));
        }
    }
    
    return moves;
}

std::vector<Cmove> Position::rookMoves(){
    Bitboard rooks = pieceBitboards[sideToMove == WHITE ? wROOK : bROOK];
    std::vector<Cmove> moves;

    while (rooks) {
        int square = std::countr_zero(rooks);
        rooks ^= (1ULL << square);
        Bitboard attacks = getRookAttacks(square, allBitboard);
        attacks &= ~colorBitboards[sideToMove];
        attackboard |= attacks;
        while (attacks) {
            int to = std::countr_zero(attacks);
            //std::cout << "square: " << to << std::endl;
            attacks ^= (1ULL << to);

            moves.push_back(Cmove(square, to, getPiceceAtSquare(to) == NO_PIECE ? NORMAL : CAPTURE));
        }
    }
    
    return moves;
}

std::vector<Cmove> Position::queenMoves(){
    Bitboard queen = pieceBitboards[sideToMove == WHITE ? wQUEEN : bQUEEN];
    std::vector<Cmove> moves;

    while (queen) {
        int square = std::countr_zero(queen);
        queen ^= (1ULL << square);

        Bitboard attacks = getRookAttacks(square, allBitboard) | getBishopAttacks(square, allBitboard);
        attacks &= ~colorBitboards[sideToMove];
        attackboard |= attacks;
        while (attacks) {
            int to = std::countr_zero(attacks);
            attacks ^= (1ULL << to);

            moves.push_back(Cmove(square, to, getPiceceAtSquare(to) == NO_PIECE ? NORMAL : CAPTURE));
        }
    }
    
    return moves;
}

const Bitboard kingAttacks [64]{
    0x302, 0x705, 0xE0A, 0x1C14, 0x3828, 0x7050, 0xE0A0, 0xC040,
    0x30203, 0x70507, 0xE0A0E, 0x1C141C, 0x382838, 0x705070, 0xE0A0E0, 0xC040C0,
    0x3020300, 0x7050700, 0xE0A0E00, 0x1C141C00, 0x38283800, 0x70507000, 0xE0A0E000, 0xC040C000,
    0x302030000, 0x705070000, 0xE0A0E0000, 0x1C141C0000, 0x3828380000, 0x7050700000, 0xE0A0E00000, 0xC040C00000,
    0x30203000000, 0x70507000000, 0xE0A0E000000, 0x1C141C000000, 0x382838000000, 0x705070000000, 0xE0A0E0000000, 0xC040C0000000,
    0x3020300000000, 0x7050700000000, 0xE0A0E00000000, 0x1C141C00000000, 0x38283800000000, 0x70507000000000, 0xE0A0E000000000, 0xC040C000000000,
    0x302030000000000, 0x705070000000000, 0xE0A0E0000000000, 0x1C141C0000000000, 0x3828380000000000, 0x7050700000000000, 0xE0A0E00000000000, 0xC040C00000000000,
    0x203000000000000, 0x507000000000000, 0xA0E000000000000, 0x141C000000000000, 0x2838000000000000, 0x5070000000000000, 0xA0E0000000000000, 0x40C0000000000000
};


std::vector<Cmove> Position::kingMoves(){
    Bitboard king = pieceBitboards[sideToMove == WHITE ? wKING : bKING];
    std::vector<Cmove> moves;
    
    int square = std::countr_zero(king);
    king ^= (1ULL << square);

    Bitboard attacks = kingAttacks[square];
    attacks &= ~colorBitboards[sideToMove];
    attackboard |= attacks;
    while (attacks) {
        int to = std::countr_zero(attacks);
        attacks ^= (1ULL << to);

        moves.push_back(Cmove(square, to, getPiceceAtSquare(to) == NO_PIECE ? NORMAL : CAPTURE));
    }
    

    if (sideToMove == WHITE) {
        if (castlingRights & (1 << WHITE_KINGSIDE)){
            if (!(allBitboard & 0x6000000000000000)) {
                moves.push_back(Cmove(60, 62, CASTELING));
            }
        }
        if (castlingRights & (1 << WHITE_QUEENSIDE)){
            if (!(allBitboard & 0xE00000000000000)) {
                moves.push_back(Cmove(60, 58, CASTELING));
            }
        }
    }else{
        if (castlingRights & (1 << BLACK_KINGSIDE)){
            if (!(allBitboard & 0x60)) {
                moves.push_back(Cmove(4, 6, CASTELING));
            }
        }
        if (castlingRights & (1 << BLACK_QUEENSIDE)){
            if (!(allBitboard & 0xE)) {
                moves.push_back(Cmove(4, 2, CASTELING));
            }
        }
    }
    
    
    
    return moves;
}

void Position::generateLegalMoves(){
    // Update masks for all pieces and colored pieces
    allBitboard = 0;
    colorBitboards[WHITE] = 0;
    colorBitboards[BLACK] = 0;
    for (PieceType i = wPAWN; i < bPAWN; i++) {
        this->colorBitboards[WHITE] |= this->pieceBitboards[i];
        this->colorBitboards[BLACK] |= this->pieceBitboards[i + 6];
        this->allBitboard |= this->pieceBitboards[i];
        this->allBitboard |= this->pieceBitboards[i + 6];
    }
    

    legalMoves.clear();

    // Check if king is in check and pinned pieces
    printBitboard(attackboard);
    if (pieceBitboards[sideToMove == WHITE ? wKING : bKING] & attackboard) {
        std::cout << "check" << std::endl;
    }

    attackboard = 0;
    // Generate legal moves for all pieces
    std::vector<Cmove> pawnmoves = pawnMoves();
    legalMoves.insert(legalMoves.end(), pawnmoves.begin(), pawnmoves.end());

    std::vector<Cmove> knightmoves = knightMoves();
    legalMoves.insert(legalMoves.end(), knightmoves.begin(), knightmoves.end());

    std::vector<Cmove> bishopmoves = bishopMoves();
    legalMoves.insert(legalMoves.end(), bishopmoves.begin(), bishopmoves.end());

    std::vector<Cmove> rookmoves = rookMoves();
    legalMoves.insert(legalMoves.end(), rookmoves.begin(), rookmoves.end());

    std::vector<Cmove> queenmoves = queenMoves();
    legalMoves.insert(legalMoves.end(), queenmoves.begin(), queenmoves.end());

    std::vector<Cmove> kingmoves = kingMoves();
    legalMoves.insert(legalMoves.end(), kingmoves.begin(), kingmoves.end());
}


