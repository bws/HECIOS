#
# Module makefile for physical module
#
DIR := src/server

SIM_SRC += $(DIR)/fs_server.cc \
	$(DIR)/fs_server_configurator.cc \
	$(DIR)/create.cc \
	$(DIR)/lookup.cc
