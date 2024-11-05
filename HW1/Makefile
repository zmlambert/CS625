program_NAME := hw1

SRCS = HW1.c
SQ = HW1_square.c
OBJS = pbPlots.o supportLib.o

CFLAGS_PBPLOTS = -c -std=c99 -O3 -march=native
CFLAGS_SUPPORTLIB = -c -std=c99 -O0 -march=native
CFLAGS_DEBUG = -g -Wall
LDFLAGS = -lm

.PHONY: all clean

all: $(program_NAME)

$(program_NAME): $(OBJS) $(SRCS)
	gcc $(SRCS) $(OBJS) $(LDFLAGS) -o $(program_NAME)

pbPlots.o: pbPlots.c
	gcc $(CFLAGS_PBPLOTS) pbPlots.c

supportLib.o: supportLib.c
	gcc $(CFLAGS_SUPPORTLIB) supportLib.c

debug: CFLAGS_PBPLOTS += $(CFLAGS_DEBUG)
debug: CFLAGS_SUPPORTLIB += $(CFLAGS_DEBUG)

debug: $(program_NAME)

clean:
	rm -f $(OBJS) $(program_NAME) square

square: $(OBJS) $(SQ)
	gcc $(SQ) $(OBJS) $(LDFLAGS) -o square
