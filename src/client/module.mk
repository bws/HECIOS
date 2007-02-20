#
# Module makefile for client module
#
DIR := src/client

SIM_SRC += $(DIR)/fs_client.ned \
	$(DIR)/io_application.ned \
	$(DIR)/middleware_cache.ned \
	$(DIR)/mpi_process.ned \
	$(DIR)/fs_client.cc \
	$(DIR)/io_application.cc \
	$(DIR)/middleware_cache.cc
