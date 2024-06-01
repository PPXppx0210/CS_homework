#define _CRT_SECURE_NO_WARNINGS
#include "FileManager.h"
#include "Kernel.h"
#include "cstring"

FileManager::FileManager() {
	//do nothing
}

FileManager::~FileManager() {
	//do nothing
}

void FileManager::FormatSystem() {
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	DiskManager& diskManager = Kernel::Instance().GetDiskManager();
	FileManager& fileManager = Kernel::Instance().GetFileManager();
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();
	State& state = Kernel::Instance().GetState();
	OpenFileTable& openFIleTable = Kernel::Instance().GetOpenFileTable();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
	Space& space = Kernel::Instance().GetSpace();

	//�����������ݽṹ�е�����
	bufferManager.ResetBufferManager();
	fileManager.ResetFileManager();
	state.ResetState();
	openFIleTable.ResetOpenFileTable();
	inodeTable.ResetInodeTable();
	superBlock.ResetSuperBlock();
	space.ResetSpace();

	//�����������
	unsigned char buffer[512];
	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}
	for (int i = 0; i < 18000; i++) {
		diskManager.seekOneBlock(i);
		diskManager.writeOneBlock(buffer);
	}

	//��ʼ�������ϵ�Inode
	//��Щ Inode ��ʾ���Ŀ¼������Ŀ¼�ȹؼ�Ŀ¼���ļ�
	//#1��Inode ��Ŀ¼
	Inode* pInode = inodeTable.GetFreeInode();
	pInode->is_changed = true;
	pInode->i_mode = 2;
	pInode->i_nlink = 1;
	pInode->i_number = 1;
	pInode->i_size = 6 * 32;
	pInode->i_addr[0] = 1024;
	//#2��Inode  bin
	pInode = inodeTable.GetFreeInode();
	pInode->is_changed = true;
	pInode->i_mode = 2;
	pInode->i_nlink = 1;
	pInode->i_number = 2;
	pInode->i_size = 2 * 32;
	pInode->i_addr[0] = 1025;
	//#3��Inode   etc
	pInode = inodeTable.GetFreeInode();
	pInode->is_changed = true;
	pInode->i_mode = 2;
	pInode->i_nlink = 1;
	pInode->i_number = 3;
	pInode->i_size = 2 * 32;
	pInode->i_addr[0] = 1026;
	//#4��Inode   dev
	pInode = inodeTable.GetFreeInode();
	pInode->is_changed = true;
	pInode->i_mode = 2;
	pInode->i_nlink = 1;
	pInode->i_number = 4;
	pInode->i_size = 3 * 32;
	pInode->i_addr[0] = 1027;
	//#5��Inode  home
	pInode = inodeTable.GetFreeInode();
	pInode->is_changed = true;
	pInode->i_mode = 2;
	pInode->i_nlink = 1;
	pInode->i_number = 5;
	pInode->i_size = 2 * 32;
	pInode->i_addr[0] = 1028;
	//#6��Inode  dev/tty1
	pInode = inodeTable.GetFreeInode();
	pInode->is_changed = true;
	pInode->i_mode = 1;
	pInode->i_nlink = 1;
	pInode->i_number = 6;

	//��ʼ�����ݿ�
	//ʹ�� DirectoryEntry �ṹ����ļ�ϵͳ�ĸ�Ŀ¼������������ҪĿ¼��Ŀ¼��
	DirectoryEntry directoryEntry;
	int* p = (int*)(&directoryEntry); //ָ���ڴ��е���ʱĿ¼�����д�뻺��
	Buf* pBuffer;
	//1024#����  ��Ŀ¼
	directoryEntry.m_ino = 1;
	std::strcpy(directoryEntry.m_name, ".");
	pBuffer = bufferManager.GetBlk(1024);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i) = *(p + i);
	} //.\0
	directoryEntry.m_ino = 1;
	std::strcpy(directoryEntry.m_name, "..");
	pBuffer = bufferManager.GetBlk(1024);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 8) = *(p + i);
	} //..\0
	directoryEntry.m_ino = 2;
	std::strcpy(directoryEntry.m_name, "bin");
	pBuffer = bufferManager.GetBlk(1024);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 16) = *(p + i);
	} //bin\0
	directoryEntry.m_ino = 3;
	std::strcpy(directoryEntry.m_name, "etc");
	pBuffer = bufferManager.GetBlk(1024);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 24) = *(p + i);
	} //etc\0
	directoryEntry.m_ino = 4;
	std::strcpy(directoryEntry.m_name, "dev");
	pBuffer = bufferManager.GetBlk(1024);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 32) = *(p + i);
	} //dev\0
	directoryEntry.m_ino = 5;
	std::strcpy(directoryEntry.m_name, "home");
	pBuffer = bufferManager.GetBlk(1024);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 40) = *(p + i);
	}//home\0
	//1025#����  bin
	directoryEntry.m_ino = 2;
	std::strcpy(directoryEntry.m_name, ".");
	pBuffer = bufferManager.GetBlk(1025);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i) = *(p + i);
	} //.\0
	directoryEntry.m_ino = 1;
	std::strcpy(directoryEntry.m_name, "..");
	pBuffer = bufferManager.GetBlk(1025);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 8) = *(p + i);
	} //..\0
	//1026#����  etc
	directoryEntry.m_ino = 3;
	std::strcpy(directoryEntry.m_name, ".");
	pBuffer = bufferManager.GetBlk(1026);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i) = *(p + i);
	} //.\0
	directoryEntry.m_ino = 1;
	std::strcpy(directoryEntry.m_name, "..");
	pBuffer = bufferManager.GetBlk(1026);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 8) = *(p + i);
	} //..\0
	//1027#����  dev
	directoryEntry.m_ino = 4;
	std::strcpy(directoryEntry.m_name, ".");
	pBuffer = bufferManager.GetBlk(1027);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i) = *(p + i);
	} //.\0
	directoryEntry.m_ino = 1;
	std::strcpy(directoryEntry.m_name, "..");
	pBuffer = bufferManager.GetBlk(1027);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 8) = *(p + i);
	} //..\0
	directoryEntry.m_ino = 6;
	std::strcpy(directoryEntry.m_name, "tty1");
	pBuffer = bufferManager.GetBlk(1027);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 16) = *(p + i);
	} //tty1\0
	//1028#����  home
	directoryEntry.m_ino = 5;
	std::strcpy(directoryEntry.m_name, ".");
	pBuffer = bufferManager.GetBlk(1028);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i) = *(p + i);
	} //.\0
	directoryEntry.m_ino = 1;
	std::strcpy(directoryEntry.m_name, "..");
	pBuffer = bufferManager.GetBlk(1028);
	for (int i = 0; i < 8; i++) {
		*((int*)pBuffer->b_addr + i + 8) = *(p + i);
	} //..\0

	//���ÿ��� Inode �ʹ��̿�
	//��100������inode���������inode��ջ
	superBlock.s_ninode = 100;
	for (int i = 0; i < 100; i++) {
		superBlock.s_inode[i] = i + 8;
	}
	//�������̿���������̿�������
	for (int i = 11; i <= 179; i++) {
		pBuffer = bufferManager.GetBlk(i * 100);
		*((int*)pBuffer->b_addr) = 100;
		for (int j = 1; j < 101; j++) {
			*((int*)pBuffer->b_addr + j) = (i + 1) * 100 - j + 1;
		}
	}
	*((int*)pBuffer->b_addr + 1) = 0;
	superBlock.s_nfree = 63;
	for (int i = 0; i < 63; i++) {
		superBlock.s_free[i] = 1100 - i;
	}

	//��superBlock�޸ı�־
	superBlock.s_fmod = 1;
	//���޸ĸ��µ�����
	fileSystem.Update();
}

