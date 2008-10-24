#
# Module makefile for client module
#
DIR := src/client/cache

SIM_SRC +=  $(DIR)/direct_paged_middleware_cache.cc \
		$(DIR)/middleware_cache.cc \
		$(DIR)/paged_cache.cc \
		$(DIR)/shared_direct_paged_middleware_cache.cc
