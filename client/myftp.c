// Jenna Wilson (jwilso27) & Madelyn Nelson (mnelso12)
// CSE 30264
// Programming Assignment 3
// 10/12/16

#include "../program3.h"

#define MAX_LINE 4096 

char * query( int, char * );

int main(int argc, char *argv[]) {   
    // declare parameters
    FILE *fp; 
    struct hostent *hp;    
    struct sockaddr_in sin;    
    char *host, *filename;
    char buf[MAX_LINE], tmp_buf[MAX_LINE], digest[MD5_DIGEST_LENGTH];
    int s, port, size, tmp_size, i, flag;
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

        // reset other parameters
        tmp_size = 0;
        flag = 1;

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
        if ( strncmp( buf, "REQ", 3 ) == 0 ) {
            // download file from server
            
            // get and send filename info to server
            filename = query( s, "download" );

            // receive file size from server
            my_recv( s, &size, sizeof(size), 0 );

            // prompt user and return to "prompt for operation"
            // if file does not exist
            if ( size == -1 ) {
                printf( "File does not exist on the server.\n" );
                continue;
            }

            // receive MD5 hash from server
            my_recv( s, tmp_buf, MD5_DIGEST_LENGTH, 0 );

            // open file in disk
            if ( ( fp = fopen( filename, "w" ) ) == NULL ){
                printf("file I/O error\n");
                exit(1);
            }

            // receive file from server
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
                printf("tmp size: %d\n",tmp_size+len);
            } while ( ( tmp_size += len ) < size );

            // close file
            fclose( fp );

            // open file in disk
            if ( ( fp = fopen( filename, "r" ) ) == NULL ){
                printf("file I/O error\n");
                exit(1);
            }

            // compute MD5 hash
            len = md5_compute( s, filename, digest, fp );

            // close file
            fclose( fp );

            // compare MD5 hashes
            for ( i = 0; i < MD5_DIGEST_LENGTH; i++ ) {
                if ( tmp_buf[i] != digest[i] ) {
                    printf("file transfer error\n");
                    flag = 0;
                    break;
                }
            }
           
            if ( flag ) printf("file transfer successful\n");

        } else if ( strncmp( buf, "UPL", 3 ) == 0 ) {
            // upload file to server
            
            // get and send filename info to server
            filename = query( s, "upload" );

            // get acknowledgement from server
            my_recv( s, &flag, sizeof(flag), 0 );
            if ( flag == 0 ) {
                printf( "Cannot write file on server\n" );
                continue;
            }
            
            // open file to read
            if ( ( fp = fopen( filename, "r" ) ) == NULL ) {
                size = -1;
                my_send( s, &size, sizeof(size), 0 );
                printf( "File does not exist on the disk.\n" );
                continue;
            }
            
            // find file size
            fseek( fp, 0L, SEEK_END ); // TODO error check here
            size = ftell(fp);
            fseek( fp, 0, SEEK_SET );

            // send file size to server
            my_send( s, &size, sizeof(size), 0 );

            // send file to server
            do {
                bzero( buf, sizeof(buf) );
                len = fread( buf, sizeof(char), MAX_LINE, fp );
                my_send( s, buf, len, 0 );
            } while ( !feof( fp ) );

            // reset file pointer
            fseek( fp, 0, SEEK_SET );

            // compute MD5 hash
            len = md5_compute( s, filename, digest, fp );

            // close file
            fclose( fp );

            // send MD5 hash
            my_send( s, digest, len, 0 );

            // receive file transfer result
            my_recv( s, &flag, sizeof(flag), 0 );

            if ( flag ) printf("file transfer successful\n");
            else printf("file transfer error\n");

        } else if ( strncmp( buf, "LIS", 3 ) == 0 ) {
            // list the directory at the server
            my_recv( s, buf, sizeof(buf), 0 );
			printf("\n\n%s\n\n", buf);
        } else if ( strncmp( buf, "MKD", 3 ) == 0 ) {
            // make a directory at the server
            char dirName[MAX_LINE];
            printf("What do you want this new directory to be named?\n");
            scanf("%s", dirName);
            short int dirNameLen;
            dirNameLen = sizeof(dirName);
            char nameLen[MAX_LINE];
			sprintf(nameLen, "%d", dirNameLen);
            
			// sending length of directory name
            my_send(s, nameLen, sizeof(nameLen), 0);

            // sending directory name
            my_send(s, dirName, sizeof(dirName), 0);
           
           	// receive response code (1, -1, or -2)
            my_recv( s, buf, sizeof(buf), 0 );

			if ( strncmp( buf, "-2", 3 ) == 0 ) {
				printf("The directory already exists on server.\n");
			}
			else if ( strncmp( buf, "-1", 3 ) == 0 ) {
				printf("Error in making directory.\n");
			}
			else {
				printf("The directory was successfully made.\n");
			}


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
