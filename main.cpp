#define _CRT_SECURE_NO_WARNINGS
#include "Kernel.h"
#include <fstream>
#include <conio.h>
#include <string>
using namespace std;

//ȫ�ֱ��� ��ģ���ʵ������
FileManager& fileManager = Kernel::Instance().GetFileManager();
DiskManager& diskManager = Kernel::Instance().GetDiskManager();
State& state = Kernel::Instance().GetState();
FileSystem& fileSystem = Kernel::Instance().GetFileSystem();
InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
OpenFileTable& openFileTable = Kernel::Instance().GetOpenFileTable();
Space& space = Kernel::Instance().GetSpace();

//���û�������ַ���ָ��ת��Ϊ��Ӧ�Ĳ����Ͳ���������ָ֤���������Ч��
string transInstruction(string instruction) {
	string op;
	string tmp;
	State& state = Kernel::Instance().GetState();
	Space& space = Kernel::Instance().GetSpace();

	int segCount = 0; //����ָ���Կո�ֿ��Ķ���
	char* p = (char*)state.arg;
	for (int i = 0; i < instruction.length(); i++) {
		if (instruction[i] != ' ') {
			tmp += instruction[i];
		}
		if (instruction[i] == ' ' || i == instruction.length() - 1) {
			if (segCount == 0) {//��������
				op = tmp;
			}
			else if (segCount >= 1) {//����
				for (int j = 0; j < tmp.length(); j++) {
					*(p++) = tmp[j];
					if (j == tmp.length() - 1) {
						*(p++) = 0;
					}
				}
			}
			segCount++;
			tmp.clear();
		}
	}
	//�������������������ƥ������
	if (op == "help" && segCount != 1) {
		cout << op << "������������" << endl;
		return "error";
	}
	else if (op == "ls" && segCount != 1 && segCount != 2) {
		cout << op << "������������" << endl;
		return "error";
	}
	else if ((op == "cd" || op == "mkdir" || op == "rmdir" || op == "fdelete" || op == "fopen" || op == "fclose" || op == "fcreat" || op == "fshow") && segCount != 2) {
		cout << op << "������������" << endl;
		return "error";
	}
	else if ((op == "exit" || op == "fformat") && segCount != 1) {
		cout << op << "������������" << endl;
		return "error";
	}
	else if (op == "flseek" && segCount != 4) {
		cout << op << "������������" << endl;
		return "error";
	}
	else if ((op == "fread" || op == "fwrite" || op == "fin" || op == "fout") && segCount != 3) {
		cout << op << "������������" << endl;
		return "error";
	}

	//����ָ��Ĳ�������
	if (op == "ls" && segCount == 1) {
		*((char*)state.arg) = '\0';
	}
	//��һ������
	if (op == "ls" || op == "cd" || op == "mkdir" || op == "rmdir" || op == "fdelete" || op == "fopen" || op == "fclose" || op == "flseek" || op == "fread" || op == "fwrite" || op == "fcreat" || op == "fin" || op == "fout" || op == "fshow") {
		p = (char*)state.arg;
		for (int j = 0; true; j++) {
			space.pathParam[j] = *(p++);
			if (space.pathParam[j] == 0) {
				break;
			}
		}
		state.dirp = space.pathParam;
	}
	//�ڶ�������
	if (op == "flseek" || op == "fread" || op == "fwrite") {
		int fd = 0;
		int offset = 0;
		//���δ�ҵ���Ӧ�� Inode �ṹָ�룬ָ����Ч
		Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
		if (pInode == NULL) {
			cout << "�ļ�������" << endl;
			return "error";
		}
		//�������ļ����ҵ��뵱ǰ Inode ��������ļ������� fd
		for (int i = 0; i < OpenFiles::NOFILES; i++) {
			File* tmp = state.ofiles.OpenFileTable[i];
			if (tmp != NULL && tmp->is_av == false && tmp->f_inode->i_number == pInode->i_number) {
				fd = i;
				break;
			}
		}
		//���������ַ���������ת��Ϊ�������͵�ƫ����
		while (*p != 0) {
			offset = offset * 10 + (*p - 48);
			p++;
		}
		state.arg[0] = fd;
		state.arg[1] = offset;
		
	}
	//����������
	if (op == "flseek") {
		int choice = 0;
		p++;
		string tmp;
		while (*p != 0) {
			tmp = tmp + *p;
			p++;
		}
		if (tmp == "SEEK_SET") {
			choice = 0;
		}
		else if (tmp == "SEEK_CUR") {
			choice = 1;
		}
		else if (tmp == "SEEK_END") {
			choice = 2;
		}
		else {
			cout << op << "�������ݴ���" << endl;
			return "error";
		}
		state.arg[2] = choice;
	}
	//�ڶ�������
	if (op == "fin" || op == "fout") {
		char* i = (char*)state.arg;
		while (true) {
			*(i++) = *(p++);
			if (*(i - 1) == 0) {
				break;
			}
		}
		if (op == "fin") {
			state.dirp = (char*)state.arg;
		}
	}

	return op;
}

