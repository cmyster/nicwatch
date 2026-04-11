CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -O2
TARGET = nicwatch
OBJS = main.o snapshot.o journal.o analysis.o daemon.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
