#define _CRT_SECURE_NO_WARNINGS
#include "DiskManager.h"

DiskManager::DiskManager() {
	this->diskFd = NULL;//do nothing
}

DiskManager::~DiskManager() {
	//do nothing
}

void DiskManager::createDisk() {
	this->diskFd = fopen(this->diskPath, "wb+");//д��ģʽ
}

void DiskManager::openDisk() {
	this->diskFd = fopen(this->diskPath, "rb+");//��дģʽ
}

void DiskManager::seekOneBlock(int blkno) {
	fseek(this->diskFd, 512 * blkno, SEEK_SET);
}

void DiskManager::readOneBlock(unsigned char* buffer) {
	fread(buffer, 512, 1, this->diskFd);
}

void DiskManager::writeOneBlock(unsigned char* buffer) {
	fwrite(buffer, 512, 1, this->diskFd);
}

void DiskManager::closeDisk() {
	fclose(this->diskFd);
}