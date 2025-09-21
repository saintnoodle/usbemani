TARGET_BOARD      ?= lain/tsugaru/rev1
TARGET_CONTROLLER ?= lain/mini-5/rev1

# compile nested directories
SRC += $(wildcard $(ROOT_DIR)/layouts/$(TARGET_LAYOUT)/**/*.c)
