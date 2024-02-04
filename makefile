CC = g++
EXECUTABLE = my-count
CFLAGS = -Wall -Iinclude

all: $(EXECUTABLE)

$(EXECUTABLE): main.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(EXECUTABLE)