#include "move.h"
#include "defs.h"

int Move::StartPosition( const int num_move )
{
	return num_move&255;  //%256
}

int Move::DestinationPosition( const int num_move )
{
	return num_move>>8;  // 除以256
}

int Move::GenarateMove( const int num_start, const int num_dst )
{
	return num_start+(num_dst<<8);
}

bool Move::IsInBoard(const int num_position)
{
	return InBoard[num_position]==1;
}

bool Move::IsInFort(const int num_position)
{
	return InFort[num_position]==1;
}

bool Move::IsKingSpan(const int num_start, const int num_dst)
{
	int my_span=num_dst-num_start;
	for(int i=0;i<4;i++){
		if(my_span==KingDelta[i])
			return true;
	}
	return false;
}

bool Move::IsAdvisorSpan(const int num_start, const int num_dst)
{
	int my_span=num_dst-num_start;
	for(int i=0;i<4;i++){
		if(my_span==AdvisorDelta[i])
			return true;
	}
	return false;
}

bool Move::IsBishopSpan(const int num_start, const int num_dst,const BYTE *board)
{
	int my_span=num_dst-num_start;
	for(int i=0;i<4;i++){
		if(my_span==BishopDelta[i])
			if(board[num_start+AdvisorDelta[i]]==0)
				return true;
	}
	return false;
}

bool Move::IsCrossRiver( const int num_postition, const int player )
{
	return (num_postition & 0x80) == (player << 7);
}

int Move::KnightPinPosition(const int num_start, const int num_dst)
{
	return KnightPin[num_dst-num_start+256];
}

bool Move::IsSameHalfSide(const int num_start, const int num_dst)
{
	return ((num_start ^ num_dst) & 0x80) == 0;
}

bool Move::IsSameRow(const int num_start, const int num_dst)
{
	return ((num_start ^ num_dst) & 0xf0) == 0;
}

bool Move::IsSameCol(const int num_start, const int num_dst)
{
	return ((num_start ^ num_dst) & 0x0f) == 0;
}

MoveHistory::MoveHistory()
{

}

void MoveHistory::SetMoveHistory(INT64_ zob_key,int move, BYTE captured_piece, bool ischeck)
{
	zob_key_=zob_key;
	move_=move;
	captured_piece_=captured_piece;
	ischeck_=ischeck;
}