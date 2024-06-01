#pragma once
#include "INode.h"
#include "File.h"

class InodeTable;

//���ڹ���ϵͳ�д򿪵��ļ����ƿ� File �ṹ���ṩ������ͷ� File �Ĺ���
class OpenFileTable {
public:
	static const int NFILE = 100; //���ļ����ƿ�File�ṹ������

public:
	OpenFileTable();
	~OpenFileTable();

	File* FAlloc(); //��ϵͳ���ļ����з���һ�����е�File�ṹ
	void CloseF(File* pFile); //�� File ����Ϊ����״̬
	void ResetOpenFileTable(); //�������ļ����ƿ�����Ϊ����״̬

public:
	File m_File[NFILE];
};

//�����ڴ������л�Ծ�� Inode���ṩ���ء��ͷ��Լ�ͬ�� Inode �����̵ķ���
class InodeTable {
public:
	static const int NINODE = 100; //�ڴ�Inode����

public:
	InodeTable();
	~InodeTable();

	Inode* IGet(int inumber); //�������Inode��Ž�������ڴ���
	void IPut(Inode* pNode); //���Ѿ�û��Ŀ¼��ָ���������ͷŴ��ļ�ռ�õĴ��̿�
	void UpdateInodeTable(); //�����б��޸Ĺ����ڴ�Inode���µ���Ӧ���Inode��
	int IsLoaded(int inumber); //�����Ϊinumber�����Inode�Ƿ����ڴ濽��
	Inode* GetFreeInode(); //���ڴ�Inode����Ѱ��һ�����е��ڴ�Inode
	void ResetInodeTable(); //����

public:
	Inode m_Inode[NINODE]; //�ڴ�Inode����
};