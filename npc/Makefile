# TOP module name. Affecting TOP_NAME in csrc/main.cpp and other C/C++ source files.
TOPNAME = top_module
# Constraint file
NXDC_FILES = constr/top.nxdc
# Include path for verilator
INC_PATH ?=

# Verilator command and flags
VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc --trace -O3 --x-assign fast --x-initial fast --noassert \

# Output directory
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
TRACE_DIR = $(BUILD_DIR)/t_trace_${TOPNAME}
STAT_DIR = $(BUILD_DIR)/stat
BIN = $(BUILD_DIR)/$(TOPNAME)
$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(TRACE_DIR))

# Constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# Remote server for yosys-sta
YOSYS_STA_SERVER = yosys@console.danghr.com

# Project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

# Rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

# Rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
CXXFLAGS += -DTRACE_NAME="\"\\\"$(TRACE_DIR)/$(TOPNAME).vcd\\\"\""		# TRACE_NAME defined here
CXXFLAGS += -DTOP_NAME_H_FILE="\"\\\"V$(TOPNAME).h\\\"\""				# TOP_NAME_H_FILE defined here

# Target for $(BIN)
$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

all:
	@echo "Write this Makefile by your self."

sim: $(BIN)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@$^

sta: $(BIN)
	rm -rf $(STAT_DIR)
	tar -cvf $(BUILD_DIR)/top.tar -C ./vsrc .
	scp $(BUILD_DIR)/top.tar $(YOSYS_STA_SERVER):~/
	rm $(BUILD_DIR)/top.tar

	ssh $(YOSYS_STA_SERVER) "\
		rm -rf yosys-sta/yosys-result.tar && \
		rm -rf yosys-sta/result && \
		rm -rf yosys-sta/top && \
		mkdir -p yosys-sta/top && \
		cd yosys-sta/top && \
		tar -xvf ~/top.tar && \
		rm -rf ~/top.tar && \
		cd .. && \
		make sta DESIGN=$(TOPNAME) && \
		tar -cvf yosys-result.tar -C ./result ."

	scp $(YOSYS_STA_SERVER):~/yosys-sta/yosys-result.tar $(BUILD_DIR)/yosys-result.tar
	mkdir $(STAT_DIR)
	tar -xvf $(BUILD_DIR)/yosys-result.tar -C $(BUILD_DIR)/stat
	rm $(BUILD_DIR)/yosys-result.tar

clean:
	rm -rf $(BUILD_DIR)

count:
	@echo "Total lines of code:"
	@find . \( -path './csrc/*' -o -path './vsrc/*' -o -path './constr/*' \) | xargs wc -l | grep total | awk '{print $$1}'

include ../Makefile
