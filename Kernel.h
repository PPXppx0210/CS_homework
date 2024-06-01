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
	static Kernel& Instance();//确保返回唯一的 Kernel 实例
	//公共接口函数，用来获取各个模块的实例引用来给外部使用
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
	//初始化各个模块指针
	void InitBuffer();
	void InitFileSystem();
	void InitState();
	void InitSpace();

private:
	static Kernel instance; //Kernel单元类实例
	//通过每个子模块的指针访问并管理不同模块的功能
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