
$(INET_DIR)/bin/INET: $(INET_DIR)/.tstamp
	cd INET && ./makemake
	cd INET && make ROOT=$(shell pwd)/$(INET_DIR)

build_inet: $(INET_DIR)/bin/INET
	@echo "INET Framework build complete."

.PHONY: build_inet