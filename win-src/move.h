#ifndef MOVE_H_
#define MOVE_H_
#include "defs.h"

class Move
{
public:
	static int StartPosition(const int num_move);
	static int DestinationPosition(const int num_move);
	static int GenarateMove(const int num_start, const int num_dst);
	static bool IsInBoard(const int num_position);
	static bool IsInFort(const int num_position);   
	static bool IsKingSpan(const int num_start, const int num_dst);
	static bool IsAdvisorSpan(const int num_start, const int num_dst);
	static bool IsBishopSpan(const int num_start, const int num_dst,const BYTE *board);
	static bool IsCrossRiver(const int num_postition, const int player);
	static int KnightPinPosition(const int num_start, const int num_dst);    //马腿位置 返回0则走法不符
	static bool IsSameHalfSide(const int num_start, const int num_dst);
	static bool IsSameRow(const int num_start, const int num_dst);
	static bool IsSameCol(const int num_start, const int num_dst);
};

class MoveHistory
{
public:
	explicit MoveHistory();
	void SetMoveHistory(INT64_ zob_key,int move, BYTE captured_piece, bool ischeck);

	INT64_ zob_key_;
	int move_;
	BYTE captured_piece_;
	bool ischeck_;

};

#endif