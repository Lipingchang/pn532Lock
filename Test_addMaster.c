#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>

#include "apduBuffer.h"
#include "nfcTool.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
string MasterPwdFile = "masterdata.txt";

typedef struct MasterStruct
{
	string master_name;
	string master_pwd;
	string master_id;    
}Master;

// 用来保存 从文件中读取的 Masters
vector<Master> Masters;

void initMaster(){
	cout << "init Masters vector" << endl;
	ifstream myfile;
	myfile.open(MasterPwdFile,ios_base::in);
	
	// 有文件的话，就读出 记录 后 加入 Masters中  
	if( ! myfile ){
		cout << "MasterPwdFile not found! need file:" << MasterPwdFile << endl;
		return ;
	}
	//cout << "find " << Masters.size() << " Master(s) " << endl;

	string buff_name,buff_pwd; int buff_id;
	while( ! myfile.eof() ){
		myfile >> buff_id >> buff_name >> buff_pwd;
		cout << "<" << buff_id << ">" << endl;
		cout << "<" << buff_name << ">" << endl;
		cout << "<" << buff_pwd << ">" << endl;
	}

	myfile.close();
}
/*
MasterPwdFile 格式： 每行保存一个用户
master_id master_name master_pwd
*/
void writeMasterToFile(){

}

int main(){
	// string k = "kkkk";

	// char * kchar = (char*)k.c_str();
	// print_hex(kchar);
	initMaster();
}