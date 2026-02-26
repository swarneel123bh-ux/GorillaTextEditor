TARGET := main
SRC_DIR := src
INC_DIR := src/include
BIN_DIR := bin
ROBJ_DIR := $(BIN_DIR)/robj
DOBJ_DIR := $(BIN_DIR)/dobj

ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    EXE_EXT := .exe
    RM := del /Q /F
    RMDIR := rmdir /S /Q
    MKDIR := mkdir
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM := linux
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM := macos
    endif

    EXE_EXT :=
    RM := rm -f
    RMDIR := rm -rf
    MKDIR := mkdir -p
endif

CC := gcc
COMMON_FLAGS := -Wall -Wextra -I$(INC_DIR) -MMD -MP
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O2 -DNDEBUG
NCURSES_LIB := $(shell pkg-config --libs ncurses 2>/dev/null || echo -lncurses)
COMMON_FLAGS += $(shell pkg-config --cflags ncurses 2>/dev/null)
SRCS := $(wildcard $(SRC_DIR)/*.c)
ROBJ := $(patsubst $(SRC_DIR)/%.c,$(ROBJ_DIR)/%.o,$(SRCS))
DOBJ := $(patsubst $(SRC_DIR)/%.c,$(DOBJ_DIR)/%.o,$(SRCS))
RDEPS := $(ROBJ:.o=.d)
DDEPS := $(DOBJ:.o=.d)
RELEASE_BIN := $(BIN_DIR)/$(TARGET)$(EXE_EXT)
DEBUG_BIN := $(BIN_DIR)/$(TARGET)dbg$(EXE_EXT)

.PHONY: all
all: release debug

.PHONY: release debug

release: $(RELEASE_BIN)
debug: $(DEBUG_BIN)

$(RELEASE_BIN): $(ROBJ) | $(BIN_DIR)
	$(CC) $(ROBJ) -o $@ $(NCURSES_LIB)

$(DEBUG_BIN): $(DOBJ) | $(BIN_DIR)
	$(CC) $(DOBJ) -o $@ $(NCURSES_LIB)

$(ROBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(ROBJ_DIR)
	$(CC) $(COMMON_FLAGS) $(RELEASE_FLAGS) -c $< -o $@

$(DOBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(DOBJ_DIR)
	$(CC) $(COMMON_FLAGS) $(DEBUG_FLAGS) -c $< -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

$(ROBJ_DIR): | $(BIN_DIR)
	$(MKDIR) $(ROBJ_DIR)

$(DOBJ_DIR): | $(BIN_DIR)
	$(MKDIR) $(DOBJ_DIR)

.PHONY: run run_debug

run: release
ifeq ($(PLATFORM),windows)
	$(RELEASE_BIN)
else
	./$(RELEASE_BIN)
endif

run_debug: debug
ifeq ($(PLATFORM),windows)
	$(DEBUG_BIN)
else
	./$(DEBUG_BIN)
endif

.PHONY: clean
clean:
ifeq ($(PLATFORM),windows)
	if exist $(BIN_DIR) $(RMDIR) $(BIN_DIR)/
else
	$(RMDIR) $(BIN_DIR)/
endif

-include $(RDEPS)
-include $(DDEPS)