void FileManager::ResetFileManager() {
	this->rootDirInode = NULL;
}

void FileManager::Open() {
	Inode* pInode;
	State& state = Kernel::Instance().GetState();
	pInode = this->NameI(NextChar, FileManager::OPEN);
	//���û���ҵ���Ӧ��Inode
	if (pInode == NULL) {
		cout << "�ļ�������" << endl;
		return;
	}
	this->Open1(pInode, 0);
}

void FileManager::Creat() {
	Inode* pInode;
	State& state = Kernel::Instance().GetState();
	//����Ŀ¼��ģʽΪ1����ʾ����������Ŀ¼����д����������
	pInode = this->NameI(NextChar, FileManager::CREATE);
	//û���ҵ���Ӧ��Inode����NameI����
	if (pInode == NULL) {
		pInode = this->MakNode(1);//������Inode
		if (pInode == NULL) {
			return;
		}
		this->Open1(pInode, 2);
	}
	else {
		this->Open1(pInode, 1);
		pInode->i_mode = 1;
	}
}

//�򿪻򴴽��ļ��ĸ�������
void FileManager::Open1(Inode* pInode, int trf) {
	State& state = Kernel::Instance().GetState();
	OpenFileTable& openFIleTable = Kernel::Instance().GetOpenFileTable();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	if (trf == 1) { //���Ҫ�������ļ��Ѿ�����
		pInode->ITrunc();
	}
	//�Ӵ��ļ�������File�ṹ,����Ӧ������������
	File* pFile = openFIleTable.FAlloc();
	if (pFile == NULL) {
		inodeTable.IPut(pInode);
		return;
	}
	pFile->is_av = false;
	pFile->f_inode = pInode;
}

