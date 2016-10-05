// Jenna Wilson (jwilso27) & Madelyn Nelson (mnelso12)
// CSE 30264
// Programming Assignment 3
// 10/12/16

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

void my_send( int s, char* buf, int flag ) {
    short int len = strlen(buf);
    int bufsize = 0;
    char tmp_buf[16];

    // send command length to server
    if ( send( s, &len, sizeof(len), flag ) == -1 ) {    
        perror("server send error");
        exit(1);
    }

    // send command to server
    while ( bufsize < len ) {
        bzero( tmp_buf, sizeof(tmp_buf) );
        strncpy( tmp_buf, &buf[bufsize], 15 );
        printf("%s\n",tmp_buf);
        if ( send( s, tmp_buf, sizeof(tmp_buf), flag ) == -1 ) {    
            perror("server send error");
            exit(1);
        }
        bufsize += 15;
        printf("bufsize: %d\n",bufsize);
    }
    printf("buf: %s\n",buf);
}

void my_recv( int s, char* buf, int flag ) {
    int tmp_len, bufsize = 0;
    short int len;
    char tmp_buf[16];

    // receive string length from server
    if ( recv( s, &len, sizeof(len), flag ) == -1 ) {
        perror("server receive error");
        exit(1);
    }
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
    printf("buf: %s", buf);
}

