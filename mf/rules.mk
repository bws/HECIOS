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
# Useful macro functions
#
dirname = $(patsubst %/,%,$(dir $(1)))

#
# Document generation rules (nasty current directory stuff to handle
# all the crazy output by latex)
#
%.dvi: %.tex
	(cd $(@D) && pslatex $(<F))
	(cd $(@D) && pslatex $(<F))

%.pdf: %.dvi
	(cd $(@D) && dvipdf -o $(@F) $(<F))

%.eps: %.fig
	fig2dev -L eps -p 1 $^ $@

%.eps: %.eps
	cp $^ $@

%.pdf: %.fig
	fig2dev -L pdf -p 1 $^ $@

%.pdf: %.eps
	epstopdf --outfile=$@ $^

#
# Generate source files from NED specs
#
%_n.cc: %.ned
	$(NEDC) $(NEDFLAGS) $<

%_m.cc: %.msg
	$(MSGC) $(NEDFLAGS) $<

%_m.h: %.msg
	$(MSGC) $(NEDFLAGS) $<

#
# Compile message files at a lower warning level
#
%_m.o: %_m.cc
	$(CXX) $(UNSAFE_CXXFLAGS) -c -o $@ $<

#
# Generate dependency information
#
%.d: %.c
	@echo "Generating dependencies for $< . . ."
	@$(DEPENDC) $(call dirname,$<) $(CC) -MM $(DEPFLAGS) $< > $@

%.d: %.cc $(SIM_MSG_OUTPUT)
	@echo "Generating dependencies for $< . . ."
	@$(DEPENDC) $(call dirname,$<) $(CXX) -MM $(DEPFLAGS) $< > $@

#
# Build rules for CppUnit tests
#
%_test.o: %_test.cc
	$(CXX) -c $(CXXFLAGS) $(TESTCFLAGS) $< -o $@
