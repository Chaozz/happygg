#include <string.h>
#include <stdio.h>
#include <stack>

#include "position.h"
#include "move.h"
#include "defs.h"
#include "fen.h"

using namespace std;

Position::Position()
{

}

int Position::Rotation( const int num_position ) const
{
	return 254-num_position;  
}

void Position::AddPiece( const int num_position, const BYTE num_piece )
{
	board_[num_position]=num_piece;
	if(num_piece&8){
		value_red_+=ValuePiecePos[num_piece-8][num_position];
		board_key_.Xor(GameZobristKey::GetInstance()->key_table_[num_piece-8][num_position]);
	}
	else if(num_piece&16){
		value_black_+=ValuePiecePos[num_piece-16][Rotation(num_position)];
		board_key_.Xor(GameZobristKey::GetInstance()->key_table_[num_piece-9][num_position]);
	}
}

void Position::DeletePiece(const int num_position, const BYTE num_piece)
{
	board_[num_position]=0;
	if(num_piece&8){
		value_red_-=ValuePiecePos[num_piece-8][num_position];
		board_key_.Xor(GameZobristKey::GetInstance()->key_table_[num_piece-8][num_position]);
	}
	else if(num_piece&16){
		value_black_-=ValuePiecePos[num_piece-16][Rotation(num_position)];
		board_key_.Xor(GameZobristKey::GetInstance()->key_table_[num_piece-9][num_position]);
	}
}

void Position::InitPosition()
{
	player_=0;
	value_red_=0;
	value_black_=0;
	num_distance_=0;
	memset(board_,0,256);
	board_key_.InitZero();
	/*
	for(int i=0;i<256;i++){
		BYTE num_piece=StartUp[i];
		if(num_piece>0){
			AddPiece(i,num_piece);
		}
	}
	*/
	while(!move_stack_.empty())
		move_stack_.pop();
}

int Position::Evaluate() const
{
	if(player_==0){
		return value_red_-value_black_;
	}
	else{
		return value_black_-value_red_;
	}
}

void Position::ChangePlayer()
{
	player_=1-player_;
	board_key_.Xor(GameZobristKey::GetInstance()->player_key_);
}

BYTE Position::MovePiece(const int move)
{
	int start_position=Move::StartPosition(move);
	int dst_position=Move::DestinationPosition(move);
	BYTE captured_piece=board_[dst_position];
	if(captured_piece){
		DeletePiece(dst_position, captured_piece);
	}
	AddPiece(dst_position,board_[start_position]);
	DeletePiece(start_position,board_[start_position]);
	return captured_piece;
}

void Position::UndoMovePiece(const int move, const BYTE captured_piece)
{
	int start_position=Move::StartPosition(move);
	int dst_position=Move::DestinationPosition(move);
	AddPiece(start_position,board_[dst_position]);
	DeletePiece(dst_position,board_[dst_position]);
	if(captured_piece){
		AddPiece(dst_position,captured_piece);
	}
}

bool Position::IsChecked() const
{
	int self_piece=8+(player_<<3);
	int opposite_piece=24-self_piece;
	int num_position,num_delta,dst_position;
	BYTE dst_piece;
	for(num_position=0;num_position<256;num_position++){
		if(board_[num_position]!=self_piece+PIECE_KING)
			continue;
		//是否被车、炮、帅将军
		for(int i=0;i<4;i++){
			num_delta=KingDelta[i];
			dst_position=num_position+num_delta;
			while(Move::IsInBoard(dst_position)){
				dst_piece=board_[dst_position];
				if(dst_piece!=0){
					if(dst_piece==opposite_piece+PIECE_KING||dst_piece==opposite_piece+PIECE_ROOK){
						return true;
					}
					dst_position+=num_delta;
					break;
				}
				dst_position+=num_delta;
			}
			while(Move::IsInBoard(dst_position)){
				BYTE dst_piece=board_[dst_position];
				if(dst_piece!=0){
					if(dst_piece==opposite_piece+PIECE_CANNON){
						return true;
					}
					break;
				}
				dst_position+=num_delta;
			}
		}
		//是否被马将军
		for(int i=0;i<4;i++){
			if(board_[num_position+AdvisorDelta[i]]!=0){
				continue;
			}
			for(int j=0;j<2;j++){
				dst_position=num_position+KnightCheckDelta[i][j];
				dst_piece=board_[dst_position];
				if(dst_piece==opposite_piece+PIECE_KNIGHT){
					return true;
				}
			}
		}
		//是否被兵将军
		for(int i=0;i<4;i++){
			if(i==3&&player_==0)
				continue;
			if(i==0&&player_==1)
				continue;
			dst_position=num_position+KingDelta[i];
			dst_piece=board_[dst_position];
			if(dst_piece==opposite_piece+PIECE_PAWN)
				return true;
		}
		return false;
	}
	return false;
}

