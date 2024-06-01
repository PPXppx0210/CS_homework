#include "OpenFileManager.h"
#include "Kernel.h"

OpenFileTable::OpenFileTable() {
	//do nothing
}

OpenFileTable::~OpenFileTable() {
	//do nothing
}

File* OpenFileTable::FAlloc() {
	int fd;
	State& state = Kernel::Instance().GetState();
	fd = state.ofiles.AllocFreeSlot(); //�ڴ��ļ����������л�ȡһ��������
	if (fd < 0) { //Ѱ�ҿ�����ʧ��
		return NULL;
	}
	//������������ File �ṹ���� File ����Ϊ�����ã������ļ���дƫ������ʼ��Ϊ 0
	for (int i = 0; i < OpenFileTable::NFILE; i++) {
		if (this->m_File[i].is_av == true) {
			state.ofiles.SetF(fd, &this->m_File[i]);
			this->m_File[i].is_av = false;
			this->m_File[i].f_offset = 0;
			return (&this->m_File[i]);
		}
	}
	return NULL;
}

void OpenFileTable::CloseF(File* pFile) {
	pFile->is_av = true;
}

void OpenFileTable::ResetOpenFileTable() {
	File tmpFile;
	for (int i = 0; i < NFILE; i++) {
		this->m_File[i] = tmpFile;
	}
}

InodeTable::InodeTable() {
	//do nothing
}

InodeTable::~InodeTable() {
	//do nothing
}

Inode* InodeTable::IGet(int inumber) {
	Inode* pInode;
	int index = this->IsLoaded(inumber); //�����Ϊinumber�����Inode�Ƿ����ڴ濽��
	if (index >= 0) { //������ڴ��У����ض�Ӧ��Inodeָ��
		pInode = &(this->m_Inode[index]);
		return pInode;
	}
	else {//����Ӵ��̼��أ���Inodetable�з������Inode��
		pInode = this->GetFreeInode();
		if (pInode == NULL) { //���ڴ�Inode������
			return NULL;
		}
		else { //�������Inode�ɹ��������Inode�����·�����ڴ�Inode
			pInode->i_number = inumber;
			BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
			//�����Inode���뻺����
			Buf* pBuf = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + inumber / FileSystem::INODE_NUMBER_PER_SECTOR);
			//�������������Inode��Ϣ�������ڴ�Inode��
			pInode->ICopy(pBuf, inumber);
			//�����¼��ص� Inode ָ��
			return pInode;
		}
	}
	return NULL; 
}

void InodeTable::IPut(Inode* pNode) {
	if (pNode->i_number == 1) {
		return;
	}
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();
	//Inode ������
	if (pNode->i_nlink <= 0) {
		//�ͷŸ��ļ�ռ�ݵ������̿�
		pNode->ITrunc();
		pNode->i_mode = 0;
		//�ͷŶ�Ӧ�����Inode
		fileSystem.IFree(pNode->i_number);
	}
	pNode->IUpdate(); //ͬ�� Inode ������
	pNode->is_changed = false; //��Ϊδ�޸Ĺ�
	pNode->i_number = -1; //�ڴ�Inode���б�־
}

//�������� Inode���������޸Ĺ��� Inode ͬ��������
void InodeTable::UpdateInodeTable() {
	for (int i = 0; i < InodeTable::NINODE; i++) {
		if (this->m_Inode[i].i_number != -1) {
			this->m_Inode[i].IUpdate();
		}
	}
}

//�����ڴ� Inode �����ظ��� inumber ��Ӧ�� Inode ����
int InodeTable::IsLoaded(int inumber) {
	for (int i = 0; i < InodeTable::NINODE; i++) {
		if (this->m_Inode[i].i_number == inumber) {
			return i;
		}
	}
	return -1;
}

//����һ�����е� Inode ����ָ��
Inode* InodeTable::GetFreeInode() {
	for (int i = 0; i < InodeTable::NINODE; i++) {
		if (this->m_Inode[i].i_number == -1) {
			return &(this->m_Inode[i]);
		}
	}
	return NULL;
}

//�������� Inode ״̬
void InodeTable::ResetInodeTable() {
	Inode tmpInode;
	for (int i = 0; i < NINODE; i++) {
		this->m_Inode[i] = tmpInode;
	}
}