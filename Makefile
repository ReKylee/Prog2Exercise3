BIN = hrm.exe

BUILD_DIR = ./build
SOURCE_DIR = ./Source

CC = gcc

CC_FLAGS = -Wall -Werror
LNK_FLAGS = -L. -lprog2

#All .c files
SOURCE =  $(wildcard $(SOURCE_DIR)/*.c)
# All .o files go to build dir.
OBJS := $(SOURCE:%=$(BUILD_DIR)/%.o)

# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)

#Executable target
$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LNK_FLAGS)

# Include all .d files
-include $(DEP)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) -MMD $(CFLAGS) -c $< -o $@


clean:
	-rm -r $(BUILD_DIR)
	-rm $(BIN)
