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

//���Ҳ�����һ�����õĿ���������������Ҳ������б������ -1
int OpenFiles::AllocFreeSlot() {
	for (int i = 0; i < NOFILES; i++) {
		if (this->OpenFileTable[i] == NULL) {
			return i;
		}
	}
	return -1;
}

//�����ļ������� fd �ҵ���Ӧ�� File ���ƿ�
File* OpenFiles::GetF(int fd) {
	File* pFile;
	//���ļ�������������Χ
	if (fd < 0 || fd >= OpenFiles::NOFILES) {
		return NULL;
	}
	pFile = this->OpenFileTable[fd];
	return pFile; 
}

//Ϊ fd �� File ָ��֮�佨��ӳ��
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