void help() {
	cout << "fformat                        ���̸�ʽ��" << endl;
	cout << "   fformat                   : ���̸�ʽ��" << endl;
	cout << "ls                             �鿴Ŀ¼�µ�����Ŀ¼���ļ�" << endl;
	cout << "   ls                        : �鿴��ǰĿ¼�µ�����Ŀ¼���ļ�" << endl;
	cout << "   ls [path]                 : �鿴ָ��Ŀ¼�µ�����Ŀ¼���ļ�" << endl;
	cout << "cd                             �л�Ŀ¼" << endl;
	cout << "   cd /                      : �л�����Ŀ¼" << endl;
	cout << "   cd [xxxx]                 : �л�����ǰĿ¼�µ�xxxxĿ¼" << endl;
	cout << "   cd [path]                 : �л���ָ��Ŀ¼" << endl;
	cout << "mkdir                          ����Ŀ¼" << endl;
	cout << "   mkdir [path]              : ����Ŀ¼" << endl;
	cout << "rmdir                          ɾ��Ŀ¼" << endl;
	cout << "   rmdir [path]              : ɾ��Ŀ¼" << endl;
	cout << "fcreat                         �����ļ�" << endl;
	cout << "   fcreat [path]             : �����ļ�" << endl;
	cout << "fdelete                        ɾ���ļ�" << endl;
	cout << "   fdelete [path]            : ɾ���ļ�" << endl;
	cout << "fopen                          ���ļ�" << endl;
	cout << "   fopen [path]              : ���ļ�" << endl;
	cout << "fclose                         �ر��ļ�" << endl;
	cout << "   fclose [path]             : �ر��ļ�" << endl;
	cout << "fshow                          �鿴�ļ�" << endl;
	cout << "   fshow [path]              : �鿴�ļ�����" << endl;
	cout << "fread                          ��ȡ�ļ�" << endl;
	cout << "   fread [path] [num]        : ���ļ���num�ֽڶ��붯̬������" << endl;
	cout << "fwrite                         д���ļ�" << endl;
	cout << "   fwrite [path] [num]       : ����̬��������num�ֽ�д���ļ�" << endl;
	cout << "flseek                         �ƶ��ļ��Ķ�дָ��" << endl;
	cout << "   flseek [path] [num] [mode]: ���ļ��Ķ�дָ���mode�����ƶ�num�ֽ�" << endl;
	cout << "   modeΪSEEK_SET��ʾ���ļ�ͷ��ʼ��SEEK_CUR�ӵ�ǰ��ʼ��SEEK_END���ļ�ĩβ��ʼ" << endl;
	cout << "fin                            ���ⲿ�ļ����뵽ϵͳ��ĳ�ļ�" << endl;
	cout << "   fin [path1] [path2]       : ���ⲿ�ļ�path1�����ݶ��뵽ϵͳ�ڲ��ļ�path2" << endl;
	cout << "fout                           ��ϵͳ��ĳ�ļ�������ⲿ�ļ�" << endl;
	cout << "   fout [path1] [path2]      : ��ϵͳ�ڲ��ļ�path1������д�����ⲿ�ļ�path2" << endl;
	cout << "exit                           �˳�ϵͳ" << endl;
	cout << "   exit                      : �˳�ϵͳ" << endl;
}

