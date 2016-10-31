# TCP File Transfer Protocol
## CSE 30264

Jenna Wilson (jwilso27)
&
Madelyn Nelson (mnelso12)

### Files:
- program3.h: file containing all included libraries and functions used
- client subdirectory: includes myftp.c and Makefile
- server subdirectory: includes myftpd.c, Makefile, and symbolic link to test
  files (requires access to /afs/nd.edu)

### Usage:

Compile server and client
```
> cd client
> make
> cd ../server
> make
```

Run server
 ```
> myftpd [port number]
```

Run client
```
> cd ../client
> myftp [hostname] [port number]
```

Enter commands from client

- REQ: download a file from the server
```
What operation would you like to execute?
...
> REQ
What file would you like to download?
> SmallFile.txt
```
- UPL: upload a file to the server
```
What operation would you like to execute?
...
> UPL
What file would you like to upload?
> upload.txt
```
- LIS: list the current directory on the server
```
What operation would you like to execute?
...
> LIS
```
- MKD: make a directory on the server
```
What operation would you like to execute?
...
> MKD
What directory would you like to create?
> dir
```
- RMD: remove a directory on the server
```
What operation would you like to execute?
...
> RMD
What directory would you like to remove?
> dir
Are you sure you want to remove dir? (Yes/No)
> Yes
```
- CHD: change current directory on the server
```
What operation would you like to execute?
...
> CHD
What directory would you like to move to?
> files
```
- DEL: delete a file on the server
```
What operation would you like to execute?
...
> DEL
What file would you like to delete?
> delete.txt
Are you sure you want to remove delete.txt? (Yes/No)
> Yes
```
- XIT: exit the connection to the server
```
What operation would you like to execute?
...
> XIT
```

Once the client has closed its connection, the server remains open for another
client to connect.
