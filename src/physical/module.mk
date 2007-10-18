#
# Module makefile for physical module
#
DIR := src/physical

SIM_SRC += $(DIR)/bmi_endpoint.cc \
	$(DIR)/bmi_direct_endpoint.cc \
	$(DIR)/bmi_tcp_client.cc \
	$(DIR)/bmi_tcp_endpoint.cc \
	$(DIR)/bmi_tcp_server.cc \
	$(DIR)/hard_disk.cc \
	$(DIR)/mpi_tcp_client.cc \
	$(DIR)/mpi_tcp_server.cc
