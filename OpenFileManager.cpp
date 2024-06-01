#include "OpenFileManager.h"
#include "Kernel.h"

OpenFileTable::OpenFileTable() {
	//do nothing
}

OpenFileTable::~OpenFileTable() {
	//do nothing
}

File* OpenFileTable::FAlloc() {
	int fd;
	State& state = Kernel::Instance().GetState();
	fd = state.ofiles.AllocFreeSlot(); //在打开文件描述符表中获取一个空闲项
	if (fd < 0) { //寻找空闲项失败
		return NULL;
	}
	//分配描述符和 File 结构，将 File 设置为不可用，并将文件读写偏移量初始化为 0
	for (int i = 0; i < OpenFileTable::NFILE; i++) {
		if (this->m_File[i].is_av == true) {
			state.ofiles.SetF(fd, &this->m_File[i]);
			this->m_File[i].is_av = false;
			this->m_File[i].f_offset = 0;
			return (&this->m_File[i]);
		}
	}
	return NULL;
}

void OpenFileTable::CloseF(File* pFile) {
	pFile->is_av = true;
}

void OpenFileTable::ResetOpenFileTable() {
	File tmpFile;
	for (int i = 0; i < NFILE; i++) {
		this->m_File[i] = tmpFile;
	}
}

InodeTable::InodeTable() {
	//do nothing
}

InodeTable::~InodeTable() {
	//do nothing
}

Inode* InodeTable::IGet(int inumber) {
	Inode* pInode;
	int index = this->IsLoaded(inumber); //检查编号为inumber的外存Inode是否有内存拷贝
	if (index >= 0) { //如果在内存中，返回对应的Inode指针
		pInode = &(this->m_Inode[index]);
		return pInode;
	}
	else {//否则从磁盘加载，在Inodetable中分配空闲Inode项
		pInode = this->GetFreeInode();
		if (pInode == NULL) { //若内存Inode表已满
			return NULL;
		}
		else { //分配空闲Inode成功，将外存Inode读入新分配的内存Inode
			pInode->i_number = inumber;
			BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
			//将外存Inode读入缓冲区
			Buf* pBuf = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + inumber / FileSystem::INODE_NUMBER_PER_SECTOR);
			//将缓冲区的外存Inode信息拷贝到内存Inode中
			pInode->ICopy(pBuf, inumber);
			//返回新加载的 Inode 指针
			return pInode;
		}
	}
	return NULL; 
}

void InodeTable::IPut(Inode* pNode) {
	if (pNode->i_number == 1) {
		return;
	}
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();
	//Inode 无链接
	if (pNode->i_nlink <= 0) {
		//释放该文件占据的数据盘块
		pNode->ITrunc();
		pNode->i_mode = 0;
		//释放对应的外存Inode
		fileSystem.IFree(pNode->i_number);
	}
	pNode->IUpdate(); //同步 Inode 到磁盘
	pNode->is_changed = false; //置为未修改过
	pNode->i_number = -1; //内存Inode空闲标志
}

//遍历所有 Inode，并将被修改过的 Inode 同步到磁盘
void InodeTable::UpdateInodeTable() {
	for (int i = 0; i < InodeTable::NINODE; i++) {
		if (this->m_Inode[i].i_number != -1) {
			this->m_Inode[i].IUpdate();
		}
	}
}

//遍历内存 Inode 表，返回给定 inumber 对应的 Inode 索引
int InodeTable::IsLoaded(int inumber) {
	for (int i = 0; i < InodeTable::NINODE; i++) {
		if (this->m_Inode[i].i_number == inumber) {
			return i;
		}
	}
	return -1;
}

//返回一个空闲的 Inode 对象指针
Inode* InodeTable::GetFreeInode() {
	for (int i = 0; i < InodeTable::NINODE; i++) {
		if (this->m_Inode[i].i_number == -1) {
			return &(this->m_Inode[i]);
		}
	}
	return NULL;
}

//重置所有 Inode 状态
void InodeTable::ResetInodeTable() {
	Inode tmpInode;
	for (int i = 0; i < NINODE; i++) {
		this->m_Inode[i] = tmpInode;
	}
}