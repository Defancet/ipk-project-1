.PHONY = all clean
CXX = g++
TARGET=ipkcpc

all: $(TARGET)

hinfosvc: hinfosvc.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf *.o $(TARGET)