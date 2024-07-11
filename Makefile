# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Define the target executable
TARGET = signature_scan

# Define the source files
SRCS = src/*

# Define the object files
OBJS = $(SRCS:.c=.o)

# Default target to build the program
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
