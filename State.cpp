#include "State.h"
#include "stdlib.h"

//���캯����ʼ��
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
	this->curdir[0] = '/';//��Ŀ¼
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

//�빹�캯������һ��
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
	free(this->buffer);//�ͷ��ڹ��캯���з���� buffer ������
}

void Space::ResetSpace() {
	this->pathParam[0] = '\0';
	//���ͷ������·���
	free(this->buffer);
	this->buffer = (unsigned char*)malloc(sizeof(unsigned char) * 5 * 1024 * 1024);
}