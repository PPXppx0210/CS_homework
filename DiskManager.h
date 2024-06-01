#pragma once
#include <stdio.h>

//管理二级文件系统的磁盘操作
//主要负责创建磁盘镜像文件、打开镜像文件、定位磁盘块以及执行读写操作
class DiskManager {
private:
	const char* diskPath = "myDisk.img"; //磁盘镜像
	FILE* diskFd;//文件指针，用于操作磁盘镜像文件

public:
	DiskManager();
	~DiskManager();

	void createDisk(); //创建磁盘
	void openDisk();   //打开磁盘
	void seekOneBlock(int blkno); //定位到给定块号 blkno 的开始位置
	void readOneBlock(unsigned char* buffer); //从当前文件指针位置读取一个块到给定的 buffer 缓存中
	void writeOneBlock(unsigned char* buffer); //将 buffer 缓存中的一个块内容写入当前文件指针位置
	void closeDisk();//关闭磁盘
};