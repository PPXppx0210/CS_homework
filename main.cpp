#define _CRT_SECURE_NO_WARNINGS
#include "Kernel.h"
#include <fstream>
#include <conio.h>
#include <string>
using namespace std;

//全局变量 各模块的实例引用
FileManager& fileManager = Kernel::Instance().GetFileManager();
DiskManager& diskManager = Kernel::Instance().GetDiskManager();
State& state = Kernel::Instance().GetState();
FileSystem& fileSystem = Kernel::Instance().GetFileSystem();
InodeTable& inodeTable = Kernel::Instance().GetInodeTable();
BufferManager& bufferManager = Kernel::Instance().GetBufferManager();
SuperBlock& superBlock = Kernel::Instance().GetSuperBlock();
OpenFileTable& openFileTable = Kernel::Instance().GetOpenFileTable();
Space& space = Kernel::Instance().GetSpace();

//将用户输入的字符串指令转化为对应的操作和参数，并验证指令参数的有效性
string transInstruction(string instruction) {
	string op;
	string tmp;
	State& state = Kernel::Instance().GetState();
	Space& space = Kernel::Instance().GetSpace();

	int segCount = 0; //计数指令以空格分开的段数
	char* p = (char*)state.arg;
	for (int i = 0; i < instruction.length(); i++) {
		if (instruction[i] != ' ') {
			tmp += instruction[i];
		}
		if (instruction[i] == ' ' || i == instruction.length() - 1) {
			if (segCount == 0) {//操作类型
				op = tmp;
			}
			else if (segCount >= 1) {//参数
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
	//操作类型与参数个数不匹配的情况
	if (op == "help" && segCount != 1) {
		cout << op << "参数个数错误" << endl;
		return "error";
	}
	else if (op == "ls" && segCount != 1 && segCount != 2) {
		cout << op << "参数个数错误" << endl;
		return "error";
	}
	else if ((op == "cd" || op == "mkdir" || op == "rmdir" || op == "fdelete" || op == "fopen" || op == "fclose" || op == "fcreat" || op == "fshow") && segCount != 2) {
		cout << op << "参数个数错误" << endl;
		return "error";
	}
	else if ((op == "exit" || op == "fformat") && segCount != 1) {
		cout << op << "参数个数错误" << endl;
		return "error";
	}
	else if (op == "flseek" && segCount != 4) {
		cout << op << "参数个数错误" << endl;
		return "error";
	}
	else if ((op == "fread" || op == "fwrite" || op == "fin" || op == "fout") && segCount != 3) {
		cout << op << "参数个数错误" << endl;
		return "error";
	}

	//特殊指令的参数处理
	if (op == "ls" && segCount == 1) {
		*((char*)state.arg) = '\0';
	}
	//第一个参数
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
	//第二个参数
	if (op == "flseek" || op == "fread" || op == "fwrite") {
		int fd = 0;
		int offset = 0;
		//如果未找到相应的 Inode 结构指针，指令无效
		Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
		if (pInode == NULL) {
			cout << "文件不存在" << endl;
			return "error";
		}
		//遍历打开文件表，找到与当前 Inode 相关联的文件描述符 fd
		for (int i = 0; i < OpenFiles::NOFILES; i++) {
			File* tmp = state.ofiles.OpenFileTable[i];
			if (tmp != NULL && tmp->is_av == false && tmp->f_inode->i_number == pInode->i_number) {
				fd = i;
				break;
			}
		}
		//解析参数字符串，将其转换为整数类型的偏移量
		while (*p != 0) {
			offset = offset * 10 + (*p - 48);
			p++;
		}
		state.arg[0] = fd;
		state.arg[1] = offset;
		
	}
	//第三个参数
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
			cout << op << "参数内容错误" << endl;
			return "error";
		}
		state.arg[2] = choice;
	}
	//第二个参数
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
	cout << "fformat                        磁盘格式化" << endl;
	cout << "   fformat                   : 磁盘格式化" << endl;
	cout << "ls                             查看目录下的所有目录和文件" << endl;
	cout << "   ls                        : 查看当前目录下的所有目录和文件" << endl;
	cout << "   ls [path]                 : 查看指定目录下的所有目录和文件" << endl;
	cout << "cd                             切换目录" << endl;
	cout << "   cd /                      : 切换到根目录" << endl;
	cout << "   cd [xxxx]                 : 切换到当前目录下的xxxx目录" << endl;
	cout << "   cd [path]                 : 切换到指定目录" << endl;
	cout << "mkdir                          创建目录" << endl;
	cout << "   mkdir [path]              : 创建目录" << endl;
	cout << "rmdir                          删除目录" << endl;
	cout << "   rmdir [path]              : 删除目录" << endl;
	cout << "fcreat                         创建文件" << endl;
	cout << "   fcreat [path]             : 创建文件" << endl;
	cout << "fdelete                        删除文件" << endl;
	cout << "   fdelete [path]            : 删除文件" << endl;
	cout << "fopen                          打开文件" << endl;
	cout << "   fopen [path]              : 打开文件" << endl;
	cout << "fclose                         关闭文件" << endl;
	cout << "   fclose [path]             : 关闭文件" << endl;
	cout << "fshow                          查看文件" << endl;
	cout << "   fshow [path]              : 查看文件内容" << endl;
	cout << "fread                          读取文件" << endl;
	cout << "   fread [path] [num]        : 将文件的num字节读入动态缓存区" << endl;
	cout << "fwrite                         写入文件" << endl;
	cout << "   fwrite [path] [num]       : 将动态缓存区的num字节写入文件" << endl;
	cout << "flseek                         移动文件的读写指针" << endl;
	cout << "   flseek [path] [num] [mode]: 将文件的读写指针从mode往后移动num字节" << endl;
	cout << "   mode为SEEK_SET表示从文件头开始，SEEK_CUR从当前开始，SEEK_END从文件末尾开始" << endl;
	cout << "fin                            将外部文件输入到系统中某文件" << endl;
	cout << "   fin [path1] [path2]       : 将外部文件path1的内容读入到系统内部文件path2" << endl;
	cout << "fout                           将系统中某文件输出到外部文件" << endl;
	cout << "   fout [path1] [path2]      : 将系统内部文件path1的内容写出到外部文件path2" << endl;
	cout << "exit                           退出系统" << endl;
	cout << "   exit                      : 退出系统" << endl;
}

//对文件系统进行格式化
void format() {
	fileManager.FormatSystem();
	//重置为初始状态，清空格式化时用到的内存中的数据结构
	fileManager.ResetFileManager();
	state.ResetState();
	inodeTable.ResetInodeTable();
	bufferManager.ResetBufferManager();
	superBlock.ResetSuperBlock();
	openFileTable.ResetOpenFileTable();
	space.ResetSpace();
}

//打开指定文件
void open() {
	fileManager.Open();
}

//关闭指定文件
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
	if (!pd)cout << "文件不存在" << endl;
}

