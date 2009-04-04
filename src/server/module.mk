#
# Module makefile for the server subsystem
#
DIR := src/server

SIM_SRC += $(DIR)/change_dir_ent.cc \
	$(DIR)/collective_create.cc \
	$(DIR)/collective_get_attr.cc \
	$(DIR)/collective_remove.cc \
	$(DIR)/create.cc \
	$(DIR)/create_dir_ent.cc \
	$(DIR)/fs_server.cc \
	$(DIR)/get_attr.cc \
	$(DIR)/lookup.cc \
	$(DIR)/read.cc \
	$(DIR)/read_dir.cc \
	$(DIR)/request_scheduler.cc \
	$(DIR)/remove.cc \
	$(DIR)/remove_dir_ent.cc \
	$(DIR)/set_attr.cc \
	$(DIR)/write.cc
