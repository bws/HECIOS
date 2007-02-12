

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
	$(NEDC) $(NEDFLAGS) $^

%_m.cc: %.msg
	$(MSGC) $(NEDFLAGS) $^

#
# Generate dependency information
#
%.d: %.c
	@echo "Generating dependencies for $< . . ."
	@$(CC) -MM $(DEPFLAGS) $< > $@

%.d: %.cc
	@echo "Generating dependencies for $< . . ."
	@$(CXX) -MM $(DEPFLAGS) $< > $@

#
# Build rules for CppUnit tests
#
%_test.o: %_test.cc
	$(CXX) -c -g $(CXXFLAGS) $(TEST_INCLUDES) $< -o $@
