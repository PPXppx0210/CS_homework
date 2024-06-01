#include "File.h"

File::File() {
	this->is_av = true;
	this->f_inode = NULL;
	this->f_offset = 0;
}

File::~File() {
	//do nothing
}

OpenFiles::OpenFiles() {
	for (int i = 0; i < NOFILES; i++) {
		this->OpenFileTable[i] = NULL;
	}
}

OpenFiles::~OpenFiles() {
	//do nothing
}

//查找并返回一个可用的空闲描述符；如果找不到空闲表项，返回 -1
int OpenFiles::AllocFreeSlot() {
	for (int i = 0; i < NOFILES; i++) {
		if (this->OpenFileTable[i] == NULL) {
			return i;
		}
	}
	return -1;
}

//根据文件描述符 fd 找到对应的 File 控制块
File* OpenFiles::GetF(int fd) {
	File* pFile;
	//打开文件描述符超出范围
	if (fd < 0 || fd >= OpenFiles::NOFILES) {
		return NULL;
	}
	pFile = this->OpenFileTable[fd];
	return pFile; 
}

//为 fd 和 File 指针之间建立映射
void OpenFiles::SetF(int fd, File* pFile) {
	if (fd < 0 || fd >= OpenFiles::NOFILES) {
		return;
	}
	this->OpenFileTable[fd] = pFile;
}

IOParameter::IOParameter() {
	this->m_Base = NULL;
	this->m_Count = 0;
	this->m_Offset = 0;
}

IOParameter::~IOParameter() {
	//do nothing
}