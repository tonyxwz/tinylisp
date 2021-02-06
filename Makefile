# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := tl 
# compiler
# ========
CC  := clang -std=c17
CXX := clang++ -std=c++17

# Source file
# ===========
SRC_DIRS := ./src
# Find all the C and C++ files we want to compile
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)

# mpc third-party
INC_DIRS += ./third-party/mpc
SRCS += ./third-party/mpc/mpc.c

# compiler & linker flags
# =======================
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Address Sanitizer
ifeq ($(NOASAN), 1)
	ASANFLAGS :=
else	
	ASANFLAGS := -fsanitize=address -fno-omit-frame-pointer
endif
# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS  := $(INC_FLAGS) -MMD -MP -Wall -Wextra -pedantic
LDFLAGS   := -lm -lreadline -lpthread

BUILD_PREFIX := ./build

ifeq ($(BUILD), RELEASE)
	BUILD_DIR := $(BUILD_PREFIX)/release
	CPPFLAGS  += -g -O3
else
	BUILD_DIR := $(BUILD_PREFIX)/debug
	CPPFLAGS  += -g -O0 $(ASANFLAGS) -DDEBUG -D_DEBUG
	LDFLAGS   += -g -O0 -lm -lreadline $(ASANFLAGS)
endif

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# The final build step.
# ====================
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean run
clean:
	rm -r $(BUILD_PREFIX)

run: $(BUILD_DIR)/$(TARGET_EXEC)
	@$(BUILD_DIR)/$(TARGET_EXEC)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
