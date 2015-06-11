#include <stdlib.h>

#include "zobrist.h"

GameZobristKey* GameZobristKey::instance_=NULL;

ZobristKey::ZobristKey()
{

}

INT32_ ZobristKey::Rand32()
{
	return rand() ^ ((INT32_)rand() << 15) ^ ((INT32_)rand() << 30);
}

INT64_ ZobristKey::Rand64()
{
	return rand() ^ ((INT64_)rand() << 15) ^ ((INT64_)rand() << 30) ^
		((INT64_)rand() << 45) ^ ((INT64_)rand() << 60);    
}

void ZobristKey::Init()
{
	zob_key_=Rand64();
	check_key_=Rand32();
}

void ZobristKey::InitZero()
{
	zob_key_=0;
	check_key_=0;
}

void ZobristKey::Xor(const ZobristKey &zob_key)
{
	zob_key_^=zob_key.zob_key_;
	check_key_^=zob_key.check_key_;
}

GameZobristKey::GameZobristKey()
{

}

void GameZobristKey::Init()
{
	player_key_.Init();
	for(int i=0;i<14;i++)
		for(int j=0;j<256;j++){
			key_table_[i][j].Init();
		}
}

GameZobristKey* GameZobristKey::GetInstance()
{
	if(instance_==NULL){
		instance_=new GameZobristKey();
		instance_->Init();
	}
	return instance_;
}

void GameZobristKey::release()
{
	if(instance_){
		delete instance_;
		instance_=NULL;
	}
}