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
#include <arpa/inet.h>
#include <netinet/in.h>   
#include <netdb.h>
#include <openssl/md5.h>

#define MAX_LINE 4096 

void my_send( int, void*, size_t, int );
void my_recv( int, void*, size_t, int );
void query( int, char * );

int req( int, char* );
int upl( int, char* );
int del( int, char* );
int lis( );
int mkd( int, char* );
int rmd( int, char* );
int chd( int, char* );
int xit( );

int main(int argc, char *argv[]) {   
    // declare parameters
    FILE *fp; 
    struct hostent *hp;    
    struct sockaddr_in sin;    
    char *host, *filename;
    char buf[MAX_LINE], tmp_buf[MAX_LINE]; 
    unsigned char digest[MD5_DIGEST_LENGTH];
    int s, len, port, size = 0;

    // check arguments
    if ( argc == 3 ) { 
        host = argv[1];    
        port = atoi(argv[2]);    
    } else {   
        fprintf(stderr, "usage: myftp <host> <port>\n");   
        exit(1);
    }   

    // translate host name into peer's IP address 
    hp = gethostbyname(host); 
    if (!hp) {   
        fprintf(stderr, "myftp: unknown host: %s\n", host);  
        exit(1);
    }   

    // build address data structure
    bzero((char *)&sin, sizeof(sin));   
    sin.sin_family = AF_INET;    
    bcopy(hp->h_addr, (char *)&sin. sin_addr, hp->h_length);  
    sin.sin_port = htons(port);

    // active open
    if ( ( s = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 ) {   
        perror("socket error");
        exit(1);
    }   

    // connect to server
    if ( connect( s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ) {
        perror("connection error");
        close(s);
        exit(1);
    }   

    // main loop: get command from stdin
    while (1) {
        printf( "What operation would you like to execute?\n\tREQ: request (download) file\n\tUPL: upload file\n\tLIS: list directory\n\tMKD: make directory\n\tRMD: remove directory\n\tCHD: change directory\n\tDEL: delete file\n\tXIT: exit\n" );
        scanf( "%s", buf );

        len = strlen(buf) + 1;  

        // send command to server
        my_send( s, buf, len, 0 );

        // handle command
        if ( strncmp( buf, "REQ", 3 ) ) {
            // download file from server
            query( s, buf );
            filename = strdup( buf );

            // receive file size from server
            my_recv( s, buf, sizeof(buf), 0 );

            // prompt user and return to "prompt for operation"
            // if file does not exist
            if ( ( len = atoi( buf ) ) == -1 ) {
                printf( "File does not exist on the server.\n" );
                continue;
            }

            // receive MD5 hash from server
            my_recv( s, &digest, MD5_DIGEST_LENGTH, 0 );

            // open file in disk
            if ( ( fp = fopen( filename, "a" ) ) == NULL ){
                printf("file I/O error\n");
                exit(1);
            }

            // receive file from server
            do {
                my_recv( s, buf, MAX_LINE, 0 );
                size += fwrite( buf, sizeof(buf[0]), MAX_LINE, fp );
            } while ( size < len );

            // close file
            fclose( fp );

            // compute MD5 hash
            MD5( (unsigned char*)&buf, len, (unsigned char*)&digest );

        } else if ( strncmp( buf, "UPL", 3 ) ) {
            // upload file to server
        } else if ( strncmp( buf, "LIS", 3 ) ) {
            // list the directory at the server
        } else if ( strncmp( buf, "MKD", 3 ) ) {
            // make a directory at the server
        } else if ( strncmp( buf, "RMD", 3 ) ) {
            // remove a directory at the server
        } else if ( strncmp( buf, "CHD", 3 ) ) {
            // change to a different directory on the server
        } else if ( strncmp( buf, "DEL", 3 ) ) {
            // delete file from server
        } else if ( strncmp( buf, "XIT", 3 ) ) {
            // exit
            break;
        } else {
            // default case
            printf( "Invalid command.\n" );
            continue;
        }
    }  
    close(s);
    printf( "The session has been closed.\n" );
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

// int query ( int s, char *buf ) {
void query ( int s, char *buf ) {
    int len;

    // receive query from server
    my_recv( s, buf, sizeof(buf), 0 );

    // print query to user
    printf( "%s\n", buf );

    // get response from user
    scanf( "%s\n", buf );
    
    len = strlen(buf);

    // send response length to server
    my_send( s, &len, sizeof(len), 0 );

    // send response to server
    my_send( s, buf, sizeof(buf), 0 );
}

int req( int len, char* filename ) {
    // download file from server
}

int upl( int len, char* filename ) {
    // upload file to server
}

int del( int len, char* filename ) {
    // delete file from server
}

int lis( ) {
    // list the directory at the server
}

int mkd( int len, char* dir_name ) {
    // make a directory at the server
}

int rmd( int len, char* dir_name ) {
    // remove a directory at the server
}

int chd( int len, char* dir_name ) {
    // change to a different directory on the server
}

int xit( ) {
    // exit
}
