#
# Module makefile for physical module
#
DIR := src/os

SIM_SRC += $(DIR)/access_manager.cc \
	$(DIR)/block_cache.cc \
	$(DIR)/block_compare.cc \
	$(DIR)/block_translator.cc \
	$(DIR)/disk_scheduler.cc \
	$(DIR)/file_system.cc \
	$(DIR)/io_library.cc \
	$(DIR)/system_call_interface.cc
