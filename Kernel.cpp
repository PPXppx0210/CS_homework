#include "Kernel.h"

//������̬ʵ��
Kernel Kernel::instance;
//ģ��ʵ�� ȫ��
BufferManager g_BufferManager;
DiskManager g_DiskManager;
FileManager g_FileManager;
FileSystem g_FileSystem;
State g_State;
OpenFileTable g_OpenFileTable;
InodeTable g_InodeTable;
SuperBlock g_SuperBlock;
Space g_Space;
//���캯����ʼ��
//������ģ���ȫ��ʵ��ָ��󶨵� Kernel �������Ӧ��Ա������
Kernel::Kernel() {
	InitBuffer();
	InitFileSystem();
	InitState();
	InitSpace();
}
//��������
Kernel::~Kernel() {
	//do nothing
}
//����ȫ��ʵ��
Kernel& Kernel::Instance() {
	return Kernel::instance;
}
//����init������ʼ��������ģ���ָ��
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
//���� Get ����������Ӧģ���ʵ������
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