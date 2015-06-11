#ifndef ZOBRIST_H_
#define ZOBRIST_H_

#include "defs.h"

class ZobristKey
{
public:
	ZobristKey();
	void Init();
	void InitZero();
	void Xor(const ZobristKey &zob_key);

	INT64_ zob_key_;
	INT32_ check_key_;

private:
	INT32_ Rand32();
	INT64_ Rand64();

};

class GameZobristKey
{
public:
	static GameZobristKey* GetInstance();
	void release();

	ZobristKey player_key_;
	ZobristKey key_table_[14][256];
private:
	GameZobristKey();
	void Init();

	static GameZobristKey* instance_;
};

#endif