void FileManager::Close() {
	State& state = Kernel::Instance().GetState();
	OpenFileTable& openFileTable = Kernel::Instance().GetOpenFileTable();
	int fd = state.arg[0];//��ϵͳ״̬�л�ȡ�ļ�������
	//��ȡ���ļ����ƿ�File�ṹ
	File* pFile = state.ofiles.GetF(fd);
	if (pFile == NULL) {
		cout << "�ļ�������" << endl;
		return;
	}
	//�ͷŴ��ļ�������fd���ݼ�File�ṹ���ü���
	state.ofiles.SetF(fd, NULL);
	openFileTable.CloseF(pFile);
}

//���ļ����ƶ���ǰ�Ķ�/дλ��
void FileManager::Seek() {
	File* pFile;
	State& state = Kernel::Instance().GetState();
	int fd = state.arg[0];
	pFile = state.ofiles.GetF(fd);
	if (pFile == NULL) {
		cout << "�ļ�������" << endl;
		return; //��FIle������
	}

	int offset = state.arg[1];

	//���state.arg[2]��3-5֮��
	//����ͬ0-2�������ȵ�λ���ֽڱ�Ϊ512�ֽ�
	if (state.arg[2] > 2) {
		offset = offset << 9;
		state.arg[2] -= 3;
	}

	switch (state.arg[2]) {
	//��дλ������Ϊoffset
	case 0:
		pFile->f_offset = offset;
		break;
	//��дλ�ü�offset
	case 1:
		pFile->f_offset += offset;
		break;
	//��дλ�õ���Ϊ�ļ����ȼ�offsest
	case 2:
		pFile->f_offset = pFile->f_inode->i_size + offset;
		break;
	}
}

void FileManager::FStat() {
	File* pFile;
	State& state = Kernel::Instance().GetState();
	int fd = state.arg[0];

	pFile = state.ofiles.GetF(fd);
	if (pFile == NULL) {
		return;
	}
	this->Stat1(pFile->f_inode, state.arg[1]);
}

void FileManager::Stat() {
	Inode* pInode;
	State& state = Kernel::Instance().GetState();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	pInode = this->NameI(NextChar, FileManager::OPEN);
	if (pInode == NULL) {
		return;
	}
	this->Stat1(pInode, state.arg[1]);
	inodeTable.IPut(pInode);
}

void FileManager::Stat1(Inode* pInode, unsigned long long statBuf) {
	Buf* pBuf;
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();

	pInode->IUpdate();
	pBuf = bufferManager.Bread(FileSystem::INODE_ZONE_START_SECTOR + pInode->i_number / FileSystem::INODE_NUMBER_PER_SECTOR);
	//��Inode�����ݸ��Ƶ�״̬��Ϣ��������
	unsigned char* p = pBuf->b_addr + (pInode->i_number % FileSystem::INODE_NUMBER_PER_SECTOR) * sizeof(DiskInode);
	for (int i = 0; i < sizeof(DiskInode) / sizeof(int); i++) {
		*((int*)statBuf + i) = *(p + i);
	}
}

void FileManager::Read() {
	//cout << "Read" << endl;
	this->Rdwr(1);
}

void FileManager::Write() {
	//cout << "Write" << endl;
	this->Rdwr(2);
}

