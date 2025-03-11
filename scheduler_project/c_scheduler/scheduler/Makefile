CC = gcc
CFLAGS = -lpthread

TARGET = scheduler
SRC = scheduler.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
