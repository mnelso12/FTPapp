CC=gcc
FILE=myftp
CFLAGS=-Wall
LDFLAGS=-lssl

all: $(FILE)

$(FILE).o: $(FILE).c
	$(CC) -c $< -o $@

$(FILE): $(FILE).o
	$(CC) $(LDFLAGS) $< -o $@

clean:
	rm $(FILE) $(FILE).o
