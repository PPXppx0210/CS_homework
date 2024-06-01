#include "State.h"
#include "stdlib.h"

//构造函数初始化
State::State() {
	for (int i = 0; i < 30; i++) {
		this->arg[i] = 0;
	}
	this->dirp = NULL;
	this->cdir = NULL;
	this->pdir = NULL;
	this->dent.m_ino = 0;
	this->dent.m_name[0] = '\0';
	this->dbuf[0] = '\0';
	this->curdir[0] = '/';//根目录
	this->curdir[1] = '\0';
	for (int i = 0; i < OpenFiles::NOFILES; i++) {
		this->ofiles.OpenFileTable[i] = NULL;
	}
	this->IOParam.m_Base = NULL;
	this->IOParam.m_Count = 0;
	this->IOParam.m_Offset = 0;
}

State::~State() {
	//do nothing
}

//与构造函数保持一致
void State::ResetState() {
	for (int i = 0; i < 30; i++) {
		this->arg[i] = 0;
	}
	this->dirp = NULL;
	this->cdir = NULL;
	this->pdir = NULL;
	this->dent.m_ino = 0;
	this->dent.m_name[0] = '\0';
	this->dbuf[0] = '\0';
	this->curdir[0] = '/';
	for (int i = 0; i < OpenFiles::NOFILES; i++) {
		this->ofiles.OpenFileTable[i] = NULL;
	}
	this->IOParam.m_Base = NULL;
	this->IOParam.m_Count = 0;
	this->IOParam.m_Offset = 0;
}

Space::Space() {
	this->pathParam[0] = '\0';
	this->buffer = (unsigned char*)malloc(sizeof(unsigned char) * 5 * 1024 * 1024);//5MB
	this->m_nbytes = 0;
}

Space::~Space() {
	free(this->buffer);//释放在构造函数中分配的 buffer 缓冲区
}

void Space::ResetSpace() {
	this->pathParam[0] = '\0';
	//先释放再重新分配
	free(this->buffer);
	this->buffer = (unsigned char*)malloc(sizeof(unsigned char) * 5 * 1024 * 1024);
}