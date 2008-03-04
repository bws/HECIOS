#
# Module makefile for the io component
#
DIR := src/io

SIM_SRC += $(DIR)/data_flow.cc \
	$(DIR)/data_flow_registry.cc \
	$(DIR)/bmi_list_io_data_flow.cc \
	$(DIR)/bmi_memory_data_flow.cc \
	$(DIR)/job_manager.cc
