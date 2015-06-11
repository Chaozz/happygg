#ifndef HASH_H_
#define HASH_H_
#include "zobrist.h"

typedef enum{
	HASH_ALPHA,
	HASH_BETA,
	HASH_EXACT,
}hash_type;

struct HashItem
{
	INT32_ check_sum_;
	int depth_;
	hash_type hash_type_;
	int value_;
	int best_move_;
};

class HashTable
{
public:
	HashTable();
	~HashTable();
	void ClearHashTable();
	int ProbeHash(int num_alpha, int num_beta, int depth, int &move, ZobristKey zob_key);
	void RecordHash(int depth, hash_type type, int value, int move, ZobristKey zob_key);

private:
	HashItem *hash_table_;
};
#endif