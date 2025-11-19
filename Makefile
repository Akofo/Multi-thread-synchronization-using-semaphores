CXX = g++
CXXFLAGS = -pthread -Wall
TARGET = cse4001_sync
SOURCES = cse4001_sync.cpp
HEADERS = semaphore_class.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: all clean
