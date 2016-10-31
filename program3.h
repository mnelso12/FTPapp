// Jenna Wilson (jwilso27) & Madelyn Nelson (mnelso12)
// CSE 30264
// Programming Assignment 3
// 10/12/16

// include librraries
#include <stdio.h>   
#include <stdlib.h>  
#include <unistd.h>
#include <string.h>  
#include <fcntl.h>
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
short int md5_compute( int, char*, unsigned char*, FILE* );
int md5_cmp( unsigned char*, unsigned char* );
int throughput( struct timeval*, struct timeval* );
char * query( int, char*, char* );
void md5_print( unsigned char* );
void success_print( int, int );
void client_del( int, char* );

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
        bzero( tmp_buf, tmp_len );
        if ( ( tmp_len = recv( s, tmp_buf, sizeof(tmp_buf), flag ) ) == -1 ) {
            perror("server receive error");
            exit(1);
        } else bufsize += tmp_len;
        strcat( buf, tmp_buf );
    }
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
int md5_cmp( unsigned char *buf, unsigned char *digest ) {
    int i;

    for ( i = 0; i < MD5_DIGEST_LENGTH; i++ )
        if ( buf[i] != digest[i] ) return 0;

    return 1;
}

// calculate throughput of file transfer
int throughput( struct timeval *start, struct timeval *fin ) {
    int usec = fin->tv_usec - start->tv_usec;
    bzero( start, sizeof( struct timeval ) );
    bzero( fin, sizeof( struct timeval ) );
    return usec;
}

// get file/directory info from user
char * query( int s, char *type, char *op ) {
    short int len;
    char buf[256], *name;

    // get name from user
    printf( "What %s would you like to %s?\n", type, op );
    scanf( "%s", buf );
    name = strdup( buf );
    len = strlen( buf ) + 1;

    // send file info to server
    my_send( s, &len, sizeof(short int), 0 ); 
    my_send( s, buf, len, 0 );

    return name;
}

// print md5sum
void md5_print( unsigned char *digest ) {
    int i;

    printf( "File MD5sum: " );

    for ( i = 0; i < MD5_DIGEST_LENGTH; i++ )
        printf( "%02hhx", digest[i] );

    printf( "\n" );
}

// print throughput
void success_print( int size, int tp ) {
    printf( "%d bytes transferred in %.6lf seconds: ", size, tp / 1000000.0 );
    printf( "%.3lf Megabytes/sec\n", ( double ) ( size / tp ) );
}

// remove file/directory
void client_del( int s, char *type ) {
    int flag = 1;
    char *name, buf[MAX_LINE];

    // get and send file info
    name = query( s, type, "delete" );
           
    // receive response code (1 or -1)
    my_recv( s, &flag, sizeof(flag), 0 );

    if ( flag == -1 ) printf( "The %s does not exist on server.\n", type );
    else {
        // get confirmation from user
        printf( "Are you sure you want to remove %s? (Yes/No)\n", name );
        scanf( "%s", buf );
        flag = strncmp( buf, "Yes", 3 );

        // send confirmation to server
        my_send(s, &flag, sizeof(flag), 0);

        if ( flag == 0 ) {
            // wait for server success/error response
            my_recv( s, &flag, sizeof(flag), 0 );
            if ( flag == 1 ) printf("%s deleted.\n", type);
            else printf("Failed to delete %s.\n", type);
        } else printf("Delete abandoned by the user!\n");
    }
}
