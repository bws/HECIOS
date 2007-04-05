#
# Module makefile for physical module
#
DIR := src/physical

SIM_SRC += $(DIR)/bmi_tcp_client.cc \
	$(DIR)/bmi_tcp_server.cc \
	$(DIR)/bmi_transport.cc \
	$(DIR)/hard_disk.cc
