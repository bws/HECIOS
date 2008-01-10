#
# Module makefile for client module
#
DIR := src/client

SIM_SRC += $(DIR)/client_fs_state.cc \
	$(DIR)/fs_client.cc \
	$(DIR)/fs_close.cc \
	$(DIR)/fs_create_directory.cc \
	$(DIR)/fs_open.cc \
	$(DIR)/fs_read.cc \
	$(DIR)/fs_write.cc \
	$(DIR)/io_application.cc \
	$(DIR)/middleware_cache.cc \
	$(DIR)/mpi_middleware.cc \
	$(DIR)/mpi_middleware_bcast_sm.cc
