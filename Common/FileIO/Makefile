CC=g++
CFLAGS=-I../../API -Isrc -O2
SRC_DIR=src/
LIB_DIR=../lib
OUT_FILE=LIB_fileio.a
#DEFINES=-D_LINUX_

all:
	# Compiling...
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)binaryfile.cpp -o binaryfile.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)fileinterface.cpp -o fileinterface.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)resourcefile.cpp -o resourcefile.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)packedfile.cpp -o packedfile.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)ramfile.cpp -o ramfile.o
	
	# Creating the static lib...
	ar rcs $(LIB_DIR)/$(OUT_FILE) binaryfile.o fileinterface.o resourcefile.o packedfile.o ramfile.o
clean:
	rm *.o
	rm $(LIB_DIR)/$(OUT_FILE)
