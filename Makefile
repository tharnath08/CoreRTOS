SRC = $(shell find -iname "*.c")
OBJ = $(patsubst %.c, %.o, $(SRC))
INCLUDE = $(shell find -iname "*.h" -exec dirname {} \; | sed 's/^./-I./g' | xargs) 

all: main
	@

run: all
	@./main

main: $(OBJ)
	gcc $^ -o $@

%.o: %.c
	gcc $(INCLUDE) -c $^ -o $@

clean:
	@rm -f $(OBJ) main *.out
	@rm -rf build

.PHONY: clean all run