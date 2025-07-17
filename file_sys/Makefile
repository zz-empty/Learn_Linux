CC:=gcc
SRC:=$(wildcard *.c)
OUT:=$(patsubst %.c,%,$(SRC))

BIN_DIR:=./bin
BIN_TARGETS:=$(addprefix $(BIN_DIR)/, $(OUT))

all: $(BIN_TARGETS)

$(BIN_DIR)/%:%.c | $(BIN_DIR)
	$(CC) $< -o $@ 

$(BIN_DIR):
	mkdir -p $@

.PHONY:clean rebuild
clean:
	$(RM) $(BIN_TARGETS)
rebuild: clean all
