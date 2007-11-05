#
# Module makefile for physical module
#
DIR := src/os

SIM_SRC += $(DIR)/access_manager.cc \
	$(DIR)/buffer_cache.cc \
	$(DIR)/block_translator.cc \
	$(DIR)/disk_scheduler.cc \
	$(DIR)/file_system.cc \
	$(DIR)/fixed_inode_storage_layout.cc \
	$(DIR)/io_library.cc \
	$(DIR)/storage_layout.cc \
	$(DIR)/storage_layout_manager.cc \
	$(DIR)/system_call_interface.cc
