#
# Module makefile for client module
#
DIR := src/client

SIM_SRC += $(DIR)/client_fs_state.cc \
	$(DIR)/fs_client.cc \
	$(DIR)/fs_client_operation.cc \
	$(DIR)/fs_create_sm.cc \
	$(DIR)/fs_create_directory_operation.cc \
	$(DIR)/fs_create_directory_sm.cc \
	$(DIR)/fs_collective_create_sm.cc \
	$(DIR)/fs_collective_get_attributes_sm.cc \
	$(DIR)/fs_collective_remove_sm.cc \
	$(DIR)/fs_lookup_name_sm.cc \
	$(DIR)/fs_get_attributes_sm.cc \
	$(DIR)/fs_read_directory_sm.cc \
	$(DIR)/fs_read_sm.cc \
	$(DIR)/fs_remove_sm.cc \
	$(DIR)/fs_set_attributes_sm.cc \
	$(DIR)/fs_write_sm.cc \
	$(DIR)/fs_close_operation.cc \
	$(DIR)/fs_delete_operation.cc \
	$(DIR)/fs_open_operation.cc \
	$(DIR)/fs_read_directory_operation.cc \
	$(DIR)/fs_read_operation.cc \
	$(DIR)/fs_stat_operation.cc \
	$(DIR)/fs_update_time_operation.cc \
	$(DIR)/fs_write_operation.cc \
	$(DIR)/io_application.cc \
	$(DIR)/middleware_cache.cc \
	$(DIR)/mpi_communication_helper.cc \
	$(DIR)/mpi_middleware.cc \
	$(DIR)/phtf_io_application.cc \
	$(DIR)/shtf_io_application.cc
