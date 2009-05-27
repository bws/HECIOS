#
# Module makefile for client module
#
DIR := src/client/aggregator

SIM_SRC +=  $(DIR)/middleware_aggregator.cc \
		$(DIR)/data_sieving_middleware_aggregator.cc \
		$(DIR)/no_middleware_aggregator.cc \
		$(DIR)/view_aware_middleware_aggregator.cc
		