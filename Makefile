TARGET = cells
LIBS = -lm
CC = gcc

ifeq ($(CC), nvc)
	CFLAGS = -acc=host -Minfo=accel -o $@
else
	CFLAGS = -g -Wall -o $@
endif

.PHONY: all clean

all: $(TARGET) 

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) 

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -f img/*
	-touch img/dummy.txt
