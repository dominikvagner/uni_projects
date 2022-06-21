# Simple GNU Makefile for C projects.
 #     ___ _   __
 #    / _ \ | / /
 #   / // / |/ /
 #  /____/|___/

 # @author Dominik Vagner, xvagne10

CC = gcc
CFLAGS = -Wall -lm
 
TARGET = hinfosvc
 
all: $(TARGET)
 
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
