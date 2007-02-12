#
# Module makefile for client module
#
DIR := src/client

SIM_SRC += $(DIR)/bmi_transport.ned \
	$(DIR)/fs_client.ned \
	$(DIR)/io_application.ned \
	$(DIR)/middleware_cache.ned \
	$(DIR)/mpi_process.ned \
	$(DIR)/io_application.cc