bool Position::DoMove(int move)
{
	BYTE captured_piece=MovePiece(move);
	if(IsChecked()){
		UndoMovePiece(move,captured_piece);
		return false;
	}
	ChangePlayer();
	++num_distance_;
	MoveHistory last_move;
	last_move.SetMoveHistory(board_key_.zob_key_,move,captured_piece,IsChecked());
	move_stack_.push(last_move);
	return true;
}

bool Position::UndoMove()
{
	if(move_stack_.empty())
		return false;
	MoveHistory last_move=move_stack_.top();
	move_stack_.pop();
	ChangePlayer();
	UndoMovePiece(last_move.move_,last_move.captured_piece_);
	--num_distance_;
	return true;
}

bool Position::IsCheckedAfterLastMove() const
{
	MoveHistory last_move=move_stack_.top();
	return last_move.ischeck_;
}

bool Position::IsNullOkay() const
{
	if(player_==0)
		return value_red_>NULL_MARGIN;
	else
		return value_black_>NULL_MARGIN;
}

bool Position::IsLegal(int move)const
{
	int self_piece=8+(player_<<3);
	int start_position,dst_position,start_piece,dst_piece,pin_position,num_delta;
	start_position=Move::StartPosition(move);
	dst_position=Move::DestinationPosition(move);
	start_piece=board_[start_position];
	if((start_piece&self_piece)==0)
		return false;
	dst_piece=board_[dst_position];
	if((dst_piece&self_piece)!=0)
		return false;
	switch(start_piece-self_piece){
	case PIECE_KING:
		return Move::IsInFort(dst_position)&&Move::IsKingSpan(start_position,dst_position);
	case PIECE_ADVISOR:
		return Move::IsInFort(dst_position)&&Move::IsAdvisorSpan(start_position,dst_position);
	case PIECE_BISHOP:
		return Move::IsSameHalfSide(start_position,dst_position)&&Move::IsBishopSpan(start_position,dst_position,board_);
	case PIECE_KNIGHT:
		pin_position=Move::KnightPinPosition(start_position,dst_position);
		if(pin_position==0)
			return false;
		if(board_[start_position+pin_position]!=0)
			return false;
		return true;
	case PIECE_ROOK:
	case PIECE_CANNON:
		if(Move::IsSameRow(start_position,dst_position)){
			num_delta=(start_position<dst_position?-1:1);
		}
		else if(Move::IsSameCol(start_position,dst_position)){
			num_delta=(start_position<dst_position?-16:16);
		}
		else{
			return false;
		}
		pin_position=start_position+num_delta;
		while(Move::IsInBoard(pin_position+num_delta)&&pin_position!=dst_position&&board_[pin_position]==0){
			//printf("pin: %d\n", pin_position);
			pin_position+=num_delta;
		}
		if(pin_position==dst_position){
			return dst_piece==0||(start_piece-self_piece==PIECE_ROOK);
		}
		else if(Move::IsInBoard(pin_position+num_delta)&&dst_piece!=0&&(start_piece-self_piece==PIECE_CANNON)){
			pin_position+=num_delta;
			while(pin_position!=dst_position&&board_[pin_position]==0){
				pin_position+=num_delta;
			}
			return pin_position==dst_position;
		}
		else{
			return false;
		}
		case PIECE_PAWN:
			if(Move::IsCrossRiver(dst_position,player_)&& (dst_position == start_position - 1 || dst_position == start_position + 1)){
				return true;
			}
			else {
				return dst_position-start_position==(player_<<5)-16;
			}
		default:
			return false;
	}
}

void Position::DoNullMove()
{
	ChangePlayer();
	++num_distance_;
	MoveHistory last_move;
	last_move.SetMoveHistory(board_key_.zob_key_,0,0,false);
	move_stack_.push(last_move);
}

void Position::UndoNullMove()
{
	ChangePlayer();
	--num_distance_;
	move_stack_.pop();
}

