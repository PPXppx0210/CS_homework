#include "INode.h"
#include "Kernel.h"

//���캯����ʼ��
Inode::Inode() {
	this->is_changed = false;
	this->i_mode = 0;
	this->i_nlink = 0;
	this->i_number = -1;
	this->i_size = 0;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = 0;
	}
}

Inode::~Inode() {
	//do nothing
}

//���ļ��ж�ȡ����
void Inode::ReadI() {
	//cout << "ReadI" << endl;
	int lbn; //�ļ��߼����
	int bn; //lbn��Ӧ�������̿��
	int offset; //��ǰ�ַ�������ʼ����λ��
	int nbytes; //��ȡ��Ŀ�������ֽ���
	Buf* pBuf;
	State& state = Kernel::Instance().GetState();
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	Space& space = Kernel::Instance().GetSpace();
	space.m_nbytes = 0;
	//cout << "m_Count" << state.IOParam.m_Count << endl;
	if (state.IOParam.m_Count == 0) { //��Ҫ�����ֽ�Ϊ0���򷵻�
		return;
	}
	// һ��һ���ַ������ȫ�����ݣ�ֱ���ļ���β
	while (state.IOParam.m_Count != 0) {
		//ʹ���ļ���ǰ��ƫ��������Ӧ����ȡ���߼���ź��ڿ��е�ƫ����
		lbn = bn = state.IOParam.m_Offset / Inode::BLOCK_SIZE;
		offset = state.IOParam.m_Offset % Inode::BLOCK_SIZE;
		//��ȡ�ֽ���ȡ ʣ������ֽ��� �� ��ǰ��ʣ���ֽ��� �Ľ�Сֵ
		nbytes = ((Inode::BLOCK_SIZE - offset) > state.IOParam.m_Count) ? state.IOParam.m_Count : (Inode::BLOCK_SIZE - offset);
		int remain = this->i_size - state.IOParam.m_Offset;//��ǰƫ��λ�õ��ļ�ĩβ��ʣ���ֽ���
		if (remain <= 0) { //����Ѷ��������ļ���β
			return;
		}
		//��ȡ�ֽ�����ȡ�����ļ�ʣ���ֽ���
		nbytes = (nbytes > remain) ? remain : nbytes;
		//���߼����lbnת���������̿��bn
		if ((bn = this->Bmap(lbn)) == 0) {
			return; //û�ҵ���Ӧ��������
		}
		pBuf = bufferManager.Bread(bn);//��ȡĿ������� bn
		//����ָ�뵽������ڵ���ʼ��ȡλ�� = ������׵�ַ + ƫ����
		unsigned char* start = pBuf->b_addr + offset; 
		//�����ݴӻ�����������Ŀ���ڴ�����
		for (int i = 0; i < nbytes; i++) {
			*(state.IOParam.m_Base + i) = *(start + i);
		}
		//cout << "nbytes" << nbytes << endl;
		//��nbytes�����ļ���дλ��
		state.IOParam.m_Base += nbytes;
		state.IOParam.m_Offset += nbytes;
		state.IOParam.m_Count -= nbytes;
		space.m_nbytes += nbytes;
		//cout << "offset" << state.IOParam.m_Offset << endl;
	}
}

//���ļ���д������
void Inode::WriteI() {
	//cout << "WriteI" << endl;
	int lbn; //�ļ��߼����
	int bn; //lbn��Ӧ�������̿��
	int offset; //��ǰ�ַ�������ʼ�����ַ
	int nbytes; //�����ֽ�����
	Buf* pBuf;
	State& state = Kernel::Instance().GetState();
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	Space& space = Kernel::Instance().GetSpace();
	//state.IOParam.m_Count = min(state.IOParam.m_Count, space.m_nbytes);
	this->is_changed = true;
	if (state.IOParam.m_Count == 0) { 
		return;
	}
	while (state.IOParam.m_Count != 0) {
		lbn = state.IOParam.m_Offset / Inode::BLOCK_SIZE;
		offset = state.IOParam.m_Offset % Inode::BLOCK_SIZE;
		nbytes = ((Inode::BLOCK_SIZE - offset) > state.IOParam.m_Count) ? state.IOParam.m_Count : (Inode::BLOCK_SIZE - offset);
		// ���߼����ת��Ϊ������
		if ((bn = this->Bmap(lbn)) == 0) {
			return;
		}
		if (Inode::BLOCK_SIZE == nbytes) {
			//���д������������һ���ַ��飬��Ϊ����仺��
			pBuf = bufferManager.GetBlk(bn);
		}
		else {
			//д�����ݲ���һ���ַ��飬�ȶ���д
			pBuf = bufferManager.Bread(bn);
		}
		unsigned char* start = (unsigned char*)(pBuf->b_addr + offset); //���������ݵ���ʼдλ��
		//cout << "write_offset" << offset << endl;
		//cout << "write_nbytes" << nbytes << endl;
		//�����ݴ�Ŀ�����򿽱���������
		for (int i = 0; i < nbytes; i++) {
			*(start++) = *(state.IOParam.m_Base + i);
		}
		//��nbytes�����ļ���дλ��
		state.IOParam.m_Base += nbytes;
		state.IOParam.m_Offset += nbytes;
		state.IOParam.m_Count -= nbytes;
		if (state.IOParam.m_Offset % Inode::BLOCK_SIZE == 0) { //���д��һ���ַ��飬���������
			bufferManager.Bwrite(pBuf);
		}
		else {
			pBuf->is_dirty = true; //��ûд����������Ϊ�࣬�ӳ�д����
		}
		//�����ļ�����
		if ((this->i_size < state.IOParam.m_Offset)) {
			this->i_size = state.IOParam.m_Offset;
		}
		//cout << "�ļ�����" << this->i_size << endl;
	}
}

