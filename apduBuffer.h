#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>
 
#define APDU_MAX_LEN 256
 
typedef  struct apdubufferStruct{
	uint8_t *buff;	
	int length;
} apduBuffer;

void initApduBuffer(apduBuffer *buffer){
	buffer->buff  = (uint8_t *)malloc(sizeof(uint8_t)*APDU_MAX_LEN);
	buffer->length = 0;
}
// insert a byte into a buffer at pos position
bool insertApduBufferByte(apduBuffer *buffer,const uint8_t newone,int pos){
	if( buffer->length+1 > APDU_MAX_LEN ){
		printf("insert byte ApduBuffer fail! out of buffer size!\n");
		return false;
	}
	if( pos > buffer->length ){
		printf("insert byte ApduBuffer fail! pos not in range!\n");
		return false;
	}

	for( int i = buffer->length ; i>=pos ; i-- ){
		buffer->buff[i] = buffer->buff[i-1];
	}
	buffer->buff[pos] = newone;
	buffer->length ++;
	return true;

}
bool insertApduBufferBytes( apduBuffer *buffer,const uint8_t *byte_array,int array_len, int pos){
	int* len = &(buffer->length);
	uint8_t *buff = buffer->buff;
	if( *len + array_len > APDU_MAX_LEN){
		printf("insert bytes ApduBuffer fail! out of buffer size\n");
		return false;
	}
	if( pos > *len ){
		printf("insert bytes ApduBuffer fail! pos not in range\n");
		return false;
	}

	for( int i = *len-1; i>=pos; i-- ){
		buff[i + array_len ] = buff[i];
	}
	for( int i = 0; i<array_len; i++ ){
		buff[i+pos] = byte_array[i]; 
	}

	*len = *len + array_len;
	return false;
}
bool appendApduBufferByte(apduBuffer *buffer,  const uint8_t newone){
	if( buffer->length+1 > APDU_MAX_LEN ){
		printf("append byte ApduBuffer fail! out of buffer size!\n");
		return false;
	}
	buffer->buff[buffer->length] = newone;
	buffer->length ++ ;
}
bool appendApduBufferBytes(apduBuffer *buffer,const uint8_t *byte_array,int array_len){
	if( buffer->length + array_len > APDU_MAX_LEN ){
		printf("append ApduBuffer fail! out of buffer size!\n");
		return false;
	}

	for( int i = 0 ; i < array_len ; i++ ){
		buffer->buff[i+buffer->length] = byte_array[i];
	}
	buffer->length += array_len;
	return true;
}
void showApduBuffer(apduBuffer *buffer){
	for( int i =0; i<buffer->length; i++ ){
		printf("%02x ",buffer->buff[i]);
	}
	printf("\n");
}
void getApduArray(apduBuffer *buffer, uint8_t** apdu_array,int* length){
	*apdu_array = ( uint8_t* ) malloc(sizeof(uint8_t) * buffer->length);
 	*length = buffer->length;
	for( int i = 0; i< buffer->length; i++ ){
 		(*apdu_array)[i] = buffer->buff[i];
 	}
}