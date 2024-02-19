CC = g++
EXECUTABLE = my-count
CFLAGS = -Iinclude -std=c++1y -pthread

all: $(EXECUTABLE)

$(EXECUTABLE): my-count.cpp
	$(CC) $(CFLAGS) $^ -o $@	

clean:
	rm -f $(EXECUTABLE)