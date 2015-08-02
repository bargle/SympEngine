CC=g++
CFLAGS=-I../../API -I../../Common/FileIO/src -I../../Common/Compression -O2
SRC_DIR=./
LIB_DIR=../../Common/lib
OUT_DIR=../../bin
OUT_FILE=$(OUT_DIR)/ResourcePacker.exe
#DEFINES=-D_LINUX_

all:
	# Compiling...
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)ResourcePacker.cpp -o ResourcePacker.o
	
	# Creating the executable...
	$(CC) $(CFLAGS) -L$(LIB_DIR) -o $(OUT_FILE) ResourcePacker.o $(LIB_DIR)/LIB_fileio.a $(LIB_DIR)/LIB_compressor.a
	
clean:
	rm *.o
