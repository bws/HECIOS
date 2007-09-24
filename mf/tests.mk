#
# This file is part of Hecios
#
# Copyright (C) 2007 Brad Settlemyer
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

#
# Testing macros local to file
#
TEST_DIR := $(HECIOS_DIR)/tests

#
# Testing directories
#
TEST_CLIENT_DIR := $(TEST_DIR)/client
TEST_COMMON_DIR := $(TEST_DIR)/common
TEST_LAYOUT_DIR := $(TEST_DIR)/layout
TEST_OS_DIR := $(TEST_DIR)/os
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
include $(TEST_CLIENT_DIR)/module.mk
include $(TEST_COMMON_DIR)/module.mk
include $(TEST_LAYOUT_DIR)/module.mk
include $(TEST_OS_DIR)/module.mk
include $(TEST_SUPPORT_DIR)/module.mk


#
# Testing dependencies
#
SIM_TEST_DEPENDS := $(patsubst %.cc, %.d, $(filter %.cc, $(SIM_TEST_SRC)))
ifeq ($(filter test, $(MAKECMDGOALS)), test)
-include $(SIM_TEST_DEPENDS)
endif

#
# Derived files for tests
#
SIM_TEST_OBJS := $(patsubst %.cc, %.o, $(filter %.cc, $(SIM_TEST_SRC)))

#
# Build unit test drivers
#
tests_all: $(BIN_DIR)/common_test $(BIN_DIR)/client_test $(BIN_DIR)/layout_test $(BIN_DIR)/os_test

#
# Cleanup test subsystem
#
tests_clean:
	@echo "Removing test subsytem derived objects."
	$(RM) $(SIM_TEST_OBJS)
	$(RM) $(SIM_TEST_DEPENDS)
	@echo "Derived files deleted."

#
# client module unit tests
#
CLIENT_TEST_OBJS = $(TEST_CLIENT_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/client_test: $(CLIENT_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) -g $^ $(TEST_LIBS) -o $@

#
# common module unit tests
#
COMMON_TEST_OBJS = $(TEST_COMMON_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/common_test: $(COMMON_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) -g $^ $(TEST_LIBS) -o $@

#
# layout module unit tests
#
LAYOUT_TEST_OBJS = $(TEST_LAYOUT_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/layout_test: $(LAYOUT_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) -g $^ $(TEST_LIBS) -o $@

#
# OS module unit tests
#
OS_TEST_OBJS = $(TEST_OS_DIR)/unit_test.o \
	$(TEST_SUPPORT_DIR)/csimple_module_tester.o \
	$(TEST_SUPPORT_DIR)/test_cenvir.o

$(BIN_DIR)/os_test: $(OS_TEST_OBJS) $(SIM_MSG_OBJS) $(SIM_OBJS) lib/inet.o
	@mkdir -p $(BIN_DIR)
	$(LD) -g $^ $(TEST_LIBS) -o $@
