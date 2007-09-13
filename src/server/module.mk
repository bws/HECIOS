#
# Module makefile for physical module
#
DIR := src/server

SIM_SRC += $(DIR)/fs_server.cc \
	$(DIR)/create.cc \
	$(DIR)/get_attr.cc \
	$(DIR)/lookup.cc \
	$(DIR)/read.cc \
	$(DIR)/write.cc
