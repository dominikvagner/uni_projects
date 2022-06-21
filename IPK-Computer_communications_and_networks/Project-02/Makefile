CC = g++
CFLAGS  = -g -Wall

TARGET = ipk-sniffer

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp -lpcap

clean:
	$(RM) $(TARGET)