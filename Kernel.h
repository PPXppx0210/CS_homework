#pragma once
#include "BufferManager.h"
#include "DiskManager.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "State.h"
#include "OpenFileManager.h"
#include <iostream>

class Kernel {
public:
	Kernel();
	~Kernel();
	static Kernel& Instance();//ȷ������Ψһ�� Kernel ʵ��
	//�����ӿں�����������ȡ����ģ���ʵ�����������ⲿʹ��
	BufferManager& GetBufferManager();
	DiskManager& GetDiskManager();
	FileManager& GetFileManager();
	FileSystem& GetFileSystem();
	State& GetState();
	OpenFileTable& GetOpenFileTable();
	InodeTable& GetInodeTable();
	SuperBlock& GetSuperBlock();
	Space& GetSpace();

private:
	//��ʼ������ģ��ָ��
	void InitBuffer();
	void InitFileSystem();
	void InitState();
	void InitSpace();

private:
	static Kernel instance; //Kernel��Ԫ��ʵ��
	//ͨ��ÿ����ģ���ָ����ʲ�����ͬģ��Ĺ���
	BufferManager* m_BufferManager = NULL;
	DiskManager* m_DiskManager = NULL;
	FileManager* m_FileManager = NULL;
	FileSystem* m_FileSystem = NULL;
	State* m_State = NULL;
	OpenFileTable* m_OpenFileTable = NULL;
	InodeTable* m_InodeTable = NULL;
	SuperBlock* m_SuperBlock = NULL;
	Space* m_Space = NULL;
};