#pragma once
#include "Buf.h"

//�����ڵ� �����ڴ��� Inode ��Ϣ��֧���ļ��Ķ�ȡ��д�롢ӳ��Ȳ���
class Inode {
public:
	static const int BLOCK_SIZE = 512; //�ļ��߼����С
	static const int ADDRESS_PER_INDEX_BLOCK = 128; //ÿ������������������̿����
	static const int SMALL_FILE_BLOCK = 6; //С���ļ���ֱ����������߼����
	static const int LARGE_FILE_BLOCK = 128 * 2 + 6; //�����ļ���һ�μ����������߼����
	static const int HUGE_FILE_BLOCK = 128 * 128 * 2 + 128 * 2 + 6; //�����ļ������μ����������߼����

public:
	Inode();
	~Inode();

	void ReadI(); //����Inode�����е�������̿���������ȡ��Ӧ�ļ�����
	void WriteI(); //����Inode�����е�������̿�������������д���ļ�
	int Bmap(int lbn); //�����ļ����߼���Ų������Ӧ�������̿��
	void IUpdate(); //���´����ϵ� Inode ��Ϣ
	void ITrunc(); //�ͷ� Inode ��Ӧ�ļ�ռ�õĴ��̿飬����������
	void Clean(); //����ڴ��е� Inode ����
	void ICopy(Buf* bp, int number); //�Ӵ��̿��ж�ȡ��� Inode����������Ϣ���Ƶ��ڴ� Inode

public:
	bool is_changed; //��Inode�Ƿ��޸Ĺ�����Ҫ���µ����
	unsigned int i_mode; //��Inode�Ƿ�ʹ���Լ�����
	int i_nlink; //�ļ�Ӳ���Ӽ���
	int i_number; //�� Inode �ڴ����϶�Ӧ�ı��
	int i_size; //�ļ���С
	int i_addr[10]; //�����ļ��߼���ź�������ת���Ļ���������
};

//���������ڵ� ��������ϵ� Inode ��Ϣ������ Inode �����ں���汣��ͬ��
class DiskInode {
public:
	DiskInode();
	~DiskInode();

public:
	unsigned int d_mode; //��inode�Ƿ�ʹ��
	int d_nlink; //�ļ�Ӳ���Ӽ���/�ļ���ͬ·��������
	int d_size; //�ļ���С���ֽ�Ϊ��λ
	int d_addr[10]; //�����ļ��߼���ź������ŵĻ���������
	int padding[3]; //����64�ֽڵĶ������
};