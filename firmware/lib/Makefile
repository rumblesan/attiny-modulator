COMPILE = clang -Wall -Os

BUILD_DIR        = build
HEADER_DIRS      = include

INCLUDES         = $(addprefix -I, $(HEADER_DIRS))

SOURCES          = $(wildcard src/*.c) $(wildcard test/*.c)
OBJECTS          = $(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))

# symbolic targets:
all:	tests

$(BUILD_DIR)/%.o: %.c
	$(COMPILE) -c $< -o $@ $(INCLUDES)

.c.s:
	$(COMPILE) -S $< -o $@

test: tests
	./tests

clean:
	rm -f tests $(OBJECTS)

# file targets:
tests: $(OBJECTS)
	$(COMPILE) -lm -o tests $(OBJECTS)
