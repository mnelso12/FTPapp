// Jenna Wilson (jwilso27) & Madelyn Nelson (mnelso12)
// CSE 30264
// Programming Assignment 3
// 10/12/16

#include "../program3.h"

#define MAX_PENDING 5   
#define MAX_LINE 4096 

void query( int, char* );
int req_size( char* );
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
    FILE *fp;
    struct sockaddr_in sin;    
    char buf[MAX_LINE], *filename, digest[MD5_DIGEST_LENGTH];
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
            //printf("%s\n",buf);

            // handle command
            if ( strncmp( buf, "REQ", 3 ) == 0) { // download file from server
                // receive filename in buf
                string_recv( new_s, buf, 0 );
                filename = strdup( buf );
                //printf("filename: %s\n", filename);

                // open file to read
                if ( ( fp = fopen( filename, "r" ) ) == NULL ){
                    // send file size of -1 and
                    // return to "wait for operation from client"
                    // if file does not exist
                    size = -1;
                    my_send( new_s, &size, sizeof(size), 0 );
                    continue;
                }
                
                // find file size
                fseek( fp, 0L, SEEK_END ); // TODO error check here
                size = ftell(fp);
                printf("int file size: %d\n", size);

                // reset file pointer
                fseek( fp, 0, SEEK_SET );

                // send file size to client
                //printf("sending file size...\n");
                my_send( new_s, &size, sizeof(size), 0 );
                //printf("sent file size: %d\n", size);

                // compute MD5 hash
                len = md5_compute( new_s, filename, digest, fp );
                //printf("len: %d\n", len);

                // send MD5 hash
                // my_send( s, &len, sizeof(short int), 0 );
                my_send( new_s, digest, len, 0 );

                // reset file pointer
                fseek( fp, 0, SEEK_SET );

                // send file to client
                do {
                    bzero( buf, sizeof(buf) );
                    len = fread( buf, sizeof(char), MAX_LINE, fp );
                    my_send( new_s, buf, len, 0 );
                } while ( !feof( fp ) );

                // close file
                fclose( fp );

            } else if ( strncmp( buf, "UPL", 3 ) == 0 ) {
                // upload file to server
                sprintf( buf, "What file would you like to upload?\n" );
            } else if ( strncmp( buf, "LIS", 3 ) == 0 ) {
                // list the directory at the server
            } else if ( strncmp( buf, "MKD", 3 ) == 0 ) {
                // make a directory at the server
                sprintf( buf, "What directory path would you like to make?\n" );
            } else if ( strncmp( buf, "RMD", 3 ) == 0 ) {
                // remove a directory at the server
                sprintf( buf, "What directory path would you like to remove?\n(Note that the directory must be empty for it to be removed.)\n" );
            } else if ( strncmp( buf, "CHD", 3 ) == 0 ) {
                // change to a different directory on the server
                sprintf( buf, "Which directory would you like to change to?\n" );
            } else if ( strncmp( buf, "DEL", 3 ) == 0 ) {
                // delete file from server
                sprintf( buf, "What file would you like to delete?\n" );
            } else if ( strncmp( buf, "XIT", 3 ) == 0 ) close( new_s ); //exit
        }
    }  
}
