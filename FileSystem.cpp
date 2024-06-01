#include "FileSystem.h"
#include "Kernel.h"

SuperBlock::SuperBlock() {
	this->s_isize = FileSystem::INODE_SIZE;
	this->s_fsize = 18000;
	this->s_nfree = 0;
	this->s_ninode = 0;
	for (int i = 0; i < 100; i++) {
		this->s_free[i] = 0;
		this->s_inode[i] = 0;
	}
	this->s_fmod = 0;
	for (int i = 0; i < 51; i++) {
		padding[i] = 0;
	}
}

SuperBlock::~SuperBlock() {
	//do nothing
}

void SuperBlock::ResetSuperBlock() {
	this->s_isize = FileSystem::INODE_SIZE;
	this->s_fsize = 18000;
	this->s_nfree = 0;
	this->s_ninode = 0;
	for (int i = 0; i < 100; i++) {
		this->s_free[i] = 0;
		this->s_inode[i] = 0;
	}
	this->s_fmod = 0;
	for (int i = 0; i < 51; i++) {
		padding[i] = 0;
	}
}

FileSystem::FileSystem() {
	//do nothing
}

FileSystem::~FileSystem() {
	//do nothing
}

//��������Ӵ����ж�ȡ���ڴ���
void FileSystem::LoadSuperBlock() {
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
	Buf* pBuf;
	//�Ӵ��̵ĳ�����������ȡ��������������
	for (int i = 0; i < 2; i++) {
		unsigned char* p = (unsigned char*)(&superBlock) + i * 512;
		pBuf = bufferManager.Bread(FileSystem::SUPER_BLOCK_SECTOR_NUMBER + i);
		//�������ڴ�
		for (int i = 0; i < 512; i++) {
			*(p + i) = *(pBuf->b_addr + i);
		}
	}
}

//ͬ��SuperBlock������
void FileSystem::Update() {
	Buf* pBuf;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	if (superBlock.s_fmod == 0) { //����ڴ渱��û�б��޸Ĺ�
		return;
	}
	superBlock.s_fmod = 0; //���޸ı�־
	//����������ڴ渱��д�ش����ϵĳ���������
	for (int i = 0; i < 2; i++) {
		unsigned char* p = (unsigned char*)(&superBlock) + i * 512;
		pBuf = bufferManager.GetBlk(FileSystem::SUPER_BLOCK_SECTOR_NUMBER + i);
		for (int i = 0; i < 512; i++) {
			*(pBuf->b_addr + i) = *(p + i);
		}
		bufferManager.Bwrite(pBuf);
	}
	inodeTable.UpdateInodeTable();//���� Inode ��
	bufferManager.Bflush(); //���ӳ�д�Ļ����д��������
}

//�������INode
Inode* FileSystem::IAlloc() {
	Buf* pBuf;
	Inode* pNode;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	int ino; //���䵽�Ŀ������ı��
	//���SuperBlockֱ�ӹ����Ŀ���Inode�������ѿ�
	if (superBlock.s_ninode <= 0) {
		ino = 0; //����Ŵ�1��ʼ
		//���ζ������Inode���еĴ��̿飬����������棬�������Inode������
		for (int i = 0; i < superBlock.s_isize; i++) {
			pBuf = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + i);
			int* p = (int*)pBuf->b_addr; //��ȡ��������ַ
			//��黺������ÿ�����Inode��i_mode�Ƿ�Ϊ0����0�����ѱ�ռ��
			for (int j = 0; j < FileSystem::INODE_NUMBER_PER_SECTOR; j++) {
				ino++;
				int mode = *(p + j * sizeof(DiskInode) / sizeof(int));
				if (mode != 0) {
					continue;
				}
				//��Ҫ����Ƿ����ڴ�Inode��δд�����
				if (inodeTable.IsLoaded(ino) == -1) {
					//��û�У������
					superBlock.s_inode[superBlock.s_ninode++] = ino;
					//��������������Ѿ�װ�����򲻼�������
					if (superBlock.s_ninode >= 100) {
						break;
					}
				}
			}
			if (superBlock.s_ninode >= 100) {
				break;
			}
		}
		//������û�п��õ����Inode
		if (superBlock.s_ninode <= 0) {
			return NULL;
		}
	}

	//��ȡ����Inode
	while (true) {
		ino = superBlock.s_inode[--superBlock.s_ninode];
		pNode = inodeTable.IGet(ino); //������Inode�����ڴ�
		//���δ�ܷ��䵽�ڴ�
		if (NULL == pNode) {
			return NULL;
		}
		//�����Inode���У����Inode�е�����
		if (0 == pNode->i_mode) {
			pNode->Clean();
			superBlock.s_fmod = 1;
			return pNode;
		}
		else {
			inodeTable.IPut(pNode);
			continue;
		}
	}
	return NULL; 
}

