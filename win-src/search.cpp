#include <string>
#include <windows.h>

#include "search.h"
#include "defs.h"
#include "movegenerator.h"
#include "fen.h"
#include "ucci.h"

Search::Search()
{
	start_time_=0;
	is_time_mode_=false;
}

void Search::InitSearch()
{
	ClearHistoryTable();
	ClearKillerMoves();
	hash_table_.ClearHashTable();
	//position_.InitPosition();
	best_move_=0;

}

void Search::ClearHistoryTable()
{
	memset(history_table_, 0, HISTORY_SIZE*sizeof(int));
}

void Search::ClearKillerMoves()
{
	memset(killer_moves_,0,MAX_DISTANCE*2*sizeof(int));
}

int Search::QuiescentSearch(int alpha, int beta)
{
	if(position_.num_distance_>=MAX_DISTANCE){
		return position_.Evaluate();
	}

	bool ismate=true;
	int moves[MAX_MOVE];
	int value,num_generate_moves;
	if(position_.IsCheckedAfterLastMove()){
		num_generate_moves=position_.GenerateAllMoves(moves);
		HistoryTableQSort(moves,0,num_generate_moves-1);
	}
	else{
		value=position_.Evaluate();
		if(value>=beta){
			return beta;
		}
		if(value>alpha){
			alpha=value;
		}
		num_generate_moves=position_.GenerateCaptureMoves(moves);
		MvvLvaQSort(moves,0,num_generate_moves-1);
	}
	for(int i=0;i<num_generate_moves;i++){
		if(position_.DoMove(moves[i])){
			ismate=false;
			value=-QuiescentSearch(-beta,-alpha);
			position_.UndoMove();
			if(value>=beta){
				return beta;
			}
			if(value>alpha){
				alpha=value;
			}
		}
	}
	if(ismate)
		return -MATE_VALUE+position_.num_distance_;
	return alpha;
}

BYTE Search::GetMvvLvaValue( int move ) const
{
	int start_piece=position_.board_[Move::StartPosition(move)];
	int destination_piece=position_.board_[Move::DestinationPosition(move)];
	return (MvvLva[destination_piece]<<3)-MvvLva[start_piece];
}

int Search::MvvLvaPartition( int *move_array, int p, int r ) const
{
	int i,j,tmp;
	int pivot=GetMvvLvaValue(move_array[r]);
	i=p-1;
	for(j=p;j<r;j++){
		if(GetMvvLvaValue(move_array[j])>=pivot){
			++i;
			tmp=move_array[i];
			move_array[i]=move_array[j];
			move_array[j]=tmp;
		}
	}
	tmp=move_array[i+1];
	move_array[i+1]=move_array[r];
	move_array[r]=tmp;
	return i+1;
}

void Search::MvvLvaQSort( int *move_array, int p, int r ) const
{
	if(p<r){
		int pivot=MvvLvaPartition(move_array,p,r);
		MvvLvaQSort(move_array,p,pivot-1);
		MvvLvaQSort(move_array,pivot+1,r);
	}
}

int Search::HistoryTableParition(int *move_array,int p, int r) const
{
	int i,j,tmp;
	int pivot=history_table_[move_array[r]];
	i=p-1;
	for(j=p;j<r;j++){
		if(history_table_[move_array[j]]>=pivot){
			++i;
			tmp=move_array[i];
			move_array[i]=move_array[j];
			move_array[j]=tmp;
		}
	}
	tmp=move_array[i+1];
	move_array[i+1]=move_array[r];
	move_array[r]=tmp;
	return i+1;
}

void Search::HistoryTableQSort(int *move_array,int p, int r) const
{
	if(p<r){
		int pivot=HistoryTableParition(move_array,p,r);
		HistoryTableQSort(move_array,p,pivot-1);
		HistoryTableQSort(move_array,pivot+1,r);
	}
}

