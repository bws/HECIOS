#
# Module makefile for client module
#
DIR := src/client

SIM_SRC += $(DIR)/client_fs_state.cc \
	$(DIR)/fs_client.cc \
	$(DIR)/fs_open.cc \
	$(DIR)/io_application.cc \
	$(DIR)/middleware_cache.cc \
	$(DIR)/cache_module.cc \
	$(DIR)/fsModule.cc
