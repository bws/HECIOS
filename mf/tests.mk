#
# This file is part of Hecios
#
# Copyright (C) 2007,2008,2009 Brad Settlemyer
#
# This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
# for details on this and other legal matters.
#

#
# Testing macros local to file
#
TEST_DIR := tests

#
# Testing directories
#
TEST_CLIENT_CACHE_DIR := $(TEST_DIR)/client/cache
TEST_CLIENT_DIR := $(TEST_DIR)/client
TEST_COMMON_DIR := $(TEST_DIR)/common
TEST_IO_DIR := $(TEST_DIR)/io
TEST_LAYOUT_DIR := $(TEST_DIR)/layout
TEST_OS_DIR := $(TEST_DIR)/os
TEST_PHYSICAL_DIR := $(TEST_DIR)/physical
TEST_SERVER_DIR := $(TEST_DIR)/server
TEST_SUPPORT_DIR := $(TEST_DIR)/support

TEST_LIBS := -L$(OMNET_DIR)/lib -L$(LIB_DIR) \
	-lsim_std -lnedxml -lxml2 \
	-lcppunit -ldl

#
# Testing macros defined and used elsewhere
#
TEST_INCLUDES = $(TEST_SUPPORT_DIR)

#
# Testing module includes
#
SIM_TEST_SRC :=
include $(TEST_CLIENT_CACHE_DIR)/module.mk
include $(TEST_CLIENT_DIR)/module.mk
include $(TEST_COMMON_DIR)/module.mk
include $(TEST_IO_DIR)/module.mk
include $(TEST_LAYOUT_DIR)/module.mk
include $(TEST_OS_DIR)/module.mk
include $(TEST_PHYSICAL_DIR)/module.mk
include $(TEST_SERVER_DIR)/module.mk
include $(TEST_SUPPORT_DIR)/module.mk

#
# Testing dependencies
#
SIM_TEST_DEPENDS := $(patsubst %.cc, %.d, $(filter %.cc, $(SIM_TEST_SRC)))

#
# Include testing dependencies if a test target is being built
#
TEST_MAKECMDGOALS := $(filter %test, $(MAKECMDGOALS))
TEST_MAKECMDGOALS += $(filter incremental, $(MAKECMDGOALS))
TEST_MAKECMDGOALS += $(filter tests_%, $(MAKECMDGOALS))
ifneq ($(strip $(TEST_MAKECMDGOALS)),)
-include $(SIM_TEST_DEPENDS)
endif

#
# Derived files for tests
#
SIM_TEST_OBJS := $(patsubst %.cc, %.o, $(filter %.cc, $(SIM_TEST_SRC)))

#
# Build unit test drivers
#
TEST_EXES = $(BIN_DIR)/common_test \
	$(BIN_DIR)/client_cache_test \
	$(BIN_DIR)/client_test \
	$(BIN_DIR)/io_test \
	$(BIN_DIR)/layout_test \
	$(BIN_DIR)/os_test \
	$(BIN_DIR)/physical_test \
	$(BIN_DIR)/server_test

tests_all: $(TEST_EXES)

tests_incremental: $(SIM_TEST_OBJS)
	@#echo "Test Objs:  $^"


.Phony: tests_all tests_incremental

#
# Cleanup test subsystem
#
tests_clean:
	@echo "Removing test subsytem derived objects."
	$(RM) $(SIM_TEST_OBJS)
	$(RM) $(SIM_TEST_DEPENDS)
	@echo "Derived files deleted."

.PHONY: tests_clean

tests_depclean:
	$(RM) $(SIM_TEST_DEPENDS)

.PHONY: tests_depclean

#
# client cache package unit tests
#
CLIENT_CACHE_TEST_OBJS = $(TEST_CLIENT_CACHE_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/csimple_module_tester.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/client_cache_test: $(CLIENT_CACHE_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

#
# client package unit tests
#
CLIENT_TEST_OBJS = $(TEST_CLIENT_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/csimple_module_tester.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/client_test: $(CLIENT_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

#
# common package unit tests
#
COMMON_TEST_OBJS = $(TEST_COMMON_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/common_test: $(COMMON_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

#
# io package unit tests
#
IO_TEST_OBJS = $(TEST_IO_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/csimple_module_tester.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/io_test: $(IO_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

#
# layout package unit tests
#
LAYOUT_TEST_OBJS = $(TEST_LAYOUT_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/layout_test: $(LAYOUT_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

#
# OS package unit tests
#
OS_TEST_OBJS = $(TEST_OS_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/csimple_module_tester.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/os_test: $(OS_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

#
# Physical package unit tests
#
PHYSICAL_TEST_OBJS = $(TEST_PHYSICAL_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/csimple_module_tester.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/physical_test: $(PHYSICAL_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

#
# Server package unit tests
#
SERVER_TEST_OBJS = $(TEST_SERVER_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/csimple_module_tester.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/server_test: $(SERVER_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

