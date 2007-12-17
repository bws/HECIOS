#
# Module makefile for the server subsystem
#
DIR := src/server

SIM_SRC += $(DIR)/create.cc \
	$(DIR)/fs_server.cc \
	$(DIR)/get_attr.cc \
	$(DIR)/lookup.cc \
	$(DIR)/read.cc \
	$(DIR)/set_attr.cc \
	$(DIR)/write.cc