void FileManager::Rdwr(int mode) {
	//cout << "Rdwr" << endl;
	File* pFile;
	State& state = Kernel::Instance().GetState();
	Space& space = Kernel::Instance().GetSpace();

	//����ϵͳ���õĲ���fd��ȡ���ļ��ṹ�Ŀ��ƿ�
	pFile = state.ofiles.GetF(state.arg[0]);
	if (pFile == NULL) {
		cout << "�ļ�������" << endl;
		return;
	}

	state.IOParam.m_Base = space.buffer;
	state.IOParam.m_Count = state.arg[1]; //Ҫ���д���ֽ�

	//�����ļ��Ķ�д
	state.IOParam.m_Offset = pFile->f_offset;
	if (mode == 1) {
		pFile->f_inode->ReadI();
	}
	else {
		state.IOParam.m_Count = min(state.IOParam.m_Count, space.m_nbytes);
		pFile->f_inode->WriteI();
	}
	//���ݶ�д�������ƶ��ļ���дƫ��ָ��
	pFile->f_offset += state.arg[1] - state.IOParam.m_Count;
}

//���ݸ�����·����������ģʽ�����Ҷ�Ӧ��Inode
Inode* FileManager::NameI(char (*func)(), enum DirectorySearchMode mode) {
	Inode* pInode;
	Buf* pBuf;
	char curchar;
	char* pChar;
	int freeEntryOffset; //�����ļ�ģʽ����Ŀ¼ʱ����¼����Ŀ¼���ƫ����
	State& state = Kernel::Instance().GetState();
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();

	//���·����'/'��ͷ����Ӹ�Ŀ¼��ʼ����������ӵ�ǰĿ¼
	pInode = state.cdir;
	if ('/' == (curchar = (*func)())) {
		pInode = this->rootDirInode;
	}

	//����Inode�Ƿ���������ʹ��
	inodeTable.IGet(pInode->i_number);

	//�����Ķ�� / ��Ϊһ��
	while ('/' == curchar) {
		curchar = (*func)();
	}

	//���·����Ϊ���Ҳ���ģʽ���� OPEN���򷵻ش���
	if ('\0' == curchar && mode != FileManager::OPEN) {
		inodeTable.IPut(pInode);//�ͷŵ�ǰInode
		return NULL;
	}

	//���ѭ��ÿ�δ���pathname��һ��·������
	while (true) {
		//����·���������
		if ('\0' == curchar) {
			return pInode;//��������Ŀ���ļ���Inode
		}
		//���Ҫ���������Ĳ���Ŀ¼�ļ����ͷ������Դ���˳�
		if (pInode->i_mode != 2) {
			break;
		}
		//����ǰ׼������ƥ���·������������state��,���ں�Ŀ¼����бȽ�
		pChar = &(state.dbuf[0]);
		while ('/' != curchar && '\0' != curchar) {
			if (pChar < &(state.dbuf[DirectoryEntry::DIRSIZE])) {
				*pChar = curchar;
				pChar++;
			}
			curchar = (*func)();
		}
		//��dbuf��ʣ��Ĳ������Ϊ'\0'
		while (pChar < &(state.dbuf[DirectoryEntry::DIRSIZE])) {
			*pChar = '\0';
			pChar++;
		}

		//�����Ķ�� / ��Ϊһ��
		while ('/' == curchar) {
			curchar = (*func)();
		}
		//�ڲ�ѭ�����ֶ���state.dbuf[]�е�·���������������Ѱƥ���Ŀ¼��
		state.IOParam.m_Offset = 0;
		//����Ŀ¼��ĸ��������հ׵�Ŀ¼��
		state.IOParam.m_Count = pInode->i_size / (DirectoryEntry::DIRSIZE + 4);
		freeEntryOffset = 0;
		pBuf = NULL;

		while (true) {
			//��Ŀ¼���Ѿ��������
			if (0 == state.IOParam.m_Count) {
				//����Ǵ������ļ�
				if (FileManager::CREATE == mode && curchar == '\0') {
					//����Ŀ¼Inodeָ�뱣������
					state.pdir = pInode;

					if (freeEntryOffset) {
						//������ڿ���Ŀ¼���λ���˿���λ��
						state.IOParam.m_Offset = freeEntryOffset - (DirectoryEntry::DIRSIZE + 4);
					}
					else {
						//�����Ǹ�Ŀ¼�Ѹı�
						pInode->is_changed = true;
					}
					return NULL;
				}
				//������������û��ƥ����ͷŵ�ǰ Inode ���˳�
				inodeTable.IPut(pInode);
				return NULL;
			}

			//�Ѿ�����Ŀ¼�ļ���ǰ�̿飬��Ҫ������һ�������̿�
			if (0 == state.IOParam.m_Offset % Inode::BLOCK_SIZE) {
				//����Ҫ���������̿��
				int phyBlkno = pInode->Bmap(state.IOParam.m_Offset / Inode::BLOCK_SIZE);
				pBuf = bufferManager.Bread(phyBlkno);//�������ݿ�
			}

			//û�ж��굱ǰĿ¼���̿飬���Ķ���һĿ¼����state.dent
			int* src = (int*)(pBuf->b_addr + (state.IOParam.m_Offset % Inode::BLOCK_SIZE));
			for (int i = 0; i < sizeof(DirectoryEntry) / sizeof(int); i++) {
				*((int*)&state.dent + i) = *(src + i);//��Ŀ¼�����ݿ����� state.den
			}
			//����ƫ����������ʣ�����
			state.IOParam.m_Offset += (DirectoryEntry::DIRSIZE + 4);
			state.IOParam.m_Count--;

			//����ǿ���Ŀ¼���¼�������ƫ����
			if (0 == state.dent.m_ino) {
				if (0 == freeEntryOffset) {
					freeEntryOffset = state.IOParam.m_Offset;
				}
				continue;
			}
			//���Ŀ¼���Ƿ���·������ƥ��
			int i;
			bool success = false;
			for (i = 0; i < DirectoryEntry::DIRSIZE; i++) {
				if (state.dbuf[i] != state.dent.m_name[i]) {
					break; //��ƥ�䣬����ѭ��
				}
				if (state.dbuf[i] == 0) {
					success = true;//ȫ��ƥ��
				}
			}
			if (!success) {
				continue; //����Ҫ������Ŀ¼��
			}
			else {
				break; //Ŀ¼��ƥ��ɹ�
			}
		}

		//��������˵����ǰ·������ƥ��ɹ�������ƥ����һ·��������ֱ��'\0'����
		
		//�����ɾ���������򷵻ظ�Ŀ¼Inode����Ҫɾ�����ļ���Inode����state.dent.m_ino��
		if (FileManager::DELETE == mode && '\0' == curchar) {
			return pInode;
		}

		//ƥ��Ŀ¼��ɹ������ͷŵ�ǰĿ¼Inode������ƥ��ɹ���Ŀ¼��m_ino�ֶλ�ȡ��Ӧ��һ��Ŀ¼���ļ���Inode
		inodeTable.IPut(pInode);
		pInode = inodeTable.IGet(state.dent.m_ino);

		//�ص����ѭ��������ƥ����һ·������
		if (NULL == pInode) {
			return NULL;
		}
	}
}

