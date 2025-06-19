BUILD_SYSTEM = CMake
ARCH         = pico
ARCH_PATH    = $(ROOT_DIR)/arch/pico

PICO_BOARD             ?= pico
PICO_PLATFORM          ?= rp2040
PICO_BOARD_HEADER_DIRS ?= $(ROOT_DIR)/boards/$(TARGET_BOARD)

SRC += $(wildcard $(ARCH_PATH)/*.c)
SRC += $(wildcard $(ARCH_PATH)/impl/*.c)

INCLUDES += $(ARCH_PATH)/tinyusb/
INCLUDES += $(ARCH_PATH)/

.PHONY: all clean cmake_prep

cmake_prep:
	@mkdir -p $(OBJDIR)/.generated && cd $(OBJDIR) && cmake -DTARGET_NAME=${CLEAN_NAME} -DPICO_BOARD_HEADER_DIRS=$(PICO_BOARD_HEADER_DIRS) -DPICO_BOARD=$(PICO_BOARD) -DPICO_PLATFORM=$(PICO_PLATFORM) -DROOT_DIR=$(ROOT_DIR) $(ROOT_DIR)/arch/pico/ -DOBJ_DIR=$(OBJDIR) -DINCLUDES="$(INCLUDES)" -DSRC="$(SRC)"

all: cmake_prep
	@cd $(OBJDIR) && make

clean: cmake_prep
	@cd $(OBJDIR) && make clean

flash: flash_$(BUILD_OS)

flash_Darwin:
	@cp -X build/$(CLEAN_NAME).uf2 /Volumes/RPI-RP2/
