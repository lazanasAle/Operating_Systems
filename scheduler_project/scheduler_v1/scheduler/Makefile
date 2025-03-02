CC = g++
CFLAGS = -std=c++20 -lboost_system -g

TARGET = scheduler
SRC = scheduler.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
