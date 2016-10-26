// Jenna Wilson (jwilso27) & Madelyn Nelson (mnelso12)
// CSE 30264
// Programming Assignment 3
// 10/12/16

// include librraries
#include <stdio.h>   
#include <stdlib.h>  
#include <unistd.h>
#include <string.h>  
#include <sys/types.h>   
#include <sys/socket.h>  
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <bits/stat.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>   
#include <netdb.h>   
#include <dirent.h>
#include <openssl/md5.h>

#define MAX_LINE 4096 

// function prototypes
void my_send( int, void*, size_t, int );
void my_recv( int, void*, size_t, int );
void string_recv( int, char*, int );
void file_recv( int, int, FILE* );
short int md5_compute( int, char*, unsigned char*, FILE* );
int md5_cmp( char*, char* );

// send with error checking
void my_send( int s, void* buf, size_t size, int flag ) {
    if ( send( s, buf, size, flag ) == -1 ) {    
        perror("send error");
        exit(1);
    }
}

// recv with error checking
// (only use when return value is not used)
void my_recv( int s, void* buf, size_t size, int flag ) {
    if ( recv( s, buf, size, flag ) == -1 ) {    
        perror("receive error");
        exit(1);
    }
}

// recv length and then string in chunks
void string_recv( int s, char* buf, int flag ) {
    int tmp_len, bufsize = 0;
    short int len;
    char tmp_buf[16];

    // receive string length from server
    my_recv( s, &len, sizeof(len), flag );

    // receive string from server
    bzero( buf, sizeof(buf) );
    while ( bufsize < len ) {
        bzero( tmp_buf, sizeof(tmp_buf) );
        if ( ( tmp_len = recv( s, tmp_buf, sizeof(tmp_buf), flag ) ) == -1 ) {
            perror("server receive error");
            exit(1);
        } else bufsize += tmp_len;
        strcat( buf, tmp_buf );
    }
}

// recv file
void file_recv( int s, int size, FILE *fp ) {
    int len, tmp_size = 0;
    char buf[MAX_LINE];

    do {
        bzero( buf, sizeof(buf) );
        if ( size - tmp_size < sizeof(buf) ) {
            len = recv( s, buf, ( size - tmp_size ), 0 );
        } else {
            len = recv( s, buf, sizeof(buf), 0 );
        }
        if ( len == -1 ) {
            perror("receive error");
            exit(1);
        }
        fwrite( buf, sizeof(char), len, fp ); 
    } while ( ( tmp_size += len ) < size );
}

// compute md5 hash
short int md5_compute( int s, char *buf, unsigned char* digest, FILE *fp ) {
    int len;
    MD5_CTX mdContext;

    MD5_Init (&mdContext);
    do {
        bzero( buf, sizeof(buf) );
        len = fread ( buf, sizeof(char), sizeof(buf), fp );
        MD5_Update( &mdContext, buf, len );
    } while ( len != 0 );
    MD5_Final ( digest, &mdContext );
    return MD5_DIGEST_LENGTH;
}

// compare md5 hashes
int md5_cmp( char *buf, char *digest ) {
    int i;

    for ( i = 0; i < MD5_DIGEST_LENGTH; i++ ) {
        if ( buf[i] != digest[i] ) {
            return 0;
        }
    }
    return 1;
}
