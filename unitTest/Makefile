
#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -rf
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -rf
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
endif

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif


UNITY_ROOT=extern/Unity
FFF_ROOT=extern/fff

LCOV_DIR=./lcov
BUILD_DIR=./build

CFLAGS=-std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition
CFLAGS += -g
CFLAGS += --coverage
#CFLAGS += -Wno-misleading-indentation

TEST_SRC_FILES=\
	test_linkedList.c \
	test_runner/test_linkedList_runner.c \
	$(UNITY_ROOT)/src/unity.c 

TEST_INC=\
	-I../inc \
	-I${FFF_ROOT} \
	-I$(UNITY_ROOT)/src

TEST_TARGET_SINGLE_LINKED_NOT_SIZE_AWARE = $(BUILD_DIR)/test_singleLinked_notSizeAware$(TARGET_EXTENSION) 
TEST_TARGET_SINGLE_LINKED_SIZE_AWARE = $(BUILD_DIR)/test_singleLinked_sizeAware$(TARGET_EXTENSION) 

TEST_TARGET_DUALLY_LINKED_NOT_SIZE_AWARE = $(BUILD_DIR)/test_duallyLinked_notSizeAware$(TARGET_EXTENSION) 
TEST_TARGET_DUALLY_LINKED_SIZE_AWARE = $(BUILD_DIR)/test_duallyLinked_sizeAware$(TARGET_EXTENSION) 

all: clean runTest

runTest:\
	$(TEST_TARGET_SINGLE_LINKED_NOT_SIZE_AWARE) \
	$(TEST_TARGET_SINGLE_LINKED_SIZE_AWARE) \
	$(TEST_TARGET_DUALLY_LINKED_NOT_SIZE_AWARE) \
	$(TEST_TARGET_DUALLY_LINKED_SIZE_AWARE) 

	- $(TEST_TARGET_SINGLE_LINKED_NOT_SIZE_AWARE)
	- $(TEST_TARGET_SINGLE_LINKED_SIZE_AWARE)
	- $(TEST_TARGET_DUALLY_LINKED_NOT_SIZE_AWARE)
	- $(TEST_TARGET_DUALLY_LINKED_SIZE_AWARE)


$(TEST_TARGET_SINGLE_LINKED_NOT_SIZE_AWARE): $(TEST_SRC_FILES)
	$(MKDIR) $(BUILD_DIR)
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $^ -o $@

$(TEST_TARGET_SINGLE_LINKED_SIZE_AWARE): $(TEST_SRC_FILES)
	$(MKDIR) $(BUILD_DIR)
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $^ -o $@ -DLINKED_LIST_BUFFER_SIZE_AWARE

$(TEST_TARGET_DUALLY_LINKED_NOT_SIZE_AWARE): $(TEST_SRC_FILES)
	$(MKDIR) $(BUILD_DIR)
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $^ -o $@ -DLINKED_LIST_BUFFER_DUALLY_LINKED

$(TEST_TARGET_DUALLY_LINKED_SIZE_AWARE): $(TEST_SRC_FILES)
	$(MKDIR) $(BUILD_DIR)
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $^ -o $@ -DLINKED_LIST_BUFFER_SIZE_AWARE -DLINKED_LIST_BUFFER_DUALLY_LINKED

test_runner/test_linkedList_runner.c: ./test_linkedList.c 
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test_linkedList.c  test_runner/test_linkedList_runner.c
	
clean:
	$(CLEANUP) $(BUILD_DIR)
	$(CLEANUP) $(LCOV_DIR)
	$(CLEANUP) test_runner/*
