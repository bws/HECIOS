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
	$(CXX) -c -g $(CXXFLAGS) $(TEST_INCLUDES) $< -o $@