int Position::GenerateAllMoves(int *move_array) const
{
	int start_position,start_piece,dst_position,dst_piece,pin_position,num_delta;
	int self_piece=8+(player_<<3);
	int opposite_piece=24-self_piece;
	int num_moves=0;
	for(start_position=0;start_position<256;start_position++){
		start_piece=board_[start_position];
		if((start_piece&self_piece)==0)
			continue;
		switch(start_piece-self_piece){
		case PIECE_KING:
			for(int i=0;i<4;i++){
				dst_position=start_position+KingDelta[i];
				if(!Move::IsInFort(dst_position))
					continue;
				dst_piece=board_[dst_position];
				if((dst_piece&self_piece)==0){
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
				}
			}
			break;
		case PIECE_ADVISOR:
			for(int i=0;i<4;i++){
				dst_position=start_position+AdvisorDelta[i];
				if(!Move::IsInFort(dst_position))
					continue;
				dst_piece=board_[dst_position];
				if((dst_piece&self_piece)==0){
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
				}
			}
			break;
		case PIECE_BISHOP:
			for(int i=0;i<4;i++){
				dst_position=start_position+BishopDelta[i];
				if(!(Move::IsSameHalfSide(start_position,dst_position)&&Move::IsInBoard(dst_position)))
					continue;
				if(board_[start_position+AdvisorDelta[i]]!=0)
					continue;
				dst_piece=board_[dst_position];
				if((dst_piece&self_piece)==0){
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
				}
			}
			break;
		case PIECE_KNIGHT:
			for(int i=0;i<4;i++){
				pin_position=start_position+KingDelta[i];
				if(board_[pin_position]!=0)
					continue;
				for(int j=0;j<2;j++){
					dst_position=start_position+KnightDelta[i][j];
					if(!Move::IsInBoard(dst_position))
						continue;
					dst_piece=board_[dst_position];
					if((dst_piece&self_piece)==0){
						move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
						++num_moves;
					}
				}
			}
			break;
		case PIECE_ROOK:
			for(int i=0;i<4;i++){
				num_delta=KingDelta[i];
				dst_position=start_position+num_delta;
				while(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if(dst_piece!=0){
						if((dst_piece&opposite_piece)!=0){
							move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
							++num_moves;
						}
						break;
					}
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
					dst_position+=num_delta;
				}
			}
			break;
		case PIECE_CANNON:
			for(int i=0;i<4;i++){
				num_delta=KingDelta[i];
				dst_position=start_position+num_delta;
				while(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if(dst_piece!=0){
						dst_position+=num_delta;
						break;
					}
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
					dst_position+=num_delta;
				}
				while(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if(dst_piece==0){
						dst_position+=num_delta;
						continue;
					}
					if((dst_piece&opposite_piece)!=0){
						move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
						++num_moves;
					}
					break;
				}
			}
			break;
		case PIECE_PAWN:
			num_delta=(player_<<5)-16;
			int pawn_delta_array[3]={num_delta,-1,1};
			int upbound;
			if(Move::IsCrossRiver(start_piece,player_)){
				upbound=3;
			}
			else{
				upbound=1;
			}
			for(int i=0;i<upbound;i++){
				dst_position=start_position+pawn_delta_array[i];
				if(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if((dst_piece&self_piece)==0){
						move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
						++num_moves;
					}
				}
			}
			break;
		}
	}
	return num_moves;
}

