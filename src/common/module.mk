#
# Module makefile for client module
#
DIR := src/common

SIM_SRC += $(DIR)/comm_man.cc \
	$(DIR)/contiguous_data_type.cc \
	$(DIR)/data_type.cc \
	$(DIR)/file_descriptor.cc \
	$(DIR)/file_view.cc \
	$(DIR)/filename.cc \
	$(DIR)/fs_operation.cc \
	$(DIR)/fs_operation_state.cc \
	$(DIR)/fs_state_machine.cc \
	$(DIR)/io_trace.cc \
	$(DIR)/ip_socket_map.cc \
	$(DIR)/pfs_utils.cc \
	$(DIR)/phtf_io_trace.cc \
	$(DIR)/serial_message_scheduler.cc \
	$(DIR)/shtf_io_trace.cc \
	$(DIR)/struct_data_type.cc \
	$(DIR)/subarray_data_type.cc \
	$(DIR)/umd_io_trace.cc \
	$(DIR)/vector_data_type.cc
