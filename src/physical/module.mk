#
# Module makefile for physical module
#
DIR := src/physical

SIM_SRC += $(DIR)/beowulf_cluster.ned \
	$(DIR)/compute_node.ned \
	$(DIR)/daemon_process.ned \
	$(DIR)/io_node.ned \
	$(DIR)/job_process.ned
