#pragma once
#include "Buf.h"

//索引节点 管理内存中 Inode 信息，支持文件的读取、写入、映射等操作
class Inode {
public:
	static const int BLOCK_SIZE = 512; //文件逻辑块大小
	static const int ADDRESS_PER_INDEX_BLOCK = 128; //每个索引块包含的物理盘块号数
	static const int SMALL_FILE_BLOCK = 6; //小型文件：直接索引最大逻辑块号
	static const int LARGE_FILE_BLOCK = 128 * 2 + 6; //大型文件：一次间接索引最大逻辑块号
	static const int HUGE_FILE_BLOCK = 128 * 128 * 2 + 128 * 2 + 6; //巨型文件：二次间接索引最大逻辑块号

public:
	Inode();
	~Inode();

	void ReadI(); //根据Inode对象中的物理磁盘块索引表，读取相应文件数据
	void WriteI(); //根据Inode对象中的物理磁盘块索引表，将数据写入文件
	int Bmap(int lbn); //根据文件的逻辑块号查找其对应的物理盘块号
	void IUpdate(); //更新磁盘上的 Inode 信息
	void ITrunc(); //释放 Inode 对应文件占用的磁盘块，调整索引表
	void Clean(); //清空内存中的 Inode 数据
	void ICopy(Buf* bp, int number); //从磁盘块中读取外存 Inode，并将其信息复制到内存 Inode

public:
	bool is_changed; //该Inode是否被修改过，需要更新到外存
	unsigned int i_mode; //该Inode是否被使用以及类型
	int i_nlink; //文件硬链接计数
	int i_number; //该 Inode 在磁盘上对应的编号
	int i_size; //文件大小
	int i_addr[10]; //用于文件逻辑块号和物理块号转换的基本索引表
};

//磁盘索引节点 代表磁盘上的 Inode 信息，并被 Inode 类用于和外存保持同步
class DiskInode {
public:
	DiskInode();
	~DiskInode();

public:
	unsigned int d_mode; //该inode是否被使用
	int d_nlink; //文件硬链接计数/文件不同路径名个数
	int d_size; //文件大小，字节为单位
	int d_addr[10]; //用于文件逻辑块号和物理块号的基本索引表
	int padding[3]; //用于64字节的对齐填充
};