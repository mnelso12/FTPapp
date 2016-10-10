// Jenna Wilson (jwilso27) & Madelyn Nelson (mnelso12)
// CSE 30264
// Programming Assignment 3
// 10/12/16

#include "../program3.h"

#define MAX_LINE 4096 

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
        if ( send( s, buf, sizeof(buf), 0 ) == -1 ) {    
            perror("client send error");
            exit(1);
        }

        // handle command
        if ( strncmp( buf, "REQ", 3 ) == 0 ) {
            // download file from server
            query( s, buf );
            filename = strdup( buf );
			printf("filename: %s\n", filename);

            // receive file size from server
            printf("waiting to receive file size...\n");
            my_recv( s, buf, 0 );
            printf("received file size: %d\n", buf);

            // prompt user and return to "prompt for operation"
            // if file does not exist
            if ( ( len = atoi( buf ) ) == -1 ) {
                printf( "File does not exist on the server.\n" );
                continue;
            }

            // receive MD5 hash from server
            printf("waiting to receive MD5 hash...\n");
            my_recv( s, (char *)&digest, 0 );

            // open file in disk
            if ( ( fp = fopen( filename, "a" ) ) == NULL ){
                printf("file I/O error\n");
                exit(1);
            }

            // receive file from server
            printf("waiting to receive file from server... (need to write this!!)\n");

            // close file
            fclose( fp );

            // compute MD5 hash
            MD5( (unsigned char*)&buf, len, (unsigned char*)&digest );

        } else if ( strncmp( buf, "UPL", 3 ) == 0 ) {
            // upload file to server
        } else if ( strncmp( buf, "LIS", 3 ) == 0 ) {
            // list the directory at the server
            my_recv( s, buf, 0 );
			printf("\n\n%s\n\n", buf);
        } else if ( strncmp( buf, "MKD", 3 ) == 0 ) {
            // make a directory at the server
        } else if ( strncmp( buf, "RMD", 3 ) == 0 ) {
            // remove a directory at the server
        } else if ( strncmp( buf, "CHD", 3 ) == 0 ) {
            // change to a different directory on the server
        } else if ( strncmp( buf, "DEL", 3 ) == 0 ) {
            // delete file from server
        } else if ( strncmp( buf, "XIT", 3 ) == 0 ) {
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

// int query ( int s, char *buf ) {
void query ( int s, char *buf ) {
	printf("in query\n");
    short int len;

    // receive query from server
    my_recv( s, buf, 0 );

    // print query to user
    printf( "%s\n", buf );

    // get response from user
    scanf( "%s\n", buf );
    
    len = strlen(buf);

    // send response to server
    my_send( s, buf, 0 );
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
