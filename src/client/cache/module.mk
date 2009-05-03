#
# Module makefile for client module
#
DIR := src/client/cache

SIM_SRC +=  $(DIR)/direct_paged_middleware_cache.cc \
		$(DIR)/middleware_cache.cc \
		$(DIR)/multi_cache.cc \
		$(DIR)/page_access_mixin.cc \
		$(DIR)/paged_cache.cc \
		$(DIR)/paged_middleware_cache_mesi.cc \
		$(DIR)/paged_middleware_cache_with_twin.cc \
		$(DIR)/paged_middleware_cache_with_twin_no_block_indexed.cc \
		$(DIR)/progressive_page_access_strategy.cc \
		$(DIR)/progressive_page_cache.cc \
		$(DIR)/progressive_paged_middleware_cache.cc \
		$(DIR)/shared_direct_paged_middleware_cache.cc \
		$(DIR)/shared_paged_middleware_cache_with_twin.cc \
		$(DIR)/shared_paged_middleware_cache_with_twin_no_block_indexed.cc
		