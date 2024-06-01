#pragma once
#include "File.h"
#include "INode.h"
#include "FileManager.h"

//用于维护文件操作的状态，包括当前目录、打开的文件、路径信息和系统调用参数
class State {
public:
	int arg[30] = {}; //系统调用的整数参数
	char* dirp = NULL; //系统调用参数，一般用于存储路径名
	Inode* cdir = NULL; //指向当前目录的Inode指针
	Inode* pdir = NULL; //指向父目录的Inode指针
	DirectoryEntry dent; //当前目录的目录项
	char dbuf[DirectoryEntry::DIRSIZE] = {}; //当前路径分量
	char curdir[128] = {}; //当前工作目录的完整路径
	OpenFiles ofiles; //管理打开文件描述符对象
	IOParameter IOParam; //记录当前读、写文件的偏移量，用户目标区域和剩余字节参数

public:
	State();
	~State();

	void ResetState(); //重置状态
};

//提供一个路径参数和缓冲区，用于文件系统的读写操作
class Space {
public:
	char pathParam[128]; //路径参数
	unsigned char* buffer; // 指向用于读写操作的动态分配缓冲区的指针
	int m_nbytes; //缓冲区有效字节数

public:
	Space();
	~Space();

	void ResetSpace(); //重置
};