int Position::GenerateCaptureMoves(int *move_array) const
{
	int start_position,start_piece,dst_position,dst_piece,pin_position,num_delta;
	int self_piece=8+(player_<<3);
	int opposite_piece=24-self_piece;
	int num_moves=0;
	for(start_position=0;start_position<256;start_position++){
		start_piece=board_[start_position];
		if((start_piece&self_piece)==0)
			continue;
		switch(start_piece-self_piece){
		case PIECE_KING:
			for(int i=0;i<4;i++){
				dst_position=start_position+KingDelta[i];
				if(!Move::IsInFort(dst_position))
					continue;
				dst_piece=board_[dst_position];
				if((dst_piece&opposite_piece)!=0){
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
				}
			}
			break;
		case PIECE_ADVISOR:
			for(int i=0;i<4;i++){
				dst_position=start_position+AdvisorDelta[i];
				if(!Move::IsInFort(dst_position))
					continue;
				dst_piece=board_[dst_position];
				if((dst_piece&opposite_piece)!=0){
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
				}
			}
			break;
		case PIECE_BISHOP:
			for(int i=0;i<4;i++){
				dst_position=start_position+BishopDelta[i];
				if(!(Move::IsSameHalfSide(start_position,dst_position)&&Move::IsInBoard(dst_position)))
					continue;
				if(board_[start_position+AdvisorDelta[i]]!=0)
					continue;
				dst_piece=board_[dst_position];
				if((dst_piece&opposite_piece)!=0){
					move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
					++num_moves;
				}
			}
			break;
		case PIECE_KNIGHT:
			for(int i=0;i<4;i++){
				pin_position=start_position+KingDelta[i];
				if(board_[pin_position]!=0)
					continue;
				for(int j=0;j<2;j++){
					dst_position=start_position+KnightDelta[i][j];
					if(!Move::IsInBoard(dst_position))
						continue;
					dst_piece=board_[dst_position];
					if((dst_piece&opposite_piece)!=0){
						move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
						++num_moves;
					}
				}
			}
			break;
		case PIECE_ROOK:
			for(int i=0;i<4;i++){
				num_delta=KingDelta[i];
				dst_position=start_position+num_delta;
				while(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if(dst_piece!=0){
						if((dst_piece&opposite_piece)!=0){
							move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
							++num_moves;
						}
						break;
					}
					dst_position+=num_delta;
				}
			}
			break;
		case PIECE_CANNON:
			for(int i=0;i<4;i++){
				num_delta=KingDelta[i];
				dst_position=start_position+num_delta;
				while(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if(dst_piece!=0){
						dst_position+=num_delta;
						break;
					}
					dst_position+=num_delta;
				}
				while(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if(dst_piece==0){
						dst_position+=num_delta;
						continue;
					}
					if((dst_piece&opposite_piece)!=0){
						move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
						++num_moves;
					}
					break;
				}
			}
			break;
		case PIECE_PAWN:
			num_delta=(player_<<5)-16;
			int pawn_delta_array[3]={num_delta,-1,1};
			int upbound;
			if(Move::IsCrossRiver(start_piece,player_)){
				upbound=3;
			}
			else{
				upbound=1;
			}
			for(int i=0;i<upbound;i++){
				dst_position=start_position+pawn_delta_array[i];
				if(Move::IsInBoard(dst_position)){
					dst_piece=board_[dst_position];
					if((dst_piece&opposite_piece)!=0){
						move_array[num_moves]=Move::GenarateMove(start_position,dst_position);
						++num_moves;
					}
				}
			}
			break;
		}
	}
	return num_moves;
}

bool Position::IsMate()
{
	int move_array[MAX_MOVE];
	int num_moves;
	BYTE captured_piece;
	num_moves=GenerateAllMoves(move_array);
	for(int i=0;i<num_moves;i++){
		captured_piece=MovePiece(move_array[i]);
		if(!IsChecked()){
			UndoMovePiece(move_array[i],captured_piece);
			return false;
		}
		UndoMovePiece(move_array[i],captured_piece);
	}
	return true;
}

void Position::FenToBoard( const char* fen_str, int num_moves, long * move_list )
{
	InitPosition();

	int i, j, k, from, to;
	int r_pc[7] = {8, 9, 10, 11, 12, 13, 14};
	int b_pc[7] = {16, 17, 18, 19, 20, 21, 22};
	const char *str;
	//position fen 1r2kab1r/2c1a4/n1c1b1n2/4p2N1/p1p6/1C4P2/P1P1P4/2N1B3C/4A4/1RBAK2R1 w - - 0 1 moves h6i4 i9h9 i4h6 h9i9
	str = fen_str;

	if (*str == '\0') {
		return;
	}

	i = 3;
	j = 3;

	while (*str != ' ') {
		if (*str == '/') {
			j = 3;
			i ++;

			if (i > 12) break;
		} else if (*str >= '1' && *str <= '9') {
			for (k = 0; k < (*str - '0'); ++k) {
				if (j >= 11) break;
				j ++;
			}
		} else if (*str >= 'A' && *str <= 'Z') {
			if (j <= 11) {
				k = Fen::CharToPieceType(*str);

				if (k < 7) {
					AddPiece((i << 4) + j,r_pc[k]);
				}

				j ++;
			}
		} else if (*str >= 'a' && *str <= 'z') {
			if (j <= 11) {

				k = Fen::CharToPieceType(*str);

				if (k < 7) {
					AddPiece((i << 4) + j,b_pc[k]);
				}

				j ++;
			}
		}

		str ++;

		if (*str == '\0') {
			return;
		}
	}

	str ++;

	if (player_ != (*str == 'b' ? 1 : 0)) {
		ChangePlayer();
	}
	for (k = 0; k < num_moves; k++)
	{
		char * str = (char *)(&(move_list[k]));
		from =((str[0] - 'a' + 3) + (('9' - str[1] + 3) << 4));
		to = ((str[2] - 'a' + 3) + (('9' - str[3] + 3) << 4));
		MovePiece(Move::GenarateMove(from, to));
		ChangePlayer();
	}

    //PrintBoard();
}

void Position::PrintBoard()
{
	for(int i=0;i<256;i++){
		if((i&15)==0){
			printf("\n");
		}
		printf("%2d, ",board_[i]);
	}
	printf("\n");
}