int Search::PVSSearch(int alpha, int beta, int depth, bool is_null_ok)
{
	bool ismate=true;
	bool isPVS=false;
	int move;
	int value;
	int best_move=0;
	hash_type hash_flag=HASH_ALPHA;

	if(depth<=0){
		return QuiescentSearch(alpha,beta);
	}
	if(position_.num_distance_>=MAX_DISTANCE){
		return position_.Evaluate();
	}
	value=hash_table_.ProbeHash(alpha,beta,depth,best_move,position_.board_key_);
	if(value!=HASH_UNKNOWN){
		if(value<-WIN_VALUE){
			return value+position_.num_distance_;
		}
		if(value>WIN_VALUE){
			return value-position_.num_distance_;
		}
		return value;
	}
	if(is_null_ok&&(!position_.IsCheckedAfterLastMove())){
		if(position_.value_red_>NULL_MARGIN){
			position_.DoNullMove();
			value=-PVSSearch(-beta,-beta+1,depth-NULL_DEPTH-1,false);
			position_.UndoNullMove();
			if(value>=beta){
				return beta;
			}
		}
	}
	MoveGenerator move_generator(this, 0, killer_moves_[position_.num_distance_][0],killer_moves_[position_.num_distance_][1]);
	while((move=move_generator.GenerateNextMove())!=0){
		if(position_.DoMove(move)){
			ismate=false;
			if(isPVS){
				value=-PVSSearch(-alpha-1,-alpha,depth-1,true);
				if(value>alpha&&value<beta){
					value=-PVSSearch(-beta, -alpha, depth-1, true);
				}
			}
			else{
				value=-PVSSearch(-beta, -alpha, depth-1, true);
			}
			position_.UndoMove();
			if(value>=beta){
				hash_flag=HASH_BETA;
				best_move=move;
				hash_table_.RecordHash(depth,hash_flag,beta,best_move,position_.board_key_);
				SetBestMove(best_move,depth);
				return beta;
			}
			if(value>alpha){
				hash_flag=HASH_EXACT;
				best_move=move;
				isPVS=true;
				alpha=value;
			}
		}
	}
	if(ismate){
		hash_table_.RecordHash(depth,HASH_EXACT,-MATE_VALUE,best_move,position_.board_key_);
		return -MATE_VALUE+position_.num_distance_;
	}
	hash_table_.RecordHash(depth,hash_flag,alpha,best_move,position_.board_key_);
	if(best_move!=0){
		SetBestMove(best_move,depth);
	}
	return alpha;
}

void Search::SetBestMove(int move, int depth)
{
	int distance=position_.num_distance_;
	history_table_[move]+=depth;
	if(move!=killer_moves_[distance][0]){
		killer_moves_[distance][1]=killer_moves_[distance][0];
		killer_moves_[distance][0]=move;
	}
}

int Search::RootSearch(int depth)
{
	int alpha=-MATE_VALUE;
	int beta=MATE_VALUE;
	bool ismate=true;
	bool isPVS=false;
	int value,move;
	int best_move=0;
	hash_type hash_flag=HASH_ALPHA;
	MoveGenerator move_generator(this, best_move_, killer_moves_[position_.num_distance_][0],killer_moves_[position_.num_distance_][1]);
	while((move=move_generator.GenerateNextMove())!=0){
		ismate=false;
		if(GetTickCount()-start_time_>(is_time_mode_?max_time_:LONGEST_SEARCH_TIME)){
			return TIME_OVER;
		}
		if(position_.DoMove(move)){
			if(isPVS){
				value=-PVSSearch(-alpha-1,-alpha,depth-1,true);
				if(value>alpha&&value<beta){
					value=-PVSSearch(-beta, -alpha, depth-1, false);
				}
			}
			else{
				value=-PVSSearch(-beta, -alpha, depth-1, false);
			}
			position_.UndoMove();
			if(value>alpha){
				hash_flag=HASH_EXACT;
				best_move=move;
				this->best_move_=best_move;
				isPVS=true;
				alpha=value;
			}
		}
	}
	if(ismate){
		hash_table_.RecordHash(depth,HASH_EXACT,-MATE_VALUE,best_move,position_.board_key_);
		return -MATE_VALUE+position_.num_distance_;
	}
	hash_table_.RecordHash(depth,hash_flag,alpha,best_move,position_.board_key_);
	if(best_move!=0){
		SetBestMove(best_move,depth);
	}
	return alpha;
}

int Search::ThinkMoveDepth(int max_depth)
{
	is_time_mode_=false;
	start_time_=GetTickCount();
	int depth,value;
	InitSearch();
	for(depth=1;depth<=max_depth;depth++){
		value=RootSearch(depth);
		if(value==TIME_OVER){
			break;
		}
		if(value>WIN_VALUE||value<-WIN_VALUE){
			break;
		}
	}
	Fen::OutputMove(best_move_);
	return best_move_;
}

int Search::ThinkMoveTime(int max_time)
{
	is_time_mode_=true;
	start_time_=GetTickCount();
	max_time_=max_time;
	int depth,value;
	InitSearch();
	for(depth=1;depth<=UCCI_MAX_DEPTH-1;depth++){
		value=RootSearch(depth);
		if(value==TIME_OVER){
			break;
		}
		if(value>WIN_VALUE||value<-WIN_VALUE){
			break;
		}
	}
	Fen::OutputMove(best_move_);
	return best_move_;
}