//��·�����������ȡ�ַ�
char FileManager::NextChar() {
	State& state = Kernel::Instance().GetState();
	return *state.dirp++;
}

//����һ���µ�Inode�ڵ�
Inode* FileManager::MakNode(unsigned int mode) {
	Inode* pInode;
	State& state = Kernel::Instance().GetState();
	FileSystem& fileSystem = Kernel::Instance().GetFileSystem();

	//����һ������DIskInode������������Ѿ���ȫ���
	pInode = fileSystem.IAlloc();
	if (NULL == pInode) {
		return NULL;
	}

	pInode->is_changed = true;
	pInode->i_mode = mode;
	pInode->i_nlink = 1;

	//�����������Ŀ¼���򻹻ᴴ����Ŀ¼��"."��".."Ŀ¼�������д�����
	if (mode == 2) {
		DirectoryEntry direcroty[2];
		//����Ŀ¼���еı�Ų���
		direcroty[0].m_ino = pInode->i_number;
		direcroty[1].m_ino = state.pdir->i_number;
		//����Ŀ¼����pathname��������
		direcroty[0].m_name[0] = '.';
		direcroty[0].m_name[1] = '\0';
		direcroty[1].m_name[0] = '.';
		direcroty[1].m_name[1] = '.';
		direcroty[1].m_name[2] = '\0';

		state.IOParam.m_Count = 2 * (DirectoryEntry::DIRSIZE + 4);
		state.IOParam.m_Base = (unsigned char*)direcroty;
		state.IOParam.m_Offset = 0;

		pInode->WriteI();
	}

	this->WriteDir(pInode);
	return pInode;
}

