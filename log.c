#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <nfc/nfc.h>
#include <time.h>

#include "nfcTool.h"

#include <deque>
using namespace std;

//　总共存储的日志条数
#define MAX_LOG_SUM 100
// 输出　最近日志的条数
#define LASTEST_LOG_SUM 10
const char* LOG_FILE_NAME = "logfile.txt";
enum userType{
	Master_e = 1,
	Guest_e = 2,
	Unknow_e = 3
};
enum passType{
	Pass_e = 1,
	Reject_e =2
};
typedef struct logStruct
{
	char userName[255];
	userType user_t;
	passType pass_t;
	int userID;
	time_t rawtime;
} Log;
deque<Log> logQueue;


void saveLogtoFile();
int getALogByte(Log gg, uint8_t* outdata);

void addLog(const char* userName,int id,userType user_t,passType pass_t){
	if( logQueue.size()+1 > MAX_LOG_SUM){
		logQueue.front();
	}

	Log* newlog = (Log*)malloc(sizeof(Log));
	strcpy(newlog->userName, userName);
	newlog->userID = id;
	time(&(newlog->rawtime));
	newlog->user_t = user_t;
	newlog->pass_t = pass_t;
	logQueue.push_back( *newlog );

	free(newlog);
	saveLogtoFile();
}
const char* getUserType(userType t){
	if( t == Guest_e )
		return "Guest";
	if( t == Master_e )
		return "Master";
	if( t == Unknow_e )
		return "Unknow_e";
}
const char* getUserTypeWithColor(userType t){
	if( t == Guest_e )
		return "\033[1;34;40mGuest\033[0m";
	if( t == Master_e )
		return "\033[1;33;40mMaster\033[0m";
	if( t == Unknow_e )
		return "\033[1;31;40mUnknow_e\033[0m";
}

void printALog(Log* l){
	Log ll = *l;
	printf("%s %s--%s at %s",
		ll.userName,
		getUserTypeWithColor(ll.user_t),
		ll.pass_t==Pass_e?"\033[1;32;40mPass\033[0m":"\033[1;31;40mRejcet\033[0m",
		asctime(localtime(&ll.rawtime))
		);
}

void printALog(int i){
	if( i<logQueue.size() )
		printALog(&logQueue[i]);
}

void showRecentLog(){
	for( int i = logQueue.size()-1; i>=0 && logQueue.size()-i<=LASTEST_LOG_SUM; i--){
		printALog(i);
	}
}
void saveLogtoFile(){
	FILE *fp = NULL;
	fp = fopen( LOG_FILE_NAME , "wb");
	for( int i = 0; i<logQueue.size(); i++ ){
		int k = fwrite((void*)&logQueue[i],sizeof(logQueue[i]),1,fp);
	}
	fclose(fp);
}
void loadLogFromFile(){
	while ( !logQueue.empty() ){
		logQueue.clear();
	}

	FILE *fp = NULL;
	fp = fopen( LOG_FILE_NAME , "rb");
	Log buffer;size_t readlen;
	while( (readlen = fread((void*)&buffer,sizeof(Log),1,fp)) != 0 ){
		logQueue.push_back(buffer);
	}
	fclose(fp);
}
int getRecentLogByte(uint8_t* outdata,int maxlen){   // return outdata byte count 
	uint8_t buffer[100];int total=0;
	for( int i = 1; i<=LASTEST_LOG_SUM && (logQueue.size()-i>=0) ; i++ ){
		int ll = getALogByte(logQueue[ logQueue.size() -i ],buffer);
		if( total + ll >=maxlen){
			break;
		}
		memcpy(&outdata[total],buffer,ll);
		total += ll;
	}
	return total;
}
int getALogByte(Log gg, uint8_t* outdata){ 
/*
	|namelength:1byte|userName....|userType:4bit,passType:4bit|userID:1byte|rawtime:8byte|
	Accroding to ProcessOn..
*/
	outdata[0] = 0xff & strlen(gg.userName);
	strcpy((char*)&outdata[1],gg.userName);
	int p = 1 + outdata[0];
	outdata[p] = 0;
	outdata[p] |= ( (0xff & gg.user_t) << 4);
	outdata[p] |= (0x0f & gg.pass_t);
	p++;
	outdata[p++] = 0xff & gg.userID;
	memcpy(&outdata[p],(uint8_t*)&gg.rawtime,8);
	// (*(int*)(&outdata[p])) = (int )( 0x00 & gg.rawtime);
	// int k = -1;
	p+=8;
	return p;
}