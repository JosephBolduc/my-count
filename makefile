CC = g++
EXECUTABLE = my-count
CFLAGS = -Wall -Iinclude -std=c++1y

all: $(EXECUTABLE)

$(EXECUTABLE): main.cpp fileManager.h SharedMemoryManager.h
	$(CC) $(CFLAGS) $^ -o $@	

clean:
	rm -f $(EXECUTABLE)