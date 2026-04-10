CC 		:= clang

INCLUDE_DIR	:= include
SRC_DIR		:= src
BUILD_DIR	:= build

CCFLAGS 	:= -Wall -Wextra --target=riscv32-unknown-none-elf -I$(INCLUDE_DIR)
LDFLAGS		:= -fuse-ld=lld -nostdlib -T$(SRC_DIR)/linker.ld \
		   -Wl,-Map=$(BUILD_DIR)/os.map

C_SRCS		:= $(SRC_DIR)/kernel/kernel.c
ASM_SRCS	:= $(SRC_DIR)/bootloader/boot.S

OBJS		:= $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS)) \
		   $(patsubst $(SRC_DIR)/%.S, $(BUILD_DIR)/%.o, $(ASM_SRCS))
TARGET		:= $(BUILD_DIR)/vanadium_os.elf



.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

run: all
	qemu-system-riscv32 -machine virt -bios none -kernel $(TARGET) -nographic