//移动文件读写指针到指定位置
void lseek() {
	fileManager.Seek();
}

//从文件中读取数据
void read() {
	fileManager.Read();
}

//将数据写入文件
void write() {
	fileManager.Write();
}

//创建新文件
void create() {
	fileManager.Creat();
}

//读取文件内容并在终端输出
void show() {
	fileManager.Open();
	state.dirp = space.pathParam;
	int fd = 0;
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	if (pInode == NULL || pInode->i_mode == 2) {
		if (pInode == NULL)return;
		else cout << "无法对目录执行fshow指令" << endl;
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

//删除文件
void deletee() {
	fileManager.Remove();
}

//列出当前目录下的文件和目录项
void ls() {
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	if (pInode == NULL || pInode->i_mode == 1) {
		if (pInode == NULL)cout << "目录不存在" << endl;
		else cout << "无法对文件执行ls指令" << endl;
		return;
	}
	int blockNum = pInode->i_size / Inode::BLOCK_SIZE;
	if (pInode->i_size % Inode::BLOCK_SIZE > 0) {
		blockNum++;
	}
	//读取并输出目录项
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
		cout << "该目录下不存在其它目录或文件";
	}
	cout << endl;
}

//切换到指定目录
void cd() {
	fileManager.ChDir();
}

//创建新目录
void mkdir() {
	fileManager.NameI(fileManager.NextChar, FileManager::CREATE);
	fileManager.MakNode(2);
	return;
}

//删除目录
void rmdir() {
	fileManager.rm();
}

//从外部文件系统导入文件到本二级文件系统中
void fin() {
	//打开Windows系统中的文件
	FILE* pFile;
	pFile = fopen(space.pathParam, "rb");
	if (pFile == NULL) {
		cout << "外部文件不存在" << endl;
		return;
	}
	//获取文件大小
	fseek(pFile, 0, SEEK_END);
	int lSize = ftell(pFile);
	rewind(pFile);
	//将文件读取到内存
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

//将文件从本二级文件系统导出到外部文件系统中
void fout() {
	FILE* pFile;
	pFile = fopen((char*)state.arg, "wb");
	fileManager.Open();
	state.dirp = space.pathParam;
	int fd = 0;
	Inode* pInode = fileManager.NameI(fileManager.NextChar, FileManager::OPEN);
	if (pInode == NULL || pInode->i_mode == 2) {
		if (pInode == NULL)cout << "内部文件不存在" << endl;
		else cout << "无法对目录执行fout指令" << endl;
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

//退出系统，保存文件系统状态
void exit() {
	fileSystem.Update();
	inodeTable.UpdateInodeTable();
	bufferManager.Bflush();
}

/*
检查是否存在文件系统的镜像文件，如果没有则询问用户是否进行格式化。
系统初始化：载入超级块，读入根 Inode 到内存，并设置当前目录。
进入交互式指令循环，用户可以输入不同的文件操作命令，通过 transInstruction 转换并调用相应的功能函数。
*/
int main() {
	ifstream f("myDisk.img");
	if (!f.good()) {
		cout << "No disk，create and format? (y/n)" << endl;
		char choice;
		while (true) {
			choice = _getch();
			if (choice == 'y') {
				diskManager.createDisk();
				format();
				break;
			}
			else if (choice == 'n') {
				cout << "磁盘不存在" << endl;
				return 0;
			}
		}
	}
	else diskManager.openDisk();

	//系统初始化
	fileSystem.LoadSuperBlock(); //载入超级块
	fileManager.rootDirInode = inodeTable.IGet(1); //读入根Inode到内存
	state.cdir = fileManager.rootDirInode;//设置当前目录
	
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
			fileSystem.LoadSuperBlock(); //载入超级块
			fileManager.rootDirInode = inodeTable.IGet(1); //读入根Inode到内存
			state.cdir = fileManager.rootDirInode;
		}
		else if (op != "error") {
			cout << "本系统不支持" << op << "指令" << endl;
		}
	}
	cout << "已退出文件系统" << endl;

	return 0;
}