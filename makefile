CC = g++
EXECUTABLE = my-count
CFLAGS = -std=c++1y -o3

all: $(EXECUTABLE)

$(EXECUTABLE): my-count.cpp
	$(CC) $(CFLAGS) $^ -o $@	

clean:
	rm -f $(EXECUTABLE)