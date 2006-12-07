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
include mf/tools.mk

#
# Include module makefiles
#
include $(DOC_DIR)/module.mk

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
# Top level psuedo targets
#
all: $(TARGETS)

clean:
	$(RM) $(DOC_DVI) $(DOC_PDF) $(DOC_PS) $(DOC_CRUFT)

doc: $(DOC_EPS) $(DOC_PDF)
	@#echo "DOC_PDF: $(DOC_PDF) DOC_DIR: $(DOC_DIR)"


#
# Required by make command
#
.PHONY: all clean doc
