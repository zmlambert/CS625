program_NAME := hw3

STRING_SRCS = string/HW3_string.c string/genetic_string.c
EQUATION_SRCS = equation/HW3_equation.c equation/genetic_equation.c

CFLAGS_DEBUG = -g -Wall
LDFLAGS = -lm

.PHONY: all clean string equation debug

all: string equation

string: $(STRING_SRCS)
	gcc $(STRING_SRCS) $(LDFLAGS) -o $(program_NAME)_string
	@echo "Built $(program_NAME)_string"

equation: $(EQUATION_SRCS)
	gcc $(EQUATION_SRCS) $(LDFLAGS) -o $(program_NAME)_equation
	@echo "Built $(program_NAME)_equation"

debug: $(EQUATION_SRCS) $(STRING_SRCS)
	gcc $(EQUATION_SRCS) $(LDFLAGS) $(CFLAGS_DEBUG) -o $(program_NAME)_equation
	gcc $(STRING_SRCS) $(LDFLAGS) $(CFLAGS_DEBUG) -o $(program_NAME)_string
	@echo "Built $(program_NAME)_equation and $(program_NAME)_string for debug"

clean:
	rm -f $(program_NAME)_string
	rm -f $(program_NAME)_equation
