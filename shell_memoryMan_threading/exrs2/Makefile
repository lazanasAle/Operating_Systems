CC = g++
CFLAGS = -std=c++20 -g

TARGET = launch
SRC = launch.cpp passenger.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
