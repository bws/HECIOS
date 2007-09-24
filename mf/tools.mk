#
# Tools used to perform builds
#
AR = ar
CP = cp
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
LDFLAGS += -g -Wl,--export-dynamic

#
# Nedtool flags
#
NEDFLAGS += $(patsubst %, -I%, $(INCLUDES))
