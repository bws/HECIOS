#
# This file is part of Hecios
#
# Copyright (C) 2007,2008,2009 Brad Settlemyer
#
# This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
# for details on this and other legal matters.
#

#
# Tools used to perform builds
#
AR = ar
CP = cp
FIND = find
INSTALL = install
MKDIR = mkdir -p
RM = rm -f
CXX = g++
LD = g++
LEX = flex
PRELINK = ld -Ur --eh-frame-hdr 
RSYNC = rsync

#
# OmNet++ tools
#
NEDC = $(OMNET_DIR)/bin/nedtool
MSGC = $(OMNET_DIR)/bin/opp_msgc

#
# Hecios tools
#
DEPENDC = scripts/depend.sh

#
# Compilation flags
#
CXXFLAGS += $(CONFIG_CXXFLAGS)
CXXFLAGS += -DWITH_PARSIM -DWITH_NETBUILDER
CXXFLAGS += $(patsubst %,-I%,$(INCLUDES))

#
# More permissive compilation flags for generated source (warnings disabled)
#
UNSAFE_CXXFLAGS += $(CONFIG_UNSAFE_CXXFLAGS)
UNSAFE_CXXFLAGS += -DWITH_PARSIM -DWITH_NETBUILDER
UNSAFE_CXXFLAGS += $(patsubst %,-I%,$(INCLUDES))

#
# Dependency generation flags
#
DEPFLAGS = $(patsubst %,-I%,$(INCLUDES)) $(patsubst %,-I%,$(TEST_INCLUDES))

#
# Addtional compilation flags to be used on unit tests
#
TESTCFLAGS = $(patsubst %,-I%,$(TEST_INCLUDES))

#
# Link flags
#
LDFLAGS += $(CONFIG_LDFLAGS)
LDFLAGS += -Wl,--export-dynamic

#
# Nedtool flags
#
NEDFLAGS += $(patsubst %, -I%, $(INCLUDES))

#
# Build LANL Trace Parsing tool
#
TOOLS_LANL_TRACE_PARSER_OBJS = $(SRC_DIR)/common/phtf_io_trace.o \
								$(SRC_DIR)/tools/lanl_trace_parser.o \
								$(SRC_DIR)/tools/lanl_trace_parser_main.o

$(BIN_DIR)/lanl_trace_parser: $(TOOLS_LANL_TRACE_PARSER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $(TOOLS_LANL_TRACE_PARSER_OBJS) -o $@

#
# Build LANL Trace Scanning tool
#
TOOLS_LANL_TRACE_SCANNER_OBJS = $(SRC_DIR)/common/phtf_io_trace.o \
								$(SRC_DIR)/tools/lanl_trace_scan_actions.o \
								$(SRC_DIR)/tools/lanl_trace_scanner.o \
								$(SRC_DIR)/tools/lanl_trace_scanner_main.o


$(BIN_DIR)/lanl_trace_scanner: $(TOOLS_LANL_TRACE_SCANNER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $(TOOLS_LANL_TRACE_SCANNER_OBJS) -lfl -o $@
