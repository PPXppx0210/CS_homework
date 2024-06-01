#pragma once
#include "File.h"
#include "INode.h"
#include "FileManager.h"

//����ά���ļ�������״̬��������ǰĿ¼���򿪵��ļ���·����Ϣ��ϵͳ���ò���
class State {
public:
	int arg[30] = {}; //ϵͳ���õ���������
	char* dirp = NULL; //ϵͳ���ò�����һ�����ڴ洢·����
	Inode* cdir = NULL; //ָ��ǰĿ¼��Inodeָ��
	Inode* pdir = NULL; //ָ��Ŀ¼��Inodeָ��
	DirectoryEntry dent; //��ǰĿ¼��Ŀ¼��
	char dbuf[DirectoryEntry::DIRSIZE] = {}; //��ǰ·������
	char curdir[128] = {}; //��ǰ����Ŀ¼������·��
	OpenFiles ofiles; //������ļ�����������
	IOParameter IOParam; //��¼��ǰ����д�ļ���ƫ�������û�Ŀ�������ʣ���ֽڲ���

public:
	State();
	~State();

	void ResetState(); //����״̬
};

//�ṩһ��·�������ͻ������������ļ�ϵͳ�Ķ�д����
class Space {
public:
	char pathParam[128]; //·������
	unsigned char* buffer; // ָ�����ڶ�д�����Ķ�̬���仺������ָ��
	int m_nbytes; //��������Ч�ֽ���

public:
	Space();
	~Space();

	void ResetSpace(); //����
};