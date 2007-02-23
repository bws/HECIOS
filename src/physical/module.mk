#
# Module makefile for physical module
#
DIR := src/physical

SIM_SRC += $(DIR)/beowulf_cluster.ned \
	$(DIR)/bmi_tcp_client.cc \
	$(DIR)/bmi_tcp_client.ned \
	$(DIR)/bmi_tcp_server.cc \
	$(DIR)/bmi_tcp_server.ned \
	$(DIR)/bmi_transport.cc \
	$(DIR)/bmi_transport.ned \
	$(DIR)/compute_node.ned \
	$(DIR)/daemon_process.ned \
	$(DIR)/io_node.ned \
	$(DIR)/job_process.ned
