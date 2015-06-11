#include "movegenerator.h"
#include "search.h"

MoveGenerator::MoveGenerator(const Search* search, int hash_move, int killer_move_1, int killer_move_2):search_(search)
{
	hash_move_=hash_move;
	killer_move_1_=killer_move_1;
	killer_move_2_=killer_move_2;
	generator_phase_=PHASE_HASH;
	memset(generated_moves_,0,MAX_MOVE*sizeof(int));
}

int MoveGenerator::GenerateNextMove()
{
	int move;
	switch(generator_phase_){
	case PHASE_HASH:
		generator_phase_=PHASE_KILLER_1;
		if(hash_move_!=0&&search_->position_.IsLegal(hash_move_))
			return hash_move_;
	case PHASE_KILLER_1:
		generator_phase_=PHASE_KILLER_2;
		if(killer_move_1_!=hash_move_&&killer_move_1_!=0&&search_->position_.IsLegal(killer_move_1_)){
			return killer_move_1_;
		}
	case PHASE_KILLER_2:
		generator_phase_=PHASE_GENERATE_MOVES;
		if(killer_move_2_!=hash_move_&&killer_move_2_!=0&&search_->position_.IsLegal(killer_move_2_)){
			return killer_move_2_;
		}
	case PHASE_GENERATE_MOVES:
		generator_phase_=PHASE_REST;
		num_generated_moves=search_->position_.GenerateAllMoves(generated_moves_);
		search_->HistoryTableQSort(generated_moves_,0,num_generated_moves-1);
		index=0;
	case PHASE_REST:
		while(index<num_generated_moves){
			move=generated_moves_[index++];
			if(move!=0&&move!=hash_move_&&move!=killer_move_1_&&move!=killer_move_2_){
				return move;
			}
		}
	default:
		return 0;
	}
}