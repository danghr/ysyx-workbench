all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	# @echo "Write this Makefile by your self."
	verilator --cc --exe --trace --build -j 0 -Wall csrc/main.cpp vsrc/top.v
	mkdir -p obj_dir/t_trace_top
	./obj_dir/Vtop

include ../Makefile
