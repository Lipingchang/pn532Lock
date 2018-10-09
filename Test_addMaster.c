#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>

#include "apduBuffer.h"
#include "nfcTool.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


typedef struct MasterStruct
{
	string master_name;
	string master_pwd;
	string master_id;
}Master;

vector<Master,10> Masters;

void initMaster(){
	ifstream myfile;
	myfile.open("masterdata.txt");
	
	if( myfile ){
		printf("ok\n");
	}else{
		printf("fuck\n");
	}
	myfile.close();
}
void writeMasterToFile(){

}

int main(){
	string k = "kkkk";

	char * kchar = (char*)k.c_str();
	print_hex(kchar);
	//initMaster();
}