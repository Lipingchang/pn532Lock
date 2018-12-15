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
	
	// char userName[255];
	// userType user_t;
	// passType pass_t;
	// int userID;
	// time_t rawtime;

void writeLogToFile(FILE* fp,Log* alog){
	fprintf(fp, "%s\n", alog->userName);
	fprintf(fp, "%d %d %d %ld\n", alog->user_t, alog->pass_t, alog->userID, alog->rawtime);
}
void saveLogtoFile(){
	FILE *fp = NULL;
	fp = fopen( LOG_FILE_NAME , "w+");
	fprintf(fp, "%d\n", logQueue.size() );
	for( int i = 0; i<logQueue.size(); i++ ){
		//int k = fwrite((void*)&logQueue[i],sizeof(logQueue[i]),1,fp);
		writeLogToFile(fp,&logQueue[i]);
	}
	fclose(fp);
}
void loadLogFromFile(){
	while ( !logQueue.empty() ){
		logQueue.clear();
	}

	FILE *fp = NULL;
	fp = fopen( LOG_FILE_NAME , "r");
	Log buffer;size_t readlen;
	int logcount = 0;

	fscanf( fp, "%d\n", &logcount );
	for( int i =0; i<logcount; i++ ){
		size_t ll;
		fgets((char*)buffer.userName,255,fp);
		buffer.userName[strlen((char*)buffer.userName)-1] = '\0';
		fscanf(fp,"%d %d %d %ld\n",&(buffer.user_t), &(buffer.pass_t), &(buffer.userID), &(buffer.rawtime));
		logQueue.push_back(buffer);
	}
	fclose(fp);
}
int getRecentLogByte(uint8_t* outdata,int maxlen){   // return outdata byte count 
	uint8_t buffer[100];int total=0;
	for( int i = logQueue.size()-1; i>=0 && logQueue.size()-i<=LASTEST_LOG_SUM; i-- ){
		printf("%d>>>:", i );
		int ll = getALogByte(logQueue[ i ],buffer);
		if( total + ll >=maxlen){
			break;
		}
		printf("\t\tmove <%d>data from buf to data\n",ll);
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
	printf("\t  setting username len\n");
	outdata[0] = 0xff & strlen(gg.userName);
	printf("\t\tcopying username, len:%d\n",outdata[0]);
	strcpy((char*)&outdata[1],gg.userName);
	int p = 1 + outdata[0];
	printf("\t\tsetting bits p:<%d>\n",p);
	outdata[p] = 0;
	outdata[p] |= ( (0xff & gg.user_t) << 4);
	outdata[p] |= (0x0f & gg.pass_t);
	p++;
	printf("\t\tsetting userid\n");
	outdata[p++] = 0xff & gg.userID;
	printf("\t\tcopying rawtime\n");
	memcpy(&outdata[p],(uint8_t*)&gg.rawtime,8);
	p+=8;
	printf("\t\treturning p:<%d>\n", p);
	return p;
}