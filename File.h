#pragma once
#include "INode.h"
#include "stddef.h"

class File {
public:
	File();
	~File();

	bool is_av; //�Ƿ����
	Inode* f_inode; //ָ����ļ����ڴ�Inode
	int f_offset; //�ļ���дλ��ָ��ƫ����
};

//����ϵͳ�д򿪵��ļ���
class OpenFiles {
public:
	static const int NOFILES = 100; //����򿪵�����ļ���

public:
	OpenFiles();
	~OpenFiles();

	int AllocFreeSlot(); //���ļ�ʱ���ڴ��ļ����������з���һ�����б���
	File* GetF(int fd); //�����ļ������������ҵ���Ӧ�Ĵ��ļ����ƿ�File�ṹ
	void SetF(int fd, File* pFile); //Ϊ�ѷ��䵽�Ŀ���������fd�Ϳ���File������������ϵ

public:
	File* OpenFileTable[NOFILES]; //File����ָ�����飬ָ��ϵͳ���ļ����е�File����
};

//���ڱ�ʾ�ļ� I/O ��صĲ������洢��д�����������Ϣ
class IOParameter {
public:
	IOParameter();
	~IOParameter();

public:
	unsigned char* m_Base; //��ǰ��дĿ�������׵�ַ
	int m_Offset; //��ǰ����д�ļ����ֽ�ƫ����
	int m_Count;  //��ǰ��ʣ��Ķ���д�ֽ�����
};