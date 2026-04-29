CC = clang
<<<<<<< HEAD
CFLAGS = -std=c23 -Wall -Wextra -O2
=======
CFLAGS = -std=c23 -Wall -Wextra -O3
>>>>>>> 22156d4 (Changed gcc to clang, removed unused includes.)
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
