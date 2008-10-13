#
# Tools used to perform builds
#
AR = ar
CP = cp
INSTALL = install
MKDIR = mkdir -p
RM = rm -f
CXX = g++
LD = g++

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
								$(SRC_DIR)/tools/parser_sm.o \

$(BIN_DIR)/lanl_trace_parser: $(TOOLS_LANL_TRACE_PARSER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $(TOOLS_LANL_TRACE_PARSER_OBJS) -o $@

