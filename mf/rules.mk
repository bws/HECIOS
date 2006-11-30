

#
# Document generation rules
#
%.dvi: %.tex
	pslatex $<

%.eps: %.fig
	fig2dev -L eps -p 1 $^ $@

%.eps: %.eps
	cp $^ $@

%.pdf: %.fig
	fig2dev -L pdf -p 1 $^ $@

%.pdf: %.eps
	epstopdf --outfile=$@ $^

%.pdf: %.dvi
	dvipdfm $<

%.pdf: %.tex
	pdflatex -output-directory $(DOC_DIR) $<

