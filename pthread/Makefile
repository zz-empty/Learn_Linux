UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    CC := clang
else
    CC := gcc
endif

# 源文件和目标文件配置
SRC:= $(wildcard *.c)
OUT:= $(patsubst %.c,%,$(SRC))

# 可执行文件输出目录
BIN_DIR:= ./bin
BIN_TARGETS:= $(addprefix $(BIN_DIR)/,$(OUT))

# 默认目标
all: $(BIN_TARGETS)

# 规则：在 bin 目录下生成可执行文件，并链接 pthread
$(BIN_DIR)/%: %.c | $(BIN_DIR)
	$(CC) $< -o $@ -lpthread

# 创建 bin 目录
$(BIN_DIR):
	mkdir -p $@

# 清理
.PHONY: clean rebuild
clean:
	$(RM) $(BIN_TARGETS)

# 重建
rebuild: clean all