//��Ŀ¼��д�븸Ŀ¼�ļ���
void FileManager::WriteDir(Inode* pInode) {
	State& state = Kernel::Instance().GetState();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();

	//����Ŀ¼���еı�Ų���
	state.dent.m_ino = pInode->i_number;

	//����Ŀ¼����pathname��������
	for (int i = 0; i < DirectoryEntry::DIRSIZE; i++) {
		state.dent.m_name[i] = state.dbuf[i];
	}

	state.IOParam.m_Count = DirectoryEntry::DIRSIZE + 4;
	state.IOParam.m_Base = (unsigned char*)&state.dent;
	state.IOParam.m_Offset = state.pdir->i_size;

	//��Ŀ¼��д�븸Ŀ¼�ļ�
	state.pdir->WriteI();
	inodeTable.IPut(state.pdir);
}

//���õ�ǰ����Ŀ¼
void FileManager::SetCurDir(char* pathname) {
	State& state = Kernel::Instance().GetState();

	//��·�����ǴӸ�Ŀ¼��ʼ����������state.curdir������ϵ�ǰ·������
	if (pathname[0] != '/') {
		int length = 0;
		while (true) {
			if (state.curdir[length] == 0) {
				break;
			}
			length++;
		}
		if (length > 0 && state.curdir[length - 1] != '/') {
			state.curdir[length] = '/';
			length++;
		}
		int i = 0;
		while (true) {
			state.curdir[length + i] = pathname[i];
			i++;
			if (pathname[i] == 0) {
				break;
			}
		}
	}
	else {
		int i = 0;
		while (true) {
			state.curdir[i] = pathname[i];
			if (state.curdir[i] == 0) {
				break;
			}
			i++;
		}
	}
	//����������.��/
	char tmp[20][128]; //���ڴ洢�ָ����·��
	int num = 0; //�ָ����·��������
	int i = 0;
	while (true) {
		while (state.curdir[i] == '/') {
			i++;
		}
		int p = 0;
		while (state.curdir[i] != '\0' && state.curdir[i] != '/') {
			tmp[num][p++] = state.curdir[i++];
		}
		tmp[num][p] = 0;
		num++;
		if (p == 1 && tmp[num - 1][0] == '.') {
			num--;
		}
		else if (p == 2 && tmp[num - 1][0] == '.' && tmp[num - 1][1] == '.') {
			num = num - 2;
			if (num < 0) {
				num = 0;
			}
		}
		if (state.curdir[i] == 0) {
			break;
		}
	}
	int offsetDir = 0; //·��ƫ��
	for (int i = 0; i < num; i++) {
		state.curdir[offsetDir++] = '/';
		int j = 0;
		while (tmp[i][j] != 0) {
			state.curdir[offsetDir++] = tmp[i][j++];
		}
	}
	if (num == 0) {
		state.curdir[offsetDir++] = '/';
	}
	state.curdir[offsetDir++] = '\0';
	for (int i = offsetDir; i < DirectoryEntry::DIRSIZE; i++) {
		state.curdir[i] = '\0';
	}
}

//�ı䵱ǰ����Ŀ¼
void FileManager::ChDir() {
	Inode* pInode;
	State& state = Kernel::Instance().GetState();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	pInode = this->NameI(NextChar, OPEN);
	if (NULL == pInode) {
		cout << "Ŀ¼������" << endl;
		return;
	}
	//������������ļ�����Ŀ¼�ļ�
	if (pInode->i_mode != 2) {
		inodeTable.IPut(pInode);
		cout << "�޷����ļ�ִ��cdָ��" << endl;
		return;
	}
	inodeTable.IPut(state.cdir);
	state.cdir = pInode;

	this->SetCurDir((char*)state.arg);
}