void FileSystem::IFree(int number) {
	SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
	if (superBlock.s_ninode >= 100) {
		return;
	}
	//�����Ϊ number �� Inode ���ӵ�������ֱ�ӹ����Ŀ��� Inode ��������
	superBlock.s_inode[superBlock.s_ninode++] = number;
	//���ó����鱻�޸ĵı�־��
	superBlock.s_fmod = 1;
}

//�ӳ�����Ŀ��п��������з���һ�����д��̿�
Buf* FileSystem::Alloc() {
	int blkno;
	Buf* pBuf;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
	blkno = superBlock.s_free[--superBlock.s_nfree];
	if (blkno == 0) { //�ѷ��価���п��д���
		superBlock.s_nfree = 0;
		return NULL;
	}
	if (superBlock.s_nfree <= 0) { //ֱ�ӹ����Ŀ����̿�Ϊ��
		pBuf = bufferManager.Bread(blkno); //������Ӧ�Ļ����
		int* p = (int*)pBuf->b_addr; //��pָ��洢��һ��100���λ��
		//�����̿��ͷ404���ֽڶ��볬�����s_nfree��s_free[100]
		superBlock.s_nfree = *p++; // �ȶ��������̿���
		for (int i = 0; i < 100; i++) { //�����д���ջ
			superBlock.s_free[i] = *(p + i);
		}
	}
	//Ϊ�ÿ��д��̿����뻺�沢��ջ�������
	pBuf = bufferManager.GetBlk(blkno);
	for (int i = 0; i < 128; i++) {
		*((int*)(pBuf->b_addr) + i) = 0;
	}
	pBuf->is_dirty = true;
	superBlock.s_fmod = 1; //����SuperBlock���޸ı�־

	return pBuf;
}

//�����Ϊ blkno �Ĵ��̿��ͷŲ����뵽������Ŀ��п���������
void FileSystem::Free(int blkno) {
	Buf* pBuf;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
	superBlock.s_fmod = 1; //����SuperBlock���޸ı�־

	//���֮ǰϵͳ��û�п����̿�
	if (superBlock.s_nfree <= 0) {
		superBlock.s_nfree = 1;
		superBlock.s_free[0] = 0;//�����̿���������־
	}

	//���SuperBlockֱ�ӹ����Ŀ��д��̿�ŵ�ջ����
	//��s_nfree��s_free��������д����ǰ�����ͷŵ��̿��ͷ404�ֽ�
	if (superBlock.s_nfree >= 100) {
		pBuf = bufferManager.GetBlk(blkno);
		int* p = (int*)pBuf->b_addr;
		*p++ = superBlock.s_nfree;
		for (int i = 0; i < 100; i++) {
			*(p + i) = superBlock.s_free[i];
		}
		superBlock.s_nfree = 0;
		bufferManager.Bwrite(pBuf);
	}
	superBlock.s_free[superBlock.s_nfree++] = blkno;
}