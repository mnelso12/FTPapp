// Jenna Wilson (jwilso27) & Madelyn Nelson (mnelso12)
// CSE 30264
// Programming Assignment 3
// 10/12/16

#include "../program3.h"

#define MAX_LINE 4096 

char * query( int, char * );

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
    char buf[MAX_LINE], tmp_buf[MAX_LINE], digest[MD5_DIGEST_LENGTH];
    int s, port, size, tmp_size = 0, i, flag = 1;
    short int len;

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
        // clear buf
        bzero( buf, sizeof(buf) );

        // get operation from user
        printf(
                "What operation would you like to execute?\n"
                "\tREQ: request (download) file\n"
                "\tUPL: upload file\n"
                "\tLIS: list directory\n"
                "\tMKD: make directory\n"
                "\tRMD: remove directory\n"
                "\tCHD: change directory\n"
                "\tDEL: delete file\n\tXIT: exit\n"
        );
        scanf( "%s", buf );

        // send command to server
        my_send( s, buf, sizeof(buf), 0 );

        // handle command
        if ( strncmp( buf, "REQ", 3 ) == 0 ) { // download file from server
//            // get filename from user
//            printf( "What file would you like to download?\n" );
//            scanf( "%s", buf );
//            filename = strdup( buf );
//            len = strlen( buf ) + 1;
//
//            // send file info to server
//            my_send( s, &len, sizeof(short int), 0 ); 
//            my_send( s, buf, len, 0 );
            
            filename = query( s, "download" );

            // receive file size from server
            //printf("waiting to receive file size...\n");
            my_recv( s, &size, sizeof(size), 0 );
            //printf("received file size: %d\n", size);

            // prompt user and return to "prompt for operation"
            // if file does not exist
            if ( size == -1 ) {
                printf( "File does not exist on the server.\n" );
                continue;
            }

            // receive MD5 hash from server
            //printf("waiting to receive MD5 hash...\n");
            my_recv( s, tmp_buf, MD5_DIGEST_LENGTH, 0 );

            // open file in disk
            if ( ( fp = fopen( filename, "w" ) ) == NULL ){
                printf("file I/O error\n");
                exit(1);
            }

            // receive file from server
            //printf("waiting to receive file from server...\n");
            do {
                bzero( buf, sizeof(buf) );
                len = recv( s, buf, sizeof(buf), 0 );
                if ( len == -1 ) {
                    perror("receive error");
                    exit(1);
                }
                //printf("len: %d\n", len);
                //printf("tmp_size: %d\n", tmp_size);
                fwrite( buf, sizeof(char), len, fp ); 
            } while ( ( tmp_size += len ) < size );

            // close file
            fclose( fp );

            // open file in disk
            if ( ( fp = fopen( filename, "r" ) ) == NULL ){
                printf("file I/O error\n");
                exit(1);
            }

            // compute MD5 hash
            // MD5( (unsigned char*)&buf, len, (unsigned char*)&digest );
            len = md5_compute( s, filename, digest, fp );

            for ( i = 0; i < MD5_DIGEST_LENGTH; i++ ) {
                if ( tmp_buf[i] != digest[i] ) {
                    printf("file transfer error\n");
                    flag = 0;
                    break;
                }
            }
           
            // close file
            fclose( fp );

            if ( flag ) printf("file transfer successful\n");

        } else if ( strncmp( buf, "UPL", 3 ) == 0 ) { // upload file to server
//            // get filename from user
//            printf( "What file would you like to upload?\n" );
//            scanf( "%s", buf );
//            filename = strdup( buf );
//            len = strlen( buf ) + 1;
//
//            // send file info to server
//            my_send( s, &len, sizeof(short int), 0 ); 
//            my_send( s, buf, len, 0 );

            filename = query( s, "upload" );

            // open file to read
            if ( ( fp = fopen( filename, "r" ) ) == NULL ){
                // send file size of -1 and
                // return to "prompt for operation"
                // if file does not exist
                size = -1;
                my_send( s, &size, sizeof(size), 0 );
                printf( "File does not exist on the disk.\n" );
                continue;
            }
            
            // find file size
            fseek( fp, 0L, SEEK_END ); // TODO error check here
            size = ftell(fp);
            printf("int file size: %d\n", size);

            // reset file pointer
            fseek( fp, 0, SEEK_SET );

            // send file size to server
            printf("sending file size...\n");
            my_send( s, &size, sizeof(size), 0 );
            printf("sent file size: %d\n", size);

            // compute MD5 hash
            len = md5_compute( s, filename, digest, fp );
            printf("len: %d\n", len);

            // send MD5 hash
            // my_send( s, &len, sizeof(short int), 0 );
            my_send( s, digest, len, 0 );

            // reset file pointer
            fseek( fp, 0, SEEK_SET );

            // get acknowledgement from server
            my_recv( s, &flag, sizeof(flag), 0 );
            if ( flag == 0 ) {
                printf( "Cannot write file on server\n" );
                continue;
            }
            
            // send file to server
            do {
                bzero( buf, sizeof(buf) );
                len = fread( buf, sizeof(char), MAX_LINE, fp );
                printf("len: %d\n",len);
                printf("%s\n",buf);
                my_send( s, buf, len, 0 );
            } while ( !feof( fp ) );

            // close file
            fclose( fp );

        } else if ( strncmp( buf, "LIS", 3 ) == 0 ) {
            // list the directory at the server
            my_recv( s, buf, sizeof(buf), 0 );
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

char * query( int s, char *op ) {
    short int len;
    char buf[256], *filename;

    // get filename from user
    printf( "What file would you like to %s?\n", op );
    scanf( "%s", buf );
    filename = strdup( buf );
    len = strlen( buf ) + 1;

    // send file info to server
    my_send( s, &len, sizeof(short int), 0 ); 
    my_send( s, buf, len, 0 );

    return filename;
}
