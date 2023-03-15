CC=g++
CPPFLAGS = -std=c++20 -O2 -Wall -Wextra -pedantic

TARGET = ipkcpc

$(TARGET): $(TARGET).cpp
	$(CC) $(CPPFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)

.phony: clean