//���ļ����߼����ӳ�䵽�����ţ��漰ֱ��������һ�μ�������Ͷ��μ������
int Inode::Bmap(int lbn) {
	Buf* pFirstBuf;
	Buf* pSecondBuf;
	int phyBlkno; //ת����������̿��
	int* iTable; //���ڷ��������̿���һ�μ�ӡ����μ��������
	int index;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();
	if (lbn >= Inode::HUGE_FILE_BLOCK) {//���������
		return 0;
	}
	if (lbn < 6) { //С���ļ�ֱ�Ӵӻ����������ȡ����
		//ֱ���������е� lbn ����Ƕ�Ӧ�������̿��
		phyBlkno = this->i_addr[lbn];
		//������߼��黹û����֮��Ӧ�������̿�ţ���Ҫ���з���
		if (phyBlkno == 0 && (pFirstBuf = fileSystem.Alloc()) != NULL) {
			phyBlkno = pFirstBuf->b_blkno;
			//���߼���ӳ�䵽�����̿��phyBlkno
			this->i_addr[lbn] = phyBlkno;
			this->is_changed = true;
		}
		return phyBlkno;
	}
	else { //�����ļ����߾����ļ�
		if (lbn < Inode::LARGE_FILE_BLOCK) { //�����ļ�
			//����һ��������Ҫȷ������һ��
			index = (lbn - Inode::SMALL_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK + 6;
		}
		else { //�����ļ�
			//��������������Ҫȷ������һ��
			index = (lbn - Inode::LARGE_FILE_BLOCK) / (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK) + 8;
		}

		phyBlkno = this->i_addr[index];
		if (phyBlkno == 0) { //��ʾ��������Ӧ�ļ��������
			this->is_changed = true;
			if ((pFirstBuf = fileSystem.Alloc()) == NULL) { //����һ�������̿��ż��������
				return 0; //����ʧ��
			}
			pFirstBuf->is_dirty = true;
			this->i_addr[index] = pFirstBuf->b_blkno; //��¼���������������̿��
		}
		else {
			pFirstBuf = bufferManager.Bread(phyBlkno);//��ȡһ�����
		}
		iTable = (int*)pFirstBuf->b_addr; //��ȡһ����ӱ������׵�ַ
		//���ھ����ļ���Ҫ���ж����������
		if (index >= 8) { 
			//������һ����ӱ��е�ƫ��λ��
			index = ((lbn - Inode::LARGE_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK;
			phyBlkno = iTable[index]; //ָ����μ��������
			if (phyBlkno == 0) { //�����ڸö��μ��������
				if ((pSecondBuf = fileSystem.Alloc()) == NULL) { //������̷���ʧ��
					return 0;
				}
				iTable[index] = pSecondBuf->b_blkno; //��һ�μ����������̿�ż�����μ����������Ӧ��
				pFirstBuf->is_dirty = true;
			}
			else {
				pSecondBuf = bufferManager.Bread(phyBlkno);//��ȡ�������
			}
			pFirstBuf = pSecondBuf; //��pFirstBufҲָ�����������
			iTable = (int*)pFirstBuf->b_addr;//��ȡ������ӱ������׵�ַ
		}

		//�����߼����lbn����λ���������еı������index
		//������һ������������Ƕ������������������һ������һ��128�������������
		if (lbn < Inode::LARGE_FILE_BLOCK) {
			index = (lbn - Inode::SMALL_FILE_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK;
		}
		else {
			index = (lbn - Inode::LARGE_FILE_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK;
		}

		if ((phyBlkno = iTable[index]) == 0 && (pSecondBuf = fileSystem.Alloc()) != NULL) {
			phyBlkno = pSecondBuf->b_blkno; //�����䵽���ļ������̿�ŵǼ��ڼ����������
			iTable[index] = phyBlkno;
			pFirstBuf->is_dirty = true;
			pSecondBuf->is_dirty = true;
		}

		return phyBlkno;
	}
}

//���ڴ��б��޸Ĺ��� Inode ���µ�����
void Inode::IUpdate() {
	Buf* pBuf;
	DiskInode dInode;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();

	if (this->is_changed) {
		//��ȡ��Ӧ Inode ����Ļ���������
		pBuf = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + this->i_number / FileSystem::INODE_NUMBER_PER_SECTOR);
		//���´��������ڵ�
		dInode.d_mode = this->i_mode;
		dInode.d_nlink = this->i_nlink;
		dInode.d_size = this->i_size;
		for (int i = 0; i < 10; i++) {
			dInode.d_addr[i] = this->i_addr[i];
		}
		//pָ�򻺴����о����Inode��ƫ��λ��
		unsigned char* p = pBuf->b_addr + (this->i_number % FileSystem::INODE_NUMBER_PER_SECTOR) * sizeof(DiskInode);
		DiskInode* pNode = &dInode;

		//��dInode�е������ݸ��ǻ����еľ����Inode
		for (int i = 0; i < sizeof(DiskInode) / sizeof(unsigned char); i++) {
			*(p + i) = *((unsigned char*)(pNode) + i);
		}
		pBuf->is_dirty = true;
	}
}

//�ͷ����ļ����������д��̿�
void Inode::ITrunc() {
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();

	for (int i = 9; i >= 0; i--) {
		//���i_addr[]�еĵ�i�����
		if (this->i_addr[i] != 0) {
			//�����i_addr[]�е�һ�μ�ӡ����μ������
			if (i >= 6 && i <= 9) {
				Buf* pFirstBuf = bufferManager.Bread(this->i_addr[i]); //�������������뻺��
				int* pFirst = (int*)pFirstBuf->b_addr;//�������׵�ַ
				//����ÿ�ż���������128�����̿��
				for (int j = 128 - 1; j >= 0; j--) {
					if (pFirst[j] != 0) { //��������������
						if (i >= 8 && i <= 9) {//���μ����������Ƕ��һ���ͷ�
							Buf* pSecondBuf = bufferManager.Bread(pFirst[j]);
							int* pSecond = (int*)pSecondBuf->b_addr;
							for (int k = 128 - 1; k >= 0; k--) {
								if (pSecond[k] != 0) {
									fileSystem.Free(pSecond[k]); //�ͷ�ָ���Ĵ��̿�
								}
							}
						}
						fileSystem.Free(pFirst[j]);//�ͷ�һ�μ��
					}
				}
			}
			//ֱ��������ֱ���ͷ�
			fileSystem.Free(this->i_addr[i]);
			this->i_addr[i] = 0;
		}
	}
	this->i_size = 0; //�ļ���С����
	this->is_changed = true; //Inode���޸Ĺ�����Ҫͬ�������
	this->i_nlink = 1;
}

//���� Inode����Ҫ�����ļ�ɾ�������� Inode
void Inode::Clean() {
	//�ض�����IAllpc()��������·���DIskInode��ԭ�����ݣ������ļ���Ϣ
	this->i_mode = 0;
	this->i_nlink = 0;
	this->i_size = 0;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = 0;
	}
}

//�Ӵ��̶�ȡ Inode ��Ϣ���ڴ��У����ļ��򿪻򴴽�ʱʹ��
void Inode::ICopy(Buf* bp, int inumber) {
	DiskInode dInode;
	DiskInode* pNode = &dInode;

	//����Ŀ�� Inode �ڻ������е�ƫ��λ��
	unsigned char* p = bp->b_addr + (inumber % FileSystem::INODE_NUMBER_PER_SECTOR) * sizeof(DiskInode);
	//����������Ŀ�� Inode �������ֽڿ�����dInode
	for (int i = 0; i < sizeof(DiskInode) / sizeof(unsigned char); i++) {
		*((unsigned char*)(pNode)+i) = *(p + i);
	}
	//���Ƶ��ڴ�Inode��
	this->i_mode = dInode.d_mode;
	this->i_nlink = dInode.d_nlink;
	this->i_size = dInode.d_size;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = dInode.d_addr[i];
	}
}

//���캯����ʼ��
DiskInode::DiskInode() {
	this->d_mode = 0;
	this->d_nlink = 0;
	this->d_size = 0;
	for (int i = 0; i < 10; i++) {
		this->d_addr[i] = 0;
	}
	for (int i = 0; i < 3; i++) {
		this->padding[i] = 0;
	}
}

DiskInode::~DiskInode() {
	//do nothing
}