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
# Compilation flags
#
CXXFLAGS += -Wno-unused
CXXFLAGS += -DNDEBUG=1 
CXXFLAGS += -DWITH_PARSIM -DWITH_NETBUILDER
CXXFLAGS += $(patsubst %,-I%,$(INCLUDES))

#
# Link flags
#
LDFLAGS += -Wl,--export-dynamic

#
# Nedtool flags
#
NEDFLAGS += $(patsubst %, -I%, $(INCLUDES))