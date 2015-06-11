#ifndef FEN_H_
#define FEN_H_
#include "defs.h"
class Fen
{
public:
	static char PieceToChar(BYTE piece);
	static int CharToPieceType(const char fen_char);
	static void OutputMove(const int move);
};
#endif