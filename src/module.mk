#
# Module makefile for src
#
DIR := src

SIM_SRC += $(DIR)/cluster.ned \
	$(DIR)/compute_node.ned \
	$(DIR)/io_node.ned \
	$(DIR)/switch.ned \
	$(DIR)/compute_node.cc \
	$(DIR)/io_node.cc \
	$(DIR)/switch.cc \
	$(DIR)/file-sender-receiver.ned
