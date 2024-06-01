#include "INode.h"
#include "Kernel.h"

//构造函数初始化
Inode::Inode() {
	this->is_changed = false;
	this->i_mode = 0;
	this->i_nlink = 0;
	this->i_number = -1;
	this->i_size = 0;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = 0;
	}
}

Inode::~Inode() {
	//do nothing
}

//从文件中读取数据
void Inode::ReadI() {
	//cout << "ReadI" << endl;
	int lbn; //文件逻辑块号
	int bn; //lbn对应的物理盘块号
	int offset; //当前字符块内起始传输位置
	int nbytes; //读取至目标区的字节数
	Buf* pBuf;
	State& state = Kernel::Instance().GetState();
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	Space& space = Kernel::Instance().GetSpace();
	space.m_nbytes = 0;
	//cout << "m_Count" << state.IOParam.m_Count << endl;
	if (state.IOParam.m_Count == 0) { //需要读的字节为0，则返回
		return;
	}
	// 一次一个字符块读入全部数据，直到文件结尾
	while (state.IOParam.m_Count != 0) {
		//使用文件当前的偏移量计算应当读取的逻辑块号和在块中的偏移量
		lbn = bn = state.IOParam.m_Offset / Inode::BLOCK_SIZE;
		offset = state.IOParam.m_Offset % Inode::BLOCK_SIZE;
		//读取字节数取 剩余需读字节数 和 当前块剩余字节数 的较小值
		nbytes = ((Inode::BLOCK_SIZE - offset) > state.IOParam.m_Count) ? state.IOParam.m_Count : (Inode::BLOCK_SIZE - offset);
		int remain = this->i_size - state.IOParam.m_Offset;//当前偏移位置到文件末尾的剩余字节数
		if (remain <= 0) { //如果已读到超过文件结尾
			return;
		}
		//读取字节数还取决于文件剩余字节数
		nbytes = (nbytes > remain) ? remain : nbytes;
		//将逻辑块号lbn转换成物理盘块号bn
		if ((bn = this->Bmap(lbn)) == 0) {
			return; //没找到对应的物理块号
		}
		pBuf = bufferManager.Bread(bn);//读取目标物理块 bn
		//设置指针到缓存块内的起始读取位置 = 物理块首地址 + 偏移量
		unsigned char* start = pBuf->b_addr + offset; 
		//将数据从缓冲区拷贝到目标内存区域
		for (int i = 0; i < nbytes; i++) {
			*(state.IOParam.m_Base + i) = *(start + i);
		}
		//cout << "nbytes" << nbytes << endl;
		//用nbytes更新文件读写位置
		state.IOParam.m_Base += nbytes;
		state.IOParam.m_Offset += nbytes;
		state.IOParam.m_Count -= nbytes;
		space.m_nbytes += nbytes;
		//cout << "offset" << state.IOParam.m_Offset << endl;
	}
}

//向文件中写入数据
void Inode::WriteI() {
	//cout << "WriteI" << endl;
	int lbn; //文件逻辑块号
	int bn; //lbn对应的物理盘块号
	int offset; //当前字符块内起始传输地址
	int nbytes; //传送字节数量
	Buf* pBuf;
	State& state = Kernel::Instance().GetState();
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	Space& space = Kernel::Instance().GetSpace();
	//state.IOParam.m_Count = min(state.IOParam.m_Count, space.m_nbytes);
	this->is_changed = true;
	if (state.IOParam.m_Count == 0) { 
		return;
	}
	while (state.IOParam.m_Count != 0) {
		lbn = state.IOParam.m_Offset / Inode::BLOCK_SIZE;
		offset = state.IOParam.m_Offset % Inode::BLOCK_SIZE;
		nbytes = ((Inode::BLOCK_SIZE - offset) > state.IOParam.m_Count) ? state.IOParam.m_Count : (Inode::BLOCK_SIZE - offset);
		// 将逻辑块号转化为物理块号
		if ((bn = this->Bmap(lbn)) == 0) {
			return;
		}
		if (Inode::BLOCK_SIZE == nbytes) {
			//如果写入数据正好满一个字符块，则为其分配缓存
			pBuf = bufferManager.GetBlk(bn);
		}
		else {
			//写入数据不满一个字符块，先读后写
			pBuf = bufferManager.Bread(bn);
		}
		unsigned char* start = (unsigned char*)(pBuf->b_addr + offset); //缓存中数据的起始写位置
		//cout << "write_offset" << offset << endl;
		//cout << "write_nbytes" << nbytes << endl;
		//将数据从目标区域拷贝到缓冲区
		for (int i = 0; i < nbytes; i++) {
			*(start++) = *(state.IOParam.m_Base + i);
		}
		//用nbytes更新文件读写位置
		state.IOParam.m_Base += nbytes;
		state.IOParam.m_Offset += nbytes;
		state.IOParam.m_Count -= nbytes;
		if (state.IOParam.m_Offset % Inode::BLOCK_SIZE == 0) { //如果写满一个字符块，输出到磁盘
			bufferManager.Bwrite(pBuf);
		}
		else {
			pBuf->is_dirty = true; //若没写满，则设置为脏，延迟写磁盘
		}
		//更新文件长度
		if ((this->i_size < state.IOParam.m_Offset)) {
			this->i_size = state.IOParam.m_Offset;
		}
		//cout << "文件长度" << this->i_size << endl;
	}
}

