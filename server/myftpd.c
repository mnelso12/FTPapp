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

#define MAX_PENDING 5   
#define MAX_LINE 4096 

void my_send( int, void*, size_t, int );
void my_recv( int, void*, size_t, int );
void query( int, char* );
int req_size( char* );
void req_md5( int, char* );
void req_send( int, char*, int );

int upl( int, char* );
int del( int, char* );
int lis( );
int mkd( int, char* );
int rmd( int, char* );
int chd( int, char* );
int xit( );

int main(int argc, char *argv[]) {
    // declare parameters
    struct sockaddr_in sin;    
    char buf[MAX_LINE];  
    int s, new_s, len, port, opt = 1, size;

    // check arguments
    if ( argc == 2 ) port = atoi(argv[1]);
    else {
        fprintf(stderr, "usage: myftpd <port>\n");   
        exit(1);
    }

    // build address data structure
    bzero((char *)&sin, sizeof(sin));   
    sin.sin_family = AF_INET;    
    sin.sin_addr.s_addr = INADDR_ANY; 
    sin.sin_port = htons(port); 

    // setup passive open 
    if ( ( s = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 ) {   
        perror("socket error");
        exit(1);    
    }   

    // set socket option   
    if ( ( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int) ) ) < 0 ) {   
        perror ("socket option error");  
        exit(1);
    }   

    // bind socket
    if ( ( bind( s, (struct sockaddr *)&sin, sizeof(sin) ) ) < 0 ) {   
        perror("bind error");
        exit(1);
    }

    // listen for client
    if ( ( listen( s, MAX_PENDING ) ) < 0 ) {   
        perror("listen error");
        exit(1);    
    }    

    // wait for connection, then receive and print text
    while (1) {   
        if ( ( new_s = accept( s, (struct sockaddr *)&sin, &len ) ) < 0 ) {   
            perror("accept error");   
            exit(1);
        }
        while (1) {
            // receive command from client
            my_recv( new_s, buf, sizeof(buf), 0 );
            
            // handle command
            if ( strncmp( buf, "REQ", 3 ) ) { // download file from server
                sprintf( buf, "What file would you like to download?\n" );
                query( new_s, buf );
                len = strlen( buf );

                // send file size to client
                my_send( new_s, buf, len, 0 );

                // return to "wait for operation from client"
                // if file does not exist
                if ( ( size = req_size( buf ) ) == -1 ) continue;

                req_md5( s, buf );
                req_send( s, buf, size );

            } else if ( strncmp( buf, "UPL", 3 ) ) {
                // upload file to server
                sprintf( buf, "What file would you like to upload?\n" );
            } else if ( strncmp( buf, "LIS", 3 ) ) {
                // list the directory at the server
            } else if ( strncmp( buf, "MKD", 3 ) ) {
                // make a directory at the server
                sprintf( buf, "What directory path would you like to make?\n" );
            } else if ( strncmp( buf, "RMD", 3 ) ) {
                // remove a directory at the server
                sprintf( buf, "What directory path would you like to remove?\n(Note that the directory must be empty for it to be removed.)\n" );
            } else if ( strncmp( buf, "CHD", 3 ) ) {
                // change to a different directory on the server
                sprintf( buf, "Which directory would you like to change to?\n" );
            } else if ( strncmp( buf, "DEL", 3 ) ) {
                // delete file from server
                sprintf( buf, "What file would you like to delete?\n" );
            } else if ( strncmp( buf, "XIT", 3 ) ) close( new_s ); //exit
        }
    }  
}

void my_send( int s, void* buf, size_t len, int flag ) {
    if ( send( s, buf, len, flag ) == -1 ) {    
        perror("server send error");
        exit(1);
    }
}

void my_recv( int s, void* buf, size_t len, int flag ) {
    if ( recv( s, buf, len, flag ) == -1 ) {
        perror("server receive error");
        exit(1);
    }
}

// int query( int s, char* buf ) {
void query( int s, char* buf ) {
    int len = strlen( buf );
    
    // send query to client
    my_send( s, buf, len, 0 );

    // receive response length from client
    my_recv( s, buf, sizeof(buf), 0 );
    len = atoi(buf);

    // receive response from client
    my_recv( s, buf, len, 0 );
}

int req_size( char *buf ) {
    struct stat *s;
    if ( stat( buf, s ) == -1 ) return -1;
    else return s->st_size;
}

// int req_md5( int s, char *buf ) {
void req_md5( int s, char *buf ) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    int len = strlen(buf);

    // compute MD5 hash
    MD5( (unsigned char*)&buf, len, (unsigned char*)&digest );

    // send MD5 hash to client
    my_send( s, &digest, len, 0 );
}

// int req_send( int s, char *buf, int size ) {
void req_send( int s, char *buf, int size ) {
    int i = 0;
    
    // send file to client in chunks
    do {
        my_send( s, &buf[i*MAX_LINE], MAX_LINE, 0 );
    } while ( size > MAX_LINE*i++ );
    // return 0;
}
