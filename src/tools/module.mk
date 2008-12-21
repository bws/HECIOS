#
# Module makefile for tools module
#
DIR := src/tools

TOOLS_SRC += $(DIR)/lanl_trace_parser.cc \
             $(DIR)/lanl_trace_parser_main.cc \
             $(DIR)/lanl_trace_scan_actions.cc \
             $(DIR)/lanl_trace_scanner.l \
             $(DIR)/lanl_trace_scanner_main.cc
