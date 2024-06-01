#include "BufferManager.h"
#include "Kernel.h"

BufferManager::BufferManager() {
	this->usedBufNum = 0;
	//对每个缓存控制块进行初始化
	for (int i = 0; i < NBUF; i++) {
		this->m_Buf[i].is_dirty = false;
		this->m_Buf[i].is_new = false;
		this->m_Buf[i].b_addr = this->Buffer[i];
		this->m_Buf[i].b_blkno = -1; //表示还没有对应的磁盘块
	}
}

BufferManager::~BufferManager() {
	//do nothing
}

Buf* BufferManager::GetBlk(int blkno) {
	DiskManager& diskManager = Kernel::Instance().GetDiskManager();
	//检测该块是否已经被分配了缓存
	//如果是，将对应的缓存块索引移到队列末尾，表示其最新使用
	for (int i = 0; i < NBUF; i++) {
		if (this->m_Buf[i].b_blkno == blkno) {
			for (int j = 0; j < this->usedBufNum; j++) {
				int tmp = this->bufQueue.front();
				this->bufQueue.pop();
				if (tmp != i) {
					this->bufQueue.push(tmp);
				}
			}
			this->bufQueue.push(i);
			return &(this->m_Buf[i]);
		}
	}
	//否则，尝试分配新的缓存块
	if (this->usedBufNum < NBUF) { //还有剩余缓存块可用
		for (int i = 0; i < NBUF; i++) {
			if (this->m_Buf[i].b_blkno == -1) {
				this->m_Buf[i].b_blkno = blkno;
				this->m_Buf[i].is_dirty = true;
				this->m_Buf[i].is_new = true;
				this->bufQueue.push(i);
				this->usedBufNum++;
				return &(this->m_Buf[i]);
			}
		}
	}
	else { //若缓存块已经用完，则FIFO淘汰队首
		int eliminateBufId = this->bufQueue.front();
		this->bufQueue.pop(); 
		//如果该块是脏的，先写回磁盘
		if (m_Buf[eliminateBufId].is_dirty) { 
			diskManager.seekOneBlock(m_Buf[eliminateBufId].b_blkno);
			diskManager.writeOneBlock(m_Buf[eliminateBufId].b_addr);
		}
		//重新分配
		this->m_Buf[eliminateBufId].b_blkno = blkno;
		this->m_Buf[eliminateBufId].is_dirty = true;
		this->m_Buf[eliminateBufId].is_new = true;
		this->bufQueue.push(eliminateBufId);
		return &(this->m_Buf[eliminateBufId]);
	}
	return NULL;
}

Buf* BufferManager::Bread(int blkno) {
	Buf* bp;
	DiskManager& diskManager = Kernel::Instance().GetDiskManager();
	bp = this->GetBlk(blkno); //调用 GetBlk 获取指定块的缓存
	//如果缓存块标记为新分配，说明磁盘块尚未加载进来，则从磁盘读取数据至该块
	if (bp->is_new == false) { //不是新分配
		return bp;
	}
	else { //是新分配
		diskManager.seekOneBlock(blkno);
		diskManager.readOneBlock(bp->b_addr);
		bp->is_new = false;
		bp->is_dirty = false;
		return bp;
	}
}

//如果缓存块为脏块，直接写入磁盘，并将其标记为干净
void BufferManager::Bwrite(Buf* bp) {
	DiskManager& diskManager = Kernel::Instance().GetDiskManager();
	if (bp->is_dirty == false) { 
		return;
	}
	else {
		diskManager.seekOneBlock(bp->b_blkno);
		diskManager.writeOneBlock(bp->b_addr);
		bp->is_dirty = false;
	}
}

//遍历所有缓存块，如果块是脏的且对应磁盘块有效，将其写回磁盘
void BufferManager::Bflush() {
	DiskManager& diskManager = Kernel::Instance().GetDiskManager();
	for (int i = 0; i < NBUF; i++) {
		if (this->m_Buf[i].b_blkno == -1 || this->m_Buf[i].is_dirty == false) {
			continue;
		}
		else {
			diskManager.seekOneBlock(this->m_Buf[i].b_blkno);
			diskManager.writeOneBlock(this->m_Buf[i].b_addr);
			this->m_Buf[i].is_dirty = false;
		}
	}
}

//将所有缓存块的状态重置，同时清空队列
void BufferManager::ResetBufferManager() {
	this->usedBufNum = 0;
	for (int i = 0; i < NBUF; i++) {
		this->m_Buf[i].is_dirty = false;
		this->m_Buf[i].is_new = false;
		this->m_Buf[i].b_addr = this->Buffer[i];
		this->m_Buf[i].b_blkno = -1; //表示还没有对应的磁盘块
	}
	while (!bufQueue.empty()) {
		bufQueue.pop();
	}
}