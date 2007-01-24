

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
