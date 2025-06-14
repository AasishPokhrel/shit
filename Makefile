# --- Arshitecture Configuration ---
ARCH ?= x86_64
ARCH_TRIPLET ?= x86-64

ifeq ($(ARCH), x86_64)
	ARCH_TRIPLET = x86-64
else
	$(error Unsupported arshitecture: $(ARCH))
endif

# --- Toolchain Configuration ---
TARGET_TRIPLET ?= $(ARCH)-elf
CC = $(TARGET_TRIPLET)-gcc
AS = nasm
LD = $(TARGET_TRIPLET)-ld
OBJCOPY = $(TARGET_TRIPLET)-objcopy
QEMU = qemu-system-$(ARCH)

# --- Directory Structure ---

ROOT_DIR = .
BUILD_DIR = $(ROOT_DIR)/build
OBJ_DIR_BASE = $(BUILD_DIR)/obj
OBJ_DIR = $(OBJ_DIR_BASE)/$(ARCH)
DIST_DIR = $(BUILD_DIR)/dist
ISO_ROOT_DIR = $(DIST_DIR)/iso_root

# Source directories
ARCH_SRC_DIR = $(ROOT_DIR)/arch/$(ARCH)
KERNEL_SRC_DIR = $(ROOT_DIR)/shit
DRIVERS_SRC_DIR = $(ROOT_DIR)/drivers
LIB_SRC_DIR = $(ROOT_DIR)/lib

# Include directories
INCLUDE_MAIN_DIR = include
INCLUDE_ARCH_INTERNAL_DIR = $(ARCH_SRC_DIR)/include
INCLUDE_KERNEL_API_DIR = $(INCLUDE_MAIN_DIR)/kernel
INCLUDE_DRIVERS_API_DIR = $(INCLUDE_MAIN_DIR)/drivers
INCLUDE_LIB_API_DIR = $(INCLUDE_MAIN_DIR)/lib

LIMINE_FILES_DIR = $(ROOT_DIR)/limine_files

# Source file discovery

find_files = $(shell find $(1) -name "$(2)" -print)

ASM_FILES = $(call find_files,$(ARCH_SRC_DIR),*.s) \
			$(call find_files,$(ARCH_SRC_DIR),*.S)

C_FILES_ARCH = $(call find_files,$(ARCH_SRC_DIR),*.c)
C_FILES_KERNEL = $(call find_files,$(KERNEL_SRC_DIR),*.c)
C_FILES_DRIVERS = $(call find_files,$(DRIVERS_SRC_DIR),*.c)
C_FILES_LIB = $(call find_files,$(LIB_SRC_DIR),*.c)

C_SOURCES = $(C_FILES_ARCH) $(C_FILES_KERNEL) $(C_FILES_DRIVERS) $(C_FILES_LIB)

# --- Object Files ---

src_to_obj = $(addprefix $(OBJ_DIR)/, $(patsubst ./%,%,$(1)))
obj_to_src = $(patsubst $(OBJ_DIR)/%,./%,$(1))

ASM_OBJECTS = $(patsubst %.s,%.o,$(call src_to_obj, $(filter %.s,$(ASM_FILES)))) \
              $(patsubst %.S,%.o,$(call src_to_obj, $(filter %.S,$(ASM_FILES))))
C_OBJECTS   = $(patsubst %.c,%.o,$(call src_to_obj, $(C_SOURCES)))

FONT_PSF_SRC = $(ROOT_DIR)/resources/font.psf
FONT_PSF_OBJ = $(OBJ_DIR)/resources/font.o

OBJECTS     = $(sort $(ASM_OBJECTS) $(C_OBJECTS) $(FONT_PSF_OBJ))
DEPS        = $(C_OBJECTS:.o=.d)

# --- Output Files ---

KERNEL_ELF = $(BUILD_DIR)/shitkernel-$(ARCH).elf
OS_ISO = $(DIST_DIR)/poopos-$(ARCH).iso

# --- Compiler and Linker Flags

INCLUDES_BASE = -I$(INCLUDE_MAIN_DIR) \
				-I$(INCLUDE_KERNEL_API_DIR) \
				-I$(INCLUDE_DRIVERS_API_DIR) \
				-I$(INCLUDE_LIB_API_DIR)

CFLAGS_COMMON = -std=c11 -Wall -Wextra -Werror -pedantic -O2 -g \
		 -ffreestanding -fno-stack-protector -fno-pie \
		 -mno-red-zone -mcmodel=kernel -mgeneral-regs-only \
		 -MMD -MP

ASFLAGS_COMMON = -g
LDFLAGS_COMMON = -T $(ROOT_DIR)/linker-$(ARCH).ld -nostdlib -static -no-pie \
				 --no-dynamic-linker -z max-page-size=0x1000

# --- Architecture Specific Settings ---
INCLUDES_ARCH = -I$(INCLUDE_ARCH_INTERNAL_DIR)

ifeq ($(ARCH), x86_64)
	ASFLAGS_ARCH = -f elf64
	CFLAGS_ARCH = -DARCH_X86_64
else
    $(error Unsupported arshitecture: $(ARCH))
endif

