CC=g++
CFLAGS=-I../../API -I../../libs/FileIO/src -I../../System/Video -I../../System/Platform -Wall -g3 -pedantic
#CFLAGS=-I../../API -I../../libs/FileIO/src -I../../System/Video -I../../System/Platform -O2
SRC_DIR=./
LIB_DIR=../../libs
OUT_DIR=../../../bin
OUT_FILE=$(OUT_DIR)/ResourcePacker.exe
DEFINES=-D_LINUX_

all:
	# Compiling...
	#$(CC) $(CFLAGS) $(DEFINES) $(SRC_DIR)Game.cpp -o Game.o
	$(CC) $(CFLAGS) $(DEFINES) -Wall -g3 $(SRC_DIR)*.cpp ../../System/Core/LIB_systemcore.a ../../System/Platform/LIB_platform.a ../../3rdParty/fmod/api/lib/libfmodex64.so -pthread -lGL -lGLU -lXxf86vm
	
	# Creating the executable...
	#$(CC) $(CFLAGS) -L$(LIB_DIR) -o $(OUT_FILE) ResourcePacker.o $(LIB_DIR)/LIB_fileio.a ../../System/Core/LIB_systemcore.a
	
clean:
	rm *.o
