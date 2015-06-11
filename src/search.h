#ifndef SEARCH_H_
#define SEARCH_H_
#include "position.h"
#include "hash.h"

class Search
{
public:
	Search();
	void InitSearch();
	void ClearHistoryTable();
	void ClearKillerMoves();
	int PVSSearch(int alpha, int beta, int depth, bool is_null_ok);    //主要变例搜索
	void HistoryTableQSort(int *move_array,int p, int r)const;
	int ThinkMoveDepth(int max_depth);
	int ThinkMoveTime(int max_time);

	Position position_;

private:
	int QuiescentSearch(int alpha, int beta);   //静态搜索
	BYTE GetMvvLvaValue(int move) const;
	int MvvLvaPartition(int *move_array, int p, int r) const;
	void MvvLvaQSort(int *move_array, int p, int r) const;
	int HistoryTableParition(int *move_array,int p, int r)const;
	void SetBestMove(int move, int depth);
	int RootSearch(int depth);     //根节点的搜索

	HashTable hash_table_;
	int history_table_[HISTORY_SIZE];
	int killer_moves_[MAX_DISTANCE][2];
	int best_move_;
	bool is_time_mode_;
    unsigned long long start_time_;
	int max_time_;
};

#endif
