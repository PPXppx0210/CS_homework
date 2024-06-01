#pragma once
#include <queue>
#include "Buf.h"
using namespace std;

//��Ҫ����������ķ��䡢��ȡ��д�뼰ˢ��
//ȷ���ļ�ϵͳ�Դ��̿�Ĳ�������Ч���û��棬����ֱ�Ӵ��̲���
class BufferManager {
public:
	static const int NBUF = 15;			/* ����顢������������ */
	static const int BUFFER_SIZE = 512;	/* ��������С*/

public:
	BufferManager();
	~BufferManager();

	Buf* GetBlk(int blkno); //����һ�黺�棬���ڶ�д�ַ���blkno
	Buf* Bread(int blkno); //�Ӵ��̶�ȡָ���飬���ض�Ӧ�Ļ����
	void Bwrite(Buf* bp); //��ָ������������д�����
	void Bflush(); // �������е��������д�ش���
	void ResetBufferManager(); //���û������������ջ����״̬

private:
	int usedBufNum;  // �Ѿ�������Ļ������
	Buf m_Buf[NBUF]; // ������ƿ����飬���ڶ�ÿһ����Ĺ���
	unsigned char Buffer[NBUF][BUFFER_SIZE] = {}; //ʵ�ʻ������Ķ�ά����
	queue<int> bufQueue; //ʹ�ö��б����ѷ���Ļ��������� FIFO��̭����
};
