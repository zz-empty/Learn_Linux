CC:=gcc

# 源文件和头文件目录
SRCDIR := src
INCDIR := include
OBJDIR := obj
BINDIR := bin

# 所有源文件，和目标文件
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))


# 最终的可执行文件
TARGET := $(BINDIR)/app

# 编译选项
CFLAGS := -I$(INCDIR)
.PHONY: all clean rebuild

all: $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(OBJ) | $(BINDIR)
	$(CC) $(OBJ) -o $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)
rebuild: clean all
