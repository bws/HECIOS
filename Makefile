#
# Master Makefile default targets
#
TARGETS = doc build_hecios

#
# System paths
#
HECIOS_DIR := $(shell pwd)
OMNET_DIR := /parl/bradles/projects/oppsim/omnetpp-3.2p1

#
# Include all make system makefiles
#
include mf/rules.mk
include mf/tools.mk

#
# Module locations
#
DOC_DIR = doc
SRC_DIR = src
INET_DIR = INET

#
# Include path (directories to search for include files)
#
INCLUDES = $(OMNET_DIR)/include \
        $(INET_DIR) \
        $(SRC_DIR)

#
# Libraries to link
#
LIBS = -L$(OMNET_DIR)/lib \
        -lenvir -ltkenv -ltk8.4 -ltcl8.4 -lsim_std -lnedxml -lxml2 -ldl \
        -lstdc++

#
# Variables used by the individual modules
#
DOC_SRC :=
SIM_SRC :=

# Include module makefiles
#
include mf/inet.mk
include $(DOC_DIR)/module.mk
include $(SRC_DIR)/module.mk

#
# Doc Module build targets
#
DOC_DVI := $(patsubst %.tex,%.dvi, $(filter %.tex,$(DOC_SRC)))
DOC_PDF := $(patsubst %.tex,%.pdf, $(filter %.tex,$(DOC_SRC)))
DOC_PS := $(patsubst %.tex,%.ps, $(filter %.tex,$(DOC_SRC)))
DOC_EPS := $(patsubst %.fig,%.eps, $(filter %.fig,$(DOC_SRC)))

#
# Latex creates a bunch of spurious files, add those to clean list
#
DOC_CRUFT := $(patsubst %.tex,%.aux, $(filter %.tex,$(DOC_SRC)))
DOC_CRUFT += $(patsubst %.tex,%.toc, $(filter %.tex,$(DOC_SRC)))
DOC_CRUFT += $(patsubst %.tex,%.log, $(filter %.tex,$(DOC_SRC)))

#
# Src Module build targets
#
SIM_NED_OUTPUT := $(patsubst %.ned,%_n.cc, $(filter %.ned, $(SIM_SRC)))
SIM_NED_OBJS := $(patsubst %.ned,%_n.o, $(filter %.ned, $(SIM_SRC)))
SIM_OBJS := $(patsubst %.cc, %.o, $(filter %.cc, $(SIM_SRC)))

#
# Top level psuedo targets
#
all: $(TARGETS)

clean:
	@echo "Removing all derived files."
	$(RM) $(DOC_DVI) $(DOC_EPS) $(DOC_PDF) $(DOC_PS) $(DOC_CRUFT)
	$(RM) $(SIM_NED_OUTPUT) $(SIM_NED_OBJS) $(SIM_OBJS)
	@echo "Derived files deleted.  Project is clean."

doc: $(DOC_EPS) $(DOC_PDF)
	@#echo "DOC_PDF: $(DOC_PDF) DOC_DIR: $(DOC_DIR)"

#
# Build targets for 3rd party frameworks
#
third_party: build_inet

#
# Targets to hecios build simulator
#
ned_gen: $(SIM_NED_OUTPUT)
	echo "Ned translation complete"

hecios: $(SIM_NED_OBJS) $(SIM_OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

build_hecios: third_party hecios

#
# Psuedotargets (required by make for some dependencies)
#
.PHONY: all build_inet build_hecios clean doc ned_gen third_party
