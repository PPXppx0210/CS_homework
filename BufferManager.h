#pragma once
#include <queue>
#include "Buf.h"
using namespace std;

//主要负责管理缓存块的分配、读取、写入及刷新
//确保文件系统对磁盘块的操作能有效利用缓存，减少直接磁盘操作
class BufferManager {
public:
	static const int NBUF = 15;			/* 缓存块、缓冲区的数量 */
	static const int BUFFER_SIZE = 512;	/* 缓冲区大小*/

public:
	BufferManager();
	~BufferManager();

	Buf* GetBlk(int blkno); //申请一块缓存，用于读写字符块blkno
	Buf* Bread(int blkno); //从磁盘读取指定块，返回对应的缓存块
	void Bwrite(Buf* bp); //将指定缓存块的数据写入磁盘
	void Bflush(); // 将缓存中的所有脏块写回磁盘
	void ResetBufferManager(); //重置缓存管理器，清空缓存块状态

private:
	int usedBufNum;  // 已经被分配的缓存块数
	Buf m_Buf[NBUF]; // 缓存控制块数组，用于对每一缓存的管理
	unsigned char Buffer[NBUF][BUFFER_SIZE] = {}; //实际缓存区的二维数组
	queue<int> bufQueue; //使用队列保存已分配的缓存块的索引 FIFO淘汰策略
};
