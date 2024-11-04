`include "vsrc/DEFINITION.v"

// DPI-C functions
import "DPI-C" function void ysyx_24070014_ecall();
import "DPI-C" function void ysyx_24070014_ebreak();

module ysyx_24070014_top_module (
  input clk,
  input reset,

  // Signal to access regfile
  output [`ysyx_24070014_DATA_LEN-1:0] top_signal_regfile[2**`ysyx_24070014_REG_ADDR_WIDTH-1:0]
);

  /******************************
   * STATE 1: Instruction Fetch *
   ******************************/

  reg [`ysyx_24070014_ADDR_LEN-1:0] pc;
  wire [`ysyx_24070014_INST_LEN-1:0] inst;
  ysyx_24070014_MemoryR #(`ysyx_24070014_ADDR_LEN, `ysyx_24070014_INST_LEN) inst_mem (
    .addr(pc),
    .rdata(inst)
  );

  // PC+4
  wire [`ysyx_24070014_ADDR_LEN-1:0] pc_plus_4 = pc + 32'h4;
  // ALU output
  wire [`ysyx_24070014_DATA_LEN-1:0] alu_out;

  // Next PC
  wire [31:0] next_pc;
  ysyx_24070014_Mux21 #(`ysyx_24070014_INST_LEN) mux_pc (
    .sel(pc_sel),
    .in0(pc_plus_4),                      // PC+4
    .in1(alu_out),                        // Branch target
    .out(next_pc)
  );
  always @(posedge clk ) begin
    if (reset) pc <= `ysyx_24070014_INIT_PC;
    else pc <= next_pc;
  end


  /***********************************************
   * STATE 2: Instruction Decode & Register Read *
   ***********************************************/
  
  // Control signal
  wire pc_sel, reg_write_en, branch_unsigned, operand_a_sel, operand_b_sel, mem_write_en;
  wire [1:0] writeback_sel;
  wire [2:0] imm_sel;
  wire [3:0] mem_write_mask;
  wire [4:0] alu_sel;
  wire ecall, ebreak;
  ysyx_24070014_Decode decoder (
    .inst(inst),
    .branch_equal(0),
    .branch_lessthan(0),
    .pc_sel(pc_sel),
    .imm_sel(imm_sel),
    .reg_write_en(reg_write_en),
    .branch_unsigned(branch_unsigned),
    .operand_a_sel(operand_a_sel),
    .operand_b_sel(operand_b_sel),
    .alu_sel(alu_sel),
    .mem_write_en(mem_write_en),
    .mem_write_mask(mem_write_mask),
    .writeback_sel(writeback_sel),
    .ecall(ecall),
    .ebreak(ebreak)
  );

  // Register operands
  wire [4:0] inst_rd = inst[11:7];
  wire [4:0] inst_rs1 = inst[19:15];
  wire [4:0] inst_rs2 = inst[24:20];

  // Register file, with 32 registers each of `DATA_LEN` bits
  wire [`ysyx_24070014_DATA_LEN-1:0] reg_data_rs1, reg_data_rs2, reg_data_write;
  ysyx_24070014_RegisterFile #(`ysyx_24070014_REG_ADDR_WIDTH, `ysyx_24070014_DATA_LEN) regfile (
    .clk(clk),
    .reset(reset),
    .raddr1(inst_rs1),
    .raddr2(inst_rs2),
    .wdata(reg_data_write),
    .waddr(inst_rd),
    .wen(reg_write_en),
    .rdata1(reg_data_rs1),
    .rdata2(reg_data_rs2),
    .signal_rf(top_signal_regfile)
  );

  // Immediate generation
  wire [`ysyx_24070014_DATA_LEN-1:0] imm;
  ysyx_24070014_ImmGen #(`ysyx_24070014_DATA_LEN) imm_gen (
    .inst(inst),
    .imm_sel(imm_sel),
    .imm(imm)
  );


  /********************
   * STATE 3: Execute *
   ********************/
  
  // ALU
  wire [`ysyx_24070014_DATA_LEN-1:0] alu_in_1, alu_in_2;
  ysyx_24070014_Mux21 #(`ysyx_24070014_DATA_LEN) mux_alu_in_1 (
    .sel(operand_a_sel),
    .in0(reg_data_rs1),   // Register rs1
    .in1(pc),             // PC
    .out(alu_in_1)
  );
  ysyx_24070014_Mux21 #(`ysyx_24070014_DATA_LEN) mux_alu_in_2 (
    .sel(operand_b_sel),
    .in0(reg_data_rs2),   // Register rs2
    .in1(imm),            // Immediate
    .out(alu_in_2)
  );
  ysyx_24070014_ALU alu (
    .in0(alu_in_1),
    .in1(alu_in_2),
    .alu_op(alu_sel),
    .out(alu_out)
  );


  /*******************
   * STATE 4: Memory *
   *******************/
  
  // Memory
  // Currently use top signal to access
  wire [`ysyx_24070014_ADDR_LEN-1:0] mem_addr;
  wire [`ysyx_24070014_DATA_LEN-1:0] mem_data_read, mem_data_write;
  assign mem_addr = alu_out;
  assign mem_data_write = reg_data_rs2;
  ysyx_24070014_MemoryRW #(`ysyx_24070014_ADDR_LEN, `ysyx_24070014_DATA_LEN) mem (
    .clk(clk),
    .reset(reset),
    .addr(mem_addr),
    .wdata(mem_data_write),
    .wen(mem_write_en),
    .mask(mem_write_mask),
    .rdata(mem_data_read)
  );


  /**********************
   * STATE 5: Writeback *
   **********************/
  
  // Register write-back
  ysyx_24070014_Mux31 #(`ysyx_24070014_DATA_LEN) mux_writeback (
    .sel(writeback_sel),
    .in0(mem_data_read),                    // Memory
    .in1(alu_out),                          // ALU
    .in2(pc_plus_4),                        // PC+4
    .out(reg_data_write)
  );

  // Ecall and ebreak
  always @(posedge clk ) begin
    if (ecall) ysyx_24070014_ecall();
    if (ebreak) ysyx_24070014_ebreak();
  end

endmodule
