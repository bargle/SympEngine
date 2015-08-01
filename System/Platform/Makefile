CC=g++
CFLAGS=-I../../API -Wall -g3
SRC_DIR=./
LIB_DIR=.
OUT_FILE=LIB_platform.a
#DEFINES=-D_LINUX_

all:
	# Compiling...
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)unix/platform.cpp -o platform.o

	# Creating the static lib...
	ar rcs $(LIB_DIR)/$(OUT_FILE) platform.o
clean:
	rm *.o
	rm $(LIB_DIR)/$(OUT_FILE)
