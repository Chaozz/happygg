#ifndef POSITION_H_
#define POSITION_H_
#include <stack>
#include "defs.h"
#include "zobrist.h"
#include "move.h"

class Position
{
public:
	Position();
	void InitPosition();
	bool DoMove(int move);
	bool UndoMove();
	bool IsLegal(int move) const;   //走法是否合法
	void DoNullMove();              //走空步
	void UndoNullMove();
	int GenerateAllMoves(int *move_array) const;        //生成所有着法
	int GenerateCaptureMoves(int *move_array) const;        //生成所有吃子着法
	bool IsMate();             //是否被杀            
	bool IsChecked() const;            //是否被将军
	bool IsCheckedAfterLastMove() const;
	bool IsNullOkay() const;    //是否可以空步裁剪 (在残局时不使用空步裁剪)
	int Evaluate() const;
	void FenToBoard(const char* fen_str, int num_moves, long * move_list);

	int num_distance_;
	BYTE board_[256];      //当前局面
	ZobristKey board_key_;
	int value_red_;   
	int value_black_;
	int player_; //0=红方，1=黑方
private:	
	int Rotation(const int num_position) const;  //翻转棋盘
	void AddPiece(const int num_position, const BYTE num_piece);  //放置棋子num_piece于num_position
	void DeletePiece(const int num_position, const BYTE num_piece); //移除棋子num_piece于num_position
	BYTE MovePiece(const int move);          
	void UndoMovePiece(const int move, const BYTE captured_piece);
	void ChangePlayer();
	void PrintBoard();

	std::stack<MoveHistory> move_stack_;
};
#endif