CXX := g++
CXXFLAGS := -std=c++20
DEBUG_CXXFLAGS := -g -O0

SRCS := $(wildcard *.cpp)
TARGET := ipkcpc

ifeq ($(OS),Windows_NT)
    CXX := x86_64-w64-mingw32-g++
    CXXFLAGS += -D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -D_UNICODE -DUNICODE
    LDFLAGS += -static-libgcc -static-libstdc++
    TARGET := $(TARGET).exe
endif

.PHONY: all
all: release

.PHONY: release
release: CXXFLAGS += $(RELEASE_CXXFLAGS)
release: $(TARGET)

.PHONY: debug
debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
debug: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET)