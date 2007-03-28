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
CXXFLAGS += -g
CXXFLAGS += -Wno-unused -Wall -Werror
CXXFLAGS += -DNDEBUG=1 
CXXFLAGS += -DWITH_PARSIM -DWITH_NETBUILDER
CXXFLAGS += $(patsubst %,-I%,$(INCLUDES))

#
# More permissive compilation flags for generated source (warnings disabled)
#
UNSAFE_CXXFLAGS += -g
UNSAFE_CXXFLAGS += -DNDEBUG=1 -DWITH_PARSIM -DWITH_NETBUILDER
UNSAFE_CXXFLAGS += $(patsubst %,-I%,$(INCLUDES))

#
# Dependency generation flags
#
DEPFLAGS = $(patsubst %,-I%,$(INCLUDES))

#
# Link flags
#
LDFLAGS += -Wl,--export-dynamic

#
# Nedtool flags
#
NEDFLAGS += $(patsubst %, -I%, $(INCLUDES))
