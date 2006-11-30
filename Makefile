#
# Master Makefile default targets
#
TARGETS = doc

#
# Module locations
#
DOC_DIR = doc

#
# Include all make system makefiles
#
include mf/rules.mk
include $(DOC_DIR)/module.mk

#
# Module build targets
#
DOC_PDF := $(patsubst %.tex,%.pdf, $(filter %.tex,$(DOC_SRC)))


#
# Top level psuedo targets
#
all: $(TARGETS)

doc: $(DOC_PDF)
	echo "DOC_PDF: $(DOC_PDF)"

#
# Required by make command
#
.PHONY: all doc
