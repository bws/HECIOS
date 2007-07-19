#
# Module makefile for client module
#
DIR := src/client

SIM_SRC += $(DIR)/client_fs_state.cc \
	$(DIR)/cache_module.cc \
	$(DIR)/fs_close.cc \
	$(DIR)/fs_open.cc \
	$(DIR)/fs_read.cc \
	$(DIR)/fs_write.cc \
	$(DIR)/fs_module.cc \
	$(DIR)/io_application.cc \
	$(DIR)/middleware_cache.cc \
	$(DIR)/mpi_configurator.cc
