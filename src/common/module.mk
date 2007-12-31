#
# Module makefile for client module
#
DIR := src/common

SIM_SRC += $(DIR)/basic_data_type.cc \
	$(DIR)/contiguous_data_type.cc \
	$(DIR)/data_type.cc \
	$(DIR)/file_descriptor.cc \
	$(DIR)/file_view.cc \
	$(DIR)/filename.cc \
	$(DIR)/io_trace.cc \
	$(DIR)/ip_socket_map.cc \
	$(DIR)/pfs_utils.cc \
	$(DIR)/umd_io_trace.cc \
	$(DIR)/vector_data_type.cc
