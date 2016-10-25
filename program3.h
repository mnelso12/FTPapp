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
#include <bits/stat.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>   
#include <netdb.h>   
#include <dirent.h>
#include <openssl/md5.h>

// function prototypes
void my_send( int, void*, size_t, int );
void my_recv( int, void*, size_t, int );
void string_send( int, char*, short int, int );
void string_recv( int, char*, int );
short int md5_compute( int, char*, unsigned char*, FILE* );

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

// send length and then string in chunks
// void string_send( int s, char* buf, short int len, int flag ) {
//     // short int len = strlen(buf) + 1;
//     int bufsize = 0;
//     char tmp_buf[16];
// 
//     // send string length
//     my_send( s, &len, sizeof(len), flag );
//     printf("len: %d\n",len);
// 
//     // send command
//     while ( bufsize < len ) {
//         bzero( tmp_buf, sizeof(tmp_buf) );
//         strncpy( tmp_buf, &buf[bufsize], 15 );
//         printf("%s\n",tmp_buf);
//         my_send( s, tmp_buf, sizeof(tmp_buf), flag );
//         bufsize += 15;
//         printf("bufsize: %d\n",bufsize);
//     }
//     printf("buf: %s\n",buf);
// }

// recv length and then string in chunks
void string_recv( int s, char* buf, int flag ) {
    int tmp_len, bufsize = 0;
    short int len;
    char tmp_buf[16];

    // receive string length from server
    my_recv( s, &len, sizeof(len), flag );
    printf("len: %d\n",len);

    // receive string from server
    bzero( buf, sizeof(buf) );
    while ( bufsize < len ) {
        bzero( tmp_buf, sizeof(tmp_buf) );
        if ( ( tmp_len = recv( s, tmp_buf, sizeof(tmp_buf), flag ) ) == -1 ) {
            perror("server receive error");
            exit(1);
        } else bufsize += tmp_len;
        strcat( buf, tmp_buf );
        printf("bufsize: %d\n", bufsize);
    }
    printf("buf: %s\n", buf);
}

short int md5_compute( int s, char *buf, unsigned char* digest, FILE *fp ) {
    int len;
    MD5_CTX mdContext;

    // // compute MD5 hash
    // printf("getting MD5 hash...\n");
    // MD5( (unsigned char*)&buf, len, (unsigned char*)&digest );

    // // send length of MD5 hash
    // printf("sending length of MD5 hash...\n");
    // my_send( s, &md5_len, sizeof(short int), 0 );

    // // send MD5 hash to client
    // printf("sending MD5 hash...\n");
    // my_send( s, &digest, MD5_DIGEST_LENGTH, 0 );

    MD5_Init (&mdContext);
    do {
        bzero( buf, sizeof(buf) );
        len = fread ( buf, sizeof(char), sizeof(buf), fp );
        MD5_Update( &mdContext, buf, len );
    } while ( len != 0 );
    MD5_Final ( digest, &mdContext );
    return (short int)MD5_DIGEST_LENGTH;
}
