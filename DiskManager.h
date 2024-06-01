#pragma once
#include <stdio.h>

//��������ļ�ϵͳ�Ĵ��̲���
//��Ҫ���𴴽����̾����ļ����򿪾����ļ�����λ���̿��Լ�ִ�ж�д����
class DiskManager {
private:
	const char* diskPath = "myDisk.img"; //���̾���
	FILE* diskFd;//�ļ�ָ�룬���ڲ������̾����ļ�

public:
	DiskManager();
	~DiskManager();

	void createDisk(); //��������
	void openDisk();   //�򿪴���
	void seekOneBlock(int blkno); //��λ��������� blkno �Ŀ�ʼλ��
	void readOneBlock(unsigned char* buffer); //�ӵ�ǰ�ļ�ָ��λ�ö�ȡһ���鵽������ buffer ������
	void writeOneBlock(unsigned char* buffer); //�� buffer �����е�һ��������д�뵱ǰ�ļ�ָ��λ��
	void closeDisk();//�رմ���
};