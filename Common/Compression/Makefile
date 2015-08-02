CPP=gcc
CC=cc
CFLAGS=-I../../API -Isrc -Izlib -O2
SRC_DIR=./
ZLIB_SRC_DIR=./zlib/
LIB_DIR=../lib
OUT_FILE=LIB_compressor.a
#DEFINES=-D_LINUX_

all:
	# Compiling...
	$(CPP) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)Compressor.cpp -o Compressor.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)adler32.c -o adler32.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)compress.c -o compress.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)crc32.c -o crc32.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)deflate.c -o deflate.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)gzio.c -o gzio.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)infback.c -o infback.o	
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)inffast.c -o inffast.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)inflate.c -o inflate.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)inftrees.c -o inftrees.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)trees.c -o trees.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)uncompr.c -o uncompr.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(ZLIB_SRC_DIR)zutil.c -o zutil.o
	
	# Creating the static lib...
	ar rcs $(LIB_DIR)/$(OUT_FILE) Compressor.o adler32.o compress.o crc32.o deflate.o gzio.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o

clean:
	rm *.o
	rm $(LIB_DIR)/$(OUT_FILE)
