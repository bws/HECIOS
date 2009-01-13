#
# Module makefile for client module
#
DIR := src/client/cache

SIM_SRC +=  $(DIR)/direct_paged_middleware_cache.cc \
		$(DIR)/file_region_set.cc \
		$(DIR)/middleware_cache.cc \
		$(DIR)/multi_cache.cc \
		$(DIR)/paged_cache.cc \
		$(DIR)/paged_middleware_cache_with_twin.cc \
		$(DIR)/paged_middleware_cache_with_twin_no_block_indexed.cc \
		$(DIR)/shared_direct_paged_middleware_cache.cc \
		$(DIR)/shared_paged_middleware_cache_with_twin.cc
		