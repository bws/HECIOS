#
# Module makefile for the server subsystem
#
DIR := src/server

SIM_SRC += $(DIR)/create.cc \
	$(DIR)/data_flow.cc \
	$(DIR)/fs_server.cc \
	$(DIR)/job_manager.cc \
	$(DIR)/get_attr.cc \
	$(DIR)/bmi_list_io_data_flow.cc \
	$(DIR)/lookup.cc \
	$(DIR)/read.cc \
	$(DIR)/set_attr.cc \
	$(DIR)/write.cc