//���ļ�ϵͳ���и�ʽ��
void format() {
	fileManager.FormatSystem();
	//����Ϊ��ʼ״̬����ո�ʽ��ʱ�õ����ڴ��е����ݽṹ
	fileManager.ResetFileManager();
	state.ResetState();
	inodeTable.ResetInodeTable();
	bufferManager.ResetBufferManager();
	superBlock.ResetSuperBlock();
	openFileTable.ResetOpenFileTable();
	space.ResetSpace();
}

//��ָ���ļ�
void open() {
	fileManager.Open();
}

//�ر�ָ���ļ�
void close() {
	bool pd = false;
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	for (int i = 0; i < OpenFiles::NOFILES; i++) {
		File* tmp = state.ofiles.OpenFileTable[i];
		if (tmp != NULL && tmp->is_av == false && tmp->f_inode->i_number == pInode->i_number) {
			state.arg[0] = i;
			fileManager.Close();
			pd = true;
			break;
		}
	}
	if (!pd)cout << "�ļ�������" << endl;
}

//�ƶ��ļ���дָ�뵽ָ��λ��
void lseek() {
	fileManager.Seek();
}

//���ļ��ж�ȡ����
void read() {
	fileManager.Read();
}

//������д���ļ�
void write() {
	fileManager.Write();
}

//�������ļ�
void create() {
	fileManager.Creat();
}

//��ȡ�ļ����ݲ����ն����
void show() {
	fileManager.Open();
	state.dirp = space.pathParam;
	int fd = 0;
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	if (pInode == NULL || pInode->i_mode == 2) {
		if (pInode == NULL)return;
		else cout << "�޷���Ŀ¼ִ��fshowָ��" << endl;
		return;
	}
	for (int i = 0; i < OpenFiles::NOFILES; i++) {
		File* tmp = state.ofiles.OpenFileTable[i];
		if (tmp != NULL && tmp->is_av == false && tmp->f_inode->i_number == pInode->i_number) {
			fd = i;
			break;
		}
	}
	int lSize = pInode->i_size;
	state.arg[0] = fd;
	state.arg[1] = lSize;
	fileManager.Read();
	for (int i = 0; i < lSize; i++) {
		cout << char(*(space.buffer + i));
	}
	cout << endl;
	fileManager.Close();
}

//ɾ���ļ�
void deletee() {
	fileManager.Remove();
}

//�г���ǰĿ¼�µ��ļ���Ŀ¼��
void ls() {
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	if (pInode == NULL || pInode->i_mode == 1) {
		if (pInode == NULL)cout << "Ŀ¼������" << endl;
		else cout << "�޷����ļ�ִ��lsָ��" << endl;
		return;
	}
	int blockNum = pInode->i_size / Inode::BLOCK_SIZE;
	if (pInode->i_size % Inode::BLOCK_SIZE > 0) {
		blockNum++;
	}
	//��ȡ�����Ŀ¼��
	int j = 0;
	int neg = 0;
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
					neg--;
					continue;
				}
				while (true) {
					if (*(buf->b_addr + 32 * (j % 16) + 4 + k) == 0) {
						break;
					}
					cout << *(buf->b_addr + 32 * (j % 16) + 4 + k);
					k++;
				}
				cout << " ";
			}
			j++;
		}
	}
	if (j + neg == 2) {
		cout << "��Ŀ¼�²���������Ŀ¼���ļ�";
	}
	cout << endl;
}

//�л���ָ��Ŀ¼
void cd() {
	fileManager.ChDir();
}

//������Ŀ¼
void mkdir() {
	fileManager.NameI(fileManager.NextChar, FileManager::CREATE);
	fileManager.MakNode(2);
	return;
}

//ɾ��Ŀ¼
void rmdir() {
	fileManager.rm();
}

//���ⲿ�ļ�ϵͳ�����ļ����������ļ�ϵͳ��
void fin() {
	//��Windowsϵͳ�е��ļ�
	FILE* pFile;
	pFile = fopen(space.pathParam, "rb");
	if (pFile == NULL) {
		cout << "�ⲿ�ļ�������" << endl;
		return;
	}
	//��ȡ�ļ���С
	fseek(pFile, 0, SEEK_END);
	int lSize = ftell(pFile);
	rewind(pFile);
	//���ļ���ȡ���ڴ�
	fread(space.buffer, 1, lSize, pFile);
	space.m_nbytes = lSize;

	fileManager.Creat();
	state.dirp = (char*)state.arg;
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	int fd = 0;
	for (int i = 0; i < OpenFiles::NOFILES; i++) {
		File* tmp = state.ofiles.OpenFileTable[i];
		if (tmp != NULL && tmp->is_av == false && tmp->f_inode->i_number == pInode->i_number) {
			fd = i;
			break;
		}
	}
	state.arg[0] = fd;
	state.arg[1] = lSize;
	fileManager.Write();
	fclose(pFile);
}

