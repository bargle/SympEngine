CC=g++
CFLAGS=-I../../API -I../Video -I../../Common/FileIO/src -Wall -g3 -pedantic
#CFLAGS=-I../../API -I../Video -I../../Common/FileIO/src -O2
SRC_DIR=./
LIB_DIR=.
OUT_FILE=LIB_systemcore.a
#DEFINES=-D_LINUX_

all:
	# Compiling...
	$(CC) $(CFLAGS) $(DEFINES) -c ../Video/unix/VideoInterface.cpp -o VideoInterface.o
	$(CC) $(CFLAGS) $(DEFINES) -c ../Video/unix/VideoInterface3D.cpp -o VideoInterface3D.o
	$(CC) $(CFLAGS) $(DEFINES) -c ../Input/unix/InputInterface.cpp -o InputInterface.o
	$(CC) $(CFLAGS) $(DEFINES) -c ../Audio/unix/AudioInterface.cpp -o AudioInterface.o
	$(CC) $(CFLAGS) $(DEFINES) -c ../Audio/unix/AudioInterface_fmod.cpp -o AudioInterface_fmod.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)system.cpp -o system.o

	# Creating the static lib...
	ar rcs $(LIB_DIR)/$(OUT_FILE) system.o VideoInterface.o VideoInterface3D.o InputInterface.o AudioInterface.o AudioInterface_fmod.o
clean:
	rm *.o
	rm $(LIB_DIR)/$(OUT_FILE)
