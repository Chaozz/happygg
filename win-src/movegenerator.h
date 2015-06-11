#ifndef MOVE_GENERATOR_H_
#define MOVE_GENERATOR_H_
#include "defs.h"
class Search;

typedef enum{
	PHASE_HASH,
	PHASE_KILLER_1,
	PHASE_KILLER_2,
	PHASE_GENERATE_MOVES,
	PHASE_REST
}phase_type;

class MoveGenerator
{
public:
	MoveGenerator(const Search* search, int hash_move, int killer_move_1, int killer_move_2);
	int GenerateNextMove();

private:
	const Search *search_;
	int hash_move_;
	int killer_move_1_;
	int killer_move_2_;
	phase_type generator_phase_;
	int generated_moves_[MAX_MOVE];
	int num_generated_moves;
	int index;

};
#endif