//���ļ��ӱ������ļ�ϵͳ�������ⲿ�ļ�ϵͳ��
void fout() {
	FILE* pFile;
	pFile = fopen((char*)state.arg, "wb");
	fileManager.Open();
	state.dirp = space.pathParam;
	int fd = 0;
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	if (pInode == NULL || pInode->i_mode == 2) {
		if (pInode == NULL)cout << "�ڲ��ļ�������" << endl;
		else cout << "�޷���Ŀ¼ִ��foutָ��" << endl;
		return;
	}
	for (int i = 0; i < OpenFiles::NOFILES; i++) {
		File* tmp = state.ofiles.OpenFileTable[i];
		if (tmp != NULL && tmp->is_av == false && tmp->f_inode->i_number == pInode->i_number) {
			fd = i;
			break;
		}
	}
	int lSize = pInode->i_size;
	state.arg[0] = fd;
	state.arg[1] = lSize;
	fileManager.Read();
	fwrite(space.buffer, lSize, 1, pFile);
	fileManager.Close();
	fclose(pFile);
}

//�˳�ϵͳ�������ļ�ϵͳ״̬
void exit() {
	fileSystem.Update();
	inodeTable.UpdateInodeTable();
	bufferManager.Bflush();
}

/*
����Ƿ�����ļ�ϵͳ�ľ����ļ������û����ѯ���û��Ƿ���и�ʽ����
ϵͳ��ʼ�������볬���飬����� Inode ���ڴ棬�����õ�ǰĿ¼��
���뽻��ʽָ��ѭ�����û��������벻ͬ���ļ��������ͨ�� transInstruction ת����������Ӧ�Ĺ��ܺ�����
*/
int main() {
	ifstream f("myDisk.img");
	if (!f.good()) {
		cout << "No disk��create and format? (y/n)" << endl;
		char choice;
		while (true) {
			choice = _getch();
			if (choice == 'y') {
				diskManager.createDisk();
				format();
				break;
			}
			else if (choice == 'n') {
				cout << "���̲�����" << endl;
				return 0;
			}
		}
	}
	else diskManager.openDisk();

	//ϵͳ��ʼ��
	fileSystem.LoadSuperBlock(); //���볬����
	fileManager.rootDirInode = inodeTable.IGet(1); //�����Inode���ڴ�
	state.cdir = fileManager.rootDirInode;//���õ�ǰĿ¼
	
	string instruction;
	while (true) {
		cout << state.curdir << ":>";
		getline(cin, instruction);
		if (instruction == "")continue;
		string op = transInstruction(instruction);
		if (op == "help") {
			help();
		}
		else if (op == "fopen") {
			open();
		}
		else if (op == "fclose") {
			close();
		}
		else if (op == "flseek") {
			lseek();
		}
		else if (op == "fread") {
			read();
		}
		else if (op == "fwrite") {
			write();
		}
		else if (op == "fcreat") {
			create();
		}
		else if (op == "fshow") {
			show();
		}
		else if (op == "fdelete") {
			deletee();
		}
		else if (op == "ls") {
			ls();
		}
		else if (op == "cd") {
			cd();
		}
		else if (op == "mkdir") {
			mkdir();
		}
		else if (op == "rmdir") {
			rmdir();
		}
		else if (op == "fin") {
			fin();
		}
		else if (op == "fout") {
			fout();
		}
		else if (op == "exit") {
			exit();
			break;
		}
		else if (op == "fformat") {
			format();
			fileSystem.LoadSuperBlock(); //���볬����
			fileManager.rootDirInode = inodeTable.IGet(1); //�����Inode���ڴ�
			state.cdir = fileManager.rootDirInode;
		}
		else if (op != "error") {
			cout << "��ϵͳ��֧��" << op << "ָ��" << endl;
		}
	}
	cout << "���˳��ļ�ϵͳ" << endl;

	return 0;
}