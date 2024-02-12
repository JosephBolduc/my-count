CC = g++
EXECUTABLE = my-count
CFLAGS = -Iinclude -std=c++1y -pthread

all: $(EXECUTABLE)

$(EXECUTABLE): my-count.cpp fileParser.h MemoryManager.h workerProcess.h
	$(CC) $(CFLAGS) $^ -o $@	

clean:
	rm -f $(EXECUTABLE)