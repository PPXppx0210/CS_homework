#pragma once
#include "INode.h"
#include "File.h"

class InodeTable;

//用于管理系统中打开的文件控制块 File 结构，提供分配和释放 File 的功能
class OpenFileTable {
public:
	static const int NFILE = 100; //打开文件控制块File结构的数量

public:
	OpenFileTable();
	~OpenFileTable();

	File* FAlloc(); //在系统打开文件表中分配一个空闲的File结构
	void CloseF(File* pFile); //将 File 设置为空闲状态
	void ResetOpenFileTable(); //将所有文件控制块重置为空闲状态

public:
	File m_File[NFILE];
};

//管理内存中所有活跃的 Inode，提供加载、释放以及同步 Inode 到磁盘的方法
class InodeTable {
public:
	static const int NINODE = 100; //内存Inode数量

public:
	InodeTable();
	~InodeTable();

	Inode* IGet(int inumber); //根据外存Inode编号将其读入内存中
	void IPut(Inode* pNode); //若已经没有目录项指向它，则释放此文件占用的磁盘块
	void UpdateInodeTable(); //将所有被修改过的内存Inode更新到对应外存Inode中
	int IsLoaded(int inumber); //检查编号为inumber的外存Inode是否有内存拷贝
	Inode* GetFreeInode(); //在内存Inode表中寻找一个空闲的内存Inode
	void ResetInodeTable(); //重置

public:
	Inode m_Inode[NINODE]; //内存Inode数组
};