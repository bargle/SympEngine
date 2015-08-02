CC=cc
CFLAGS= -O2
SRC_DIR=./
LIB_DIR=../../lib
OUT_FILE=LIB_zlib.a
#DEFINES=-D_LINUX_

all:
	# Compiling...
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)adler32.c -o adler32.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)compress.c -o compress.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)crc32.c -o crc32.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)deflate.c -o deflate.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)gzio.c -o gzio.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)infback.c -o infback.o	
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)inffast.c -o inffast.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)inflate.c -o inflate.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)inftrees.c -o inftrees.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)trees.c -o trees.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)uncompr.c -o uncompr.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)zutil.c -o zutil.o

	
	# Creating the static lib...
	ar rcs $(LIB_DIR)/$(OUT_FILE) adler32.o compress.o crc32.o deflate.o gzio.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
clean:
	rm *.o
	rm $(LIB_DIR)/$(OUT_FILE)
