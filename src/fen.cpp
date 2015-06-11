#include <string.h>
#include <stdio.h>

#include "fen.h"
#include "defs.h"
#include "move.h"


char Fen::PieceToChar(BYTE piece)
{
	if(piece<16){
		piece-=8;
	}
	else{
		piece-=16;
	}
	switch(piece){
	case 0: return 'K';
	case 1: return 'A';
	case 2: return 'B';
	case 3: return 'N';
	case 4: return 'R';
	case 5: return 'C';
	case 6: return 'P';
	default: return 0;
	}
}

int Fen::CharToPieceType(const char fen_char){
	switch(fen_char){
	case 'k':
	case 'K': return 0;
	case 'a':
	case 'A': return 1;
	case 'b':
	case 'B': return 2;
	case 'n':
	case 'N': return 3;
	case 'r':
	case 'R': return 4;
	case 'c':
	case 'C': return 5;
	case 'p':
	case 'P': return 6;
	default: return 7;
	}
}

void Fen::OutputMove(const int move)
{
	if(move==0){
        printf("nobestmove\n");
		return;
	}
	char move_string[5];
    int from=Move::StartPosition(move);
    int to=Move::DestinationPosition(move);
	move_string[0]=(from&15)-3+'a';
	move_string[1]='9'-from/16+3;
	move_string[2]=(to&15)-3+'a';
	move_string[3]='9'-to/16+3;
	move_string[4]='\0';
    printf("bestmove %.4s\n", move_string);
	fflush(stdout);
}
