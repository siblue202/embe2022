CC=arm-none-linux-gnueabi-gcc

#CC= gcc

# project set up and directories
.SUFFIXES: .c .h .o

DEPS= main.h #input_process.h output_process.h
CFLAGS= #-W

TARGET = 120220184_hw1
OBJDIR = build/

_OBJS = input.o output.o main.o counter.o clock.o
OBJS = $(addprefix $(OBJDIR),$(_OBJS))

# SRCS = input_process.c output_process.c main.c
# _OBJS = $(patsubst .%.c,%.o,$(SRCS)) 
# OBJS = $(patsubst %,$(OBJDIR)%,$(_OBJS))

$(OBJDIR)$(TARGET): $(OBJS)
	$(CC) -static -o $(OBJDIR)$(TARGET) $(OBJS)

$(OBJDIR)%.o: %.c $(OBJDIR)
	$(CC) -static -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)

.PHONY: push
push:
	make
	adb push $(OBJDIR)$(TARGET) /data/local/tmp