void FileManager::Link() {
	Inode* pInode;
	Inode* pNewInode;
	State& state = Kernel::Instance().GetState();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	pInode = this->NameI(NextChar, OPEN);
	//���ļ�ʧ��
	if (NULL == pInode) {
		return;
	}
	//���������Ѿ����
	if (pInode->i_nlink >= 255) {
		inodeTable.IPut(pInode);
		return;
	}
	state.dirp = (char*)state.arg;
	pNewInode = this->NameI(NextChar, CREATE);
	//����ļ��Ѿ�����
	if (NULL != pNewInode) {
		inodeTable.IPut(pNewInode);
	}
	this->WriteDir(pInode);
	pInode->i_nlink++;
	pInode->is_changed = true;
	inodeTable.IPut(pInode);
}

void FileManager::UnLink() {
	Inode* pInode;
	Inode* pDeleteInode;
	State& state = Kernel::Instance().GetState();
	InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
	pDeleteInode = this->NameI(NextChar, DELETE);
	if (NULL == pDeleteInode) {
		cout << "�ļ�������" << endl;
		return;
	}
	pInode = inodeTable.IGet(state.dent.m_ino);
	//д��������Ŀ¼��
	state.IOParam.m_Offset -= (DirectoryEntry::DIRSIZE + 4);
	state.IOParam.m_Base = (unsigned char*)&state.dent;
	state.IOParam.m_Count = DirectoryEntry::DIRSIZE + 4;
	state.dent.m_ino = 0;
	pDeleteInode->WriteI();
	//�޸�Inode��
	pInode->i_nlink--;
	pInode->is_changed = true;
	inodeTable.IPut(pInode);
	inodeTable.IPut(pDeleteInode);
}

void FileManager::rm() {
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	Space& space = Kernel::Instance().GetSpace();
	State& state = Kernel::Instance().GetState();

	char tmp[128];
	for (int i = 0; i < 128; i++) {
		tmp[i] = *(state.dirp + i);
		if (tmp[i] == 0) {
			break;
		}
	}
	Inode* pInode = NameI(NextChar, FileManager::OPEN);
	if (pInode == NULL) {
		cout << "Ŀ¼������" << endl;
		return;
	}
	if (pInode->i_mode == 1) {
		cout << "�޷����ļ�ִ��rmdirָ��" << endl;
		return;
	}
	//�����Ŀ¼����ݹ��ɾ�����ڲ��������ļ�����Ŀ¼
	else if (pInode->i_mode == 2) {
		int blockNum = pInode->i_size / Inode::BLOCK_SIZE;
		int j = 0;
		for (int i = 0; i < blockNum; i++) {
			int phyBlockNum = pInode->Bmap(i);
			Buf* buf = bufferManager.Bread(phyBlockNum);
			for (int l = 0; l < 16; l++) {
				if (j >= pInode->i_size / (sizeof(int) * 8)) {
					break;
				}
				if (j > 1) {
					int k = 0;
					if (*((int*)(buf->b_addr + 32 * (j % 16))) == 0) {
						j++;
						continue;
					}
					while (true) {
						space.pathParam[k] = *(buf->b_addr + 32 * (j % 16) + 4 + k);
						k++;
						if (*(buf->b_addr + 32 * (j % 16) + 4 + k) == 0) {
							break;
						}
					}
					state.dirp = space.pathParam;
					Remove();
				}
				j++;
			}
		}
	}
	state.dirp = tmp;
	UnLink();
}

void FileManager::Remove() {
	BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
	Space& space = Kernel::Instance().GetSpace();
	State& state = Kernel::Instance().GetState();

	char tmp[128];
	for (int i = 0; i < 128; i++) {
		tmp[i] = *(state.dirp + i);
		if (tmp[i] == 0) {
			break;
		}
	}
	Inode* pInode = NameI(NextChar, FileManager::OPEN);
	if (pInode == NULL) {
		cout << "�ļ�������" << endl;
		return;
	}
	if (pInode->i_mode == 2) {
		cout << "�޷���Ŀ¼ִ��fdeleteָ��" << endl;
		return;
	}
	state.dirp = tmp;
	UnLink();
}

DirectoryEntry::DirectoryEntry() {
	this->m_ino = 0;
	this->m_name[0] = '\0';
}

DirectoryEntry::~DirectoryEntry() {
	//do nothing
}