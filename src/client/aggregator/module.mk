#
# Module makefile for client module
#
DIR := src/client/aggregator

SIM_SRC +=  $(DIR)/aggregation_io.cc \
		$(DIR)/aggregator_access_strategy.cc \
		$(DIR)/data_sieving_access_strategy.cc \
		$(DIR)/data_sieving_middleware_aggregator.cc \
		$(DIR)/middleware_aggregator.cc \
		$(DIR)/no_middleware_aggregator.cc \
		$(DIR)/view_aware_access_strategy.cc \
		$(DIR)/view_aware_middleware_aggregator.cc
		