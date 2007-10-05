#
# Module makefile for messages module
#
DIR := src/messages

SIM_SRC += $(DIR)/bmi_proto.msg \
	$(DIR)/cache_proto.msg \
	$(DIR)/mpi_proto.msg \
	$(DIR)/network_proto.msg \
	$(DIR)/os_proto.msg \
	$(DIR)/pvfs_proto.msg \
	$(DIR)/mpi_mid.msg