# Final flags
CFLAGS = $(CFLAGS_COMMON) $(INCLUDES_BASE) $(INCLUDES_ARCH) $(CFLAGS_ARCH)
ASFLAGS = $(ASFLAGS_COMMON) $(ASFLAGS_ARCH)
LDFLAGS = $(LDFLAGS_COMMON) $(LDFLAGS_ARCH)

# --- Limine Files ---
LIMINE_BIOS_SYS_SRC ?= $(LIMINE_FILES_DIR)/limine-bios.sys
LIMINE_BIOS_CD_SRC ?= $(LIMINE_FILES_DIR)/limine-bios-cd.bin
ESSENTIAL_LIMINE_FILES = \
	$(LIMINE_BIOS_SYS_SRC) \
	$(LIMINE_BIOS_CD_SRC)

# --- Build Rules ---
.PHONY: all clean iso run qemu_debug directories fetch-limine check-limine-files

all: $(KERNEL_ELF)
	echo $(OBJECTS)

directories:
	@echo "Creating directories for $(ARCH)..."
	@mkdir -p $(OBJ_DIR) $(BUILD_DIR) $(DIST_DIR) $(ISO_ROOT_DIR)
	@$(foreach dir, $(sort $(dir $(OBJECTS))), mkdir -p $(dir);)

$(FONT_PSF_OBJ): $(FONT_PSF_SRC) | directories
	@echo "OBJCOPY [$(ARCH)] $< -> $@"
	@mkdir -p $(dir $@)
	$(OBJCOPY) -I binary -O elf64-$(ARCH_TRIPLET) $< $@

$(KERNEL_ELF): $(OBJECTS) $(ROOT_DIR)/linker-$(ARCH).ld | directories
	echo "LD   [$(ARCH)] $@"
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(OBJ_DIR)/%.o: ./%.c Makefile | directories
	@echo "CC   [$(ARCH)] $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: ./%.s Makefile | directories
	@echo "AS   [$(ARCH)] $<"
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: ./%.S Makefile | directories
	@echo "AS   [$(ARCH)] $<"
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@echo "CLEAN"
	rm -rf $(BUILD_DIR)/obj/$(ARCH) $(BUILD_DIR)/shit-$(ARCH).elf $(DIST_DIR)

iso: $(KERNEL_ELF) $(ROOT_DIR)/limine.conf check-limine-files | directories
	@echo "--> Preparing files for ISO image [$(ARCH)]..."
	cp $(KERNEL_ELF) $(ISO_ROOT_DIR)/shit.elf
	cp $(ROOT_DIR)/limine.conf $(ISO_ROOT_DIR)/limine.conf

	@echo "--> Copying Limine BIOS files..."
	cp $(LIMINE_BIOS_SYS_SRC) $(ISO_ROOT_DIR)
	cp $(LIMINE_BIOS_CD_SRC) $(ISO_ROOT_DIR)

	@echo "--> Creating ISO image: $(OS_ISO)..."

	xorriso -as mkisofs \
		-b limine-bios-cd.bin \
		--no-emul-boot -boot-load-size 4 -boot-info-table \
		$(ISO_ROOT_DIR) -o $(OS_ISO)
	
	@echo "--> ISO image created successfully: $(OS_ISO)"

# --- QEMU Execution ---
QEMU_FLAGS_COMMON = -m 256M

QEMU_FLAGS = $($(QEMU_FLAGS_$(ARCH))) $(QEMU_FLAGS_COMMON)

run: iso
	$(QEMU) $(QEMU_FLAGS) -cdrom $(OS_ISO)

qemu_debug: iso
	$(QEMU) $(QEMU_FLAGS) -cdrom $(OS_ISO) -s -S

# --- Limine Fetcing ---
fetch-limine:
	@if [ ! -d "$(LIMINE_FILES_DIR)" ]; then mkdir -p $(LIMINE_FILES_DIR); fi
	@sh tools/fetch_limine.sh

check-limine-files:
	@missing_files=0; \
	for file in $(ESSENTIAL_LIMINE_FILES); do \
		if [ ! -f "$$file" ]; then \
			echo "ERROR: Limine file '$$file' is missing."; \
			missing_files=1; \
		fi; \
	done; \
	if [ "$$missing_files" -ne 0 ]; then \
	    echo "--> Some Limine files are missing. Attempting to fetch them..."; \
	    $(MAKE) fetch-limine; \
	    missing_files_after_fetch=0; \
	    for file_after_fetch in $(ESSENTIAL_LIMINE_FILES); do \
	        if [ ! -f "$$file_after_fetch" ]; then \
	            echo "ERROR: Limine file '$$file_after_fetch' is still missing after fetch attempt."; \
	            missing_files_after_fetch=1; \
	        fi; \
	    done; \
	    if [ "$$missing_files_after_fetch" -ne 0 ]; then \
	        echo "Please ensure 'tools/fetch_limine.sh' ran successfully or place the files manually in '$(LIMINE_FILES_DIR)'."; \
	        exit 1; \
	    fi; \
	    echo "--> Limine files successfully fetched."; \
	else \
	    echo "--> All essential Limine files found."; \
	fi

-include $(DEPS)