//将文件的逻辑块号映射到物理块号，涉及直接索引、一次间接索引和二次间接索引
int Inode::Bmap(int lbn) {
	Buf* pFirstBuf;
	Buf* pSecondBuf;
	int phyBlkno; //转换后的物理盘块号
	int* iTable; //用于访问索引盘块中一次间接、两次间接索引表
	int index;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();
	if (lbn >= Inode::HUGE_FILE_BLOCK) {//超出最大块号
		return 0;
	}
	if (lbn < 6) { //小型文件直接从基本索引表获取即可
		//直接索引表中的 lbn 项就是对应的物理盘块号
		phyBlkno = this->i_addr[lbn];
		//如果该逻辑块还没有与之对应的物理盘块号，则要进行分配
		if (phyBlkno == 0 && (pFirstBuf = fileSystem.Alloc()) != NULL) {
			phyBlkno = pFirstBuf->b_blkno;
			//将逻辑块映射到物理盘块号phyBlkno
			this->i_addr[lbn] = phyBlkno;
			this->is_changed = true;
		}
		return phyBlkno;
	}
	else { //大型文件或者巨型文件
		if (lbn < Inode::LARGE_FILE_BLOCK) { //大型文件
			//两个一次索引表，要确认是哪一个
			index = (lbn - Inode::SMALL_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK + 6;
		}
		else { //巨型文件
			//两个二次索引表，要确认是哪一个
			index = (lbn - Inode::LARGE_FILE_BLOCK) / (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK) + 8;
		}

		phyBlkno = this->i_addr[index];
		if (phyBlkno == 0) { //表示不存在相应的间接索引表
			this->is_changed = true;
			if ((pFirstBuf = fileSystem.Alloc()) == NULL) { //分配一个空闲盘块存放间接索引表
				return 0; //分配失败
			}
			pFirstBuf->is_dirty = true;
			this->i_addr[index] = pFirstBuf->b_blkno; //记录间接索引表的物理盘块号
		}
		else {
			pFirstBuf = bufferManager.Bread(phyBlkno);//获取一级间接
		}
		iTable = (int*)pFirstBuf->b_addr; //获取一级间接表缓冲区首地址
		//对于巨型文件还要进行二级间接索引
		if (index >= 8) { 
			//计算在一级间接表中的偏移位置
			index = ((lbn - Inode::LARGE_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK;
			phyBlkno = iTable[index]; //指向二次间接索引表
			if (phyBlkno == 0) { //不存在该二次间接索引表
				if ((pSecondBuf = fileSystem.Alloc()) == NULL) { //如果磁盘分配失败
					return 0;
				}
				iTable[index] = pSecondBuf->b_blkno; //将一次间接索引表磁盘块号记入二次间接索引表相应项
				pFirstBuf->is_dirty = true;
			}
			else {
				pSecondBuf = bufferManager.Bread(phyBlkno);//获取二级间接
			}
			pFirstBuf = pSecondBuf; //让pFirstBuf也指向二级索引块
			iTable = (int*)pFirstBuf->b_addr;//获取二级间接表缓冲区首地址
		}

		//计算逻辑块号lbn最终位于索引表中的表项序号index
		//不论是一级间接索引还是二级间接索引，到了这一步都是一张128个表项的索引表
		if (lbn < Inode::LARGE_FILE_BLOCK) {
			index = (lbn - Inode::SMALL_FILE_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK;
		}
		else {
			index = (lbn - Inode::LARGE_FILE_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK;
		}

		if ((phyBlkno = iTable[index]) == 0 && (pSecondBuf = fileSystem.Alloc()) != NULL) {
			phyBlkno = pSecondBuf->b_blkno; //将分配到的文件数据盘块号登记在间接索引表中
			iTable[index] = phyBlkno;
			pFirstBuf->is_dirty = true;
			pSecondBuf->is_dirty = true;
		}

		return phyBlkno;
	}
}

//将内存中被修改过的 Inode 更新到磁盘
void Inode::IUpdate() {
	Buf* pBuf;
	DiskInode dInode;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();

	if (this->is_changed) {
		//读取对应 Inode 区域的缓冲区数据
		pBuf = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + this->i_number / FileSystem::INODE_NUMBER_PER_SECTOR);
		//更新磁盘索引节点
		dInode.d_mode = this->i_mode;
		dInode.d_nlink = this->i_nlink;
		dInode.d_size = this->i_size;
		for (int i = 0; i < 10; i++) {
			dInode.d_addr[i] = this->i_addr[i];
		}
		//p指向缓存区中旧外存Inode的偏移位置
		unsigned char* p = pBuf->b_addr + (this->i_number % FileSystem::INODE_NUMBER_PER_SECTOR) * sizeof(DiskInode);
		DiskInode* pNode = &dInode;

		//用dInode中的新数据覆盖缓存中的旧外存Inode
		for (int i = 0; i < sizeof(DiskInode) / sizeof(unsigned char); i++) {
			*(p + i) = *((unsigned char*)(pNode) + i);
		}
		pBuf->is_dirty = true;
	}
}

//释放与文件关联的所有磁盘块
void Inode::ITrunc() {
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();

	for (int i = 9; i >= 0; i--) {
		//如果i_addr[]中的第i项存在
		if (this->i_addr[i] != 0) {
			//如果是i_addr[]中的一次间接、两次间接索引
			if (i >= 6 && i <= 9) {
				Buf* pFirstBuf = bufferManager.Bread(this->i_addr[i]); //将间接索引表读入缓存
				int* pFirst = (int*)pFirstBuf->b_addr;//索引表首地址
				//遍历每张间接索引表的128个磁盘块号
				for (int j = 128 - 1; j >= 0; j--) {
					if (pFirst[j] != 0) { //如果该项存在索引
						if (i >= 8 && i <= 9) {//二次间接索引，再嵌套一层释放
							Buf* pSecondBuf = bufferManager.Bread(pFirst[j]);
							int* pSecond = (int*)pSecondBuf->b_addr;
							for (int k = 128 - 1; k >= 0; k--) {
								if (pSecond[k] != 0) {
									fileSystem.Free(pSecond[k]); //释放指定的磁盘块
								}
							}
						}
						fileSystem.Free(pFirst[j]);//释放一次间接
					}
				}
			}
			//直接索引，直接释放
			fileSystem.Free(this->i_addr[i]);
			this->i_addr[i] = 0;
		}
	}
	this->i_size = 0; //文件大小清零
	this->is_changed = true; //Inode被修改过，需要同步到外存
	this->i_nlink = 1;
}

//清理 Inode，主要用于文件删除或重用 Inode
void Inode::Clean() {
	//特定用于IAllpc()中清空重新分配DIskInode的原有数据，即旧文件信息
	this->i_mode = 0;
	this->i_nlink = 0;
	this->i_size = 0;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = 0;
	}
}

//从磁盘读取 Inode 信息到内存中，在文件打开或创建时使用
void Inode::ICopy(Buf* bp, int inumber) {
	DiskInode dInode;
	DiskInode* pNode = &dInode;

	//计算目标 Inode 在缓冲区中的偏移位置
	unsigned char* p = bp->b_addr + (inumber % FileSystem::INODE_NUMBER_PER_SECTOR) * sizeof(DiskInode);
	//将缓冲区中目标 Inode 数据逐字节拷贝到dInode
	for (int i = 0; i < sizeof(DiskInode) / sizeof(unsigned char); i++) {
		*((unsigned char*)(pNode)+i) = *(p + i);
	}
	//复制到内存Inode中
	this->i_mode = dInode.d_mode;
	this->i_nlink = dInode.d_nlink;
	this->i_size = dInode.d_size;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = dInode.d_addr[i];
	}
}

//构造函数初始化
DiskInode::DiskInode() {
	this->d_mode = 0;
	this->d_nlink = 0;
	this->d_size = 0;
	for (int i = 0; i < 10; i++) {
		this->d_addr[i] = 0;
	}
	for (int i = 0; i < 3; i++) {
		this->padding[i] = 0;
	}
}

DiskInode::~DiskInode() {
	//do nothing
}