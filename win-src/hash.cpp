#include <string>

#include "hash.h"


HashTable::HashTable()
{
	hash_table_ = new HashItem[HASH_SIZE];
}

HashTable::~HashTable()
{
	delete [] hash_table_;
}
void HashTable::ClearHashTable()
{
	memset(hash_table_,0,HASH_SIZE*sizeof(HashItem));
}

void HashTable::RecordHash( int depth, hash_type type, int value, int move, ZobristKey zob_key )
{
	HashItem hash_item;
	int hash_index=zob_key.zob_key_&(HASH_SIZE-1);
	hash_item=hash_table_[hash_index];
	if(hash_item.depth_>depth)
		return;
	hash_item.depth_=depth;
	hash_item.hash_type_=type;
	hash_item.value_=value;
	hash_item.best_move_=move;
	hash_item.check_sum_=zob_key.check_key_;
	hash_table_[hash_index]=hash_item;
}

int HashTable::ProbeHash(int num_alpha, int num_beta, int depth, int &move, ZobristKey zob_key)
{
	bool mate_flag=false;   //如果是杀棋 不需要满足深度条件
	HashItem hash_item;
	int hash_index=zob_key.zob_key_&(HASH_SIZE-1);
	hash_item=hash_table_[hash_index];
	if(hash_item.check_sum_!=zob_key.check_key_){
		move=0;
		return HASH_UNKNOWN;
	}
	move=hash_item.best_move_;  
	if(hash_item.value_>WIN_VALUE||hash_item.value_<-WIN_VALUE){
		mate_flag=true;
	}
	if(hash_item.depth_>=depth||mate_flag){
		if(hash_item.hash_type_==HASH_ALPHA){
			if(hash_item.value_<=num_alpha){
				return hash_item.value_;
			}
			else{
				return HASH_UNKNOWN;
			}
		}
		else if(hash_item.hash_type_==HASH_BETA){
			if(hash_item.value_>=num_beta){
				return hash_item.value_;
			}
			else{
				return HASH_UNKNOWN;
			}
		}
		else if(hash_item.hash_type_==HASH_EXACT){
			return hash_item.value_;
		}
	}
	return HASH_UNKNOWN;
}