TARGET:=Purr
SRCS:=Purr.c
OBJS:=$(SRCS:.c=.o)
LZED:=-lzed
LALSA:= -lasound -lpthread -lrt -ldl -lm

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LALSA) $(LZED)

%.o: %.c %.h
	$(CC) -c $<

%.o: %.c
	$(CC) -c $<

clean:
	rm -rf $(OBJS) $(TARGET)