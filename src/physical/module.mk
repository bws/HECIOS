#
# Module makefile for physical module
#
DIR := src/physical

SIM_SRC += $(DIR)/bmi_endpoint.cc \
	$(DIR)/bmi_direct_endpoint.cc \
	$(DIR)/bmi_tcp_client.cc \
	$(DIR)/bmi_tcp_server.cc \
	$(DIR)/enhanced_ether_encap.cc \
	$(DIR)/enhanced_ether_mac_base.cc \
	$(DIR)/enhanced_ether_mac2.cc \
	$(DIR)/enhanced_mac_relay_unit_pp.cc \
	$(DIR)/hard_disk.cc \
	$(DIR)/mpi_tcp_client.cc \
	$(DIR)/mpi_tcp_server.cc
