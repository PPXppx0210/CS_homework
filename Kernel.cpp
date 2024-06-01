#include "Kernel.h"

//声明静态实例
Kernel Kernel::instance;
//模块实例 全局
BufferManager g_BufferManager;
DiskManager g_DiskManager;
FileManager g_FileManager;
FileSystem g_FileSystem;
State g_State;
OpenFileTable g_OpenFileTable;
InodeTable g_InodeTable;
SuperBlock g_SuperBlock;
Space g_Space;
//构造函数初始化
//将各个模块的全局实例指针绑定到 Kernel 对象的相应成员变量上
Kernel::Kernel() {
	InitBuffer();
	InitFileSystem();
	InitState();
	InitSpace();
}
//析构函数
Kernel::~Kernel() {
	//do nothing
}
//返回全局实例
Kernel& Kernel::Instance() {
	return Kernel::instance;
}
//各个init方法初始化各个子模块的指针
void Kernel::InitBuffer() {
	this->m_BufferManager = &g_BufferManager;
	this->m_DiskManager = &g_DiskManager;
}

void Kernel::InitFileSystem() {
	this->m_FileManager = &g_FileManager;
	this->m_FileSystem = &g_FileSystem;
	this->m_OpenFileTable = &g_OpenFileTable;
	this->m_InodeTable = &g_InodeTable;
	this->m_SuperBlock = &g_SuperBlock;
}

void Kernel::InitState() {
	this->m_State = &g_State;
}

void Kernel::InitSpace() {
	this->m_Space = &g_Space;
}
//各个 Get 方法返回相应模块的实例引用
BufferManager& Kernel::GetBufferManager() {
	return *(this->m_BufferManager);
}

DiskManager& Kernel::GetDiskManager() {
	return *(this->m_DiskManager);
}

FileSystem& Kernel::GetFileSystem() {
	return *(this->m_FileSystem);
}

FileManager& Kernel::GetFileManager() {
	return *(this->m_FileManager);
}

State& Kernel::GetState() {
	return *(this->m_State);
}

OpenFileTable& Kernel::GetOpenFileTable() {
	return *(this->m_OpenFileTable);
}

InodeTable& Kernel::GetInodeTable() {
	return *(this->m_InodeTable);
}

SuperBlock& Kernel::GetSuperBlock() {
	return *(this->m_SuperBlock);
}

Space& Kernel::GetSpace() {
	return *(this->m_Space);
}