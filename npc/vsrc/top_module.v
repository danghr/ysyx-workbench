`define ysyx_24070014_WORD_LEN 32
`define ysyx_24070014_INIT_PC `ysyx_24070014_WORD_LEN'h80000000

module ysyx_24070014_top_module (
  input clk,
  input reset,

  // Signal to access the memory for instruction fetch
  output reg [`ysyx_24070014_WORD_LEN-1:0] pc,
  input [31:0] inst,

  // Signal to access main mamory
  output [`ysyx_24070014_WORD_LEN-1:0] top_signal_mem_addr,
  input [`ysyx_24070014_WORD_LEN-1:0] top_signal_mem_data_read,
  output [`ysyx_24070014_WORD_LEN-1:0] top_signal_mem_data_write,
  output top_signal_mem_write_en,

  // Signal to read the register file
  output [`ysyx_24070014_WORD_LEN-1:0] top_signal_regfile [2**5-1:0],

  // Signal for control signals
  output [15:0] top_signal_control_signal
);

  wire [4:0] inst_rd = inst[11:7];
  wire [4:0] inst_rs1 = inst[19:15];
  wire [4:0] inst_rs2 = inst[24:20];

  // Control signal
  wire [15:0] control_signal;
  wire pc_sel, reg_write_en, branch_unsigned, operand_a_sel, operand_b_sel, mem_write_en;
  wire [1:0] writeback_sel;
  wire [2:0] imm_sel;
  wire [4:0] alu_sel;
  assign top_signal_control_signal = control_signal;
  assign pc_sel = control_signal[0];
  assign imm_sel = control_signal[3:1];
  assign reg_write_en = control_signal[4];
  assign branch_unsigned = control_signal[5];
  assign operand_a_sel = control_signal[6];
  assign operand_b_sel = control_signal[7];
  assign alu_sel = control_signal[12:8];
  assign mem_write_en = control_signal[13];
  assign writeback_sel = control_signal[15:14];
  ysyx_24070014_Decode #(`ysyx_24070014_WORD_LEN) decode (
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
    .writeback_sel(writeback_sel)
  );

  // PC+4
  wire [`ysyx_24070014_WORD_LEN-1:0] pc_plus_4 = pc + 32'h4;
  // ALU output
  wire [`ysyx_24070014_WORD_LEN-1:0] alu_out;

  // Next PC
  wire [31:0] next_pc;
  ysyx_24070014_Mux21 #(`ysyx_24070014_WORD_LEN) mux_pc (
    .sel(pc_sel),
    .in0(pc_plus_4),                      // PC+4
    .in1(alu_out),                        // Branch target
    .out(next_pc)
  );
  always @(posedge clk ) begin
    if (reset) pc <= `ysyx_24070014_INIT_PC;
    else pc <= next_pc;
  end

  // Immediate generator
  wire [`ysyx_24070014_WORD_LEN-1:0] imm;
  ysyx_24070014_ImmGen #(`ysyx_24070014_WORD_LEN) imm_gen (
    .inst(inst),
    .imm_sel(imm_sel),
    .imm(imm)
  );

  // Register file, with 32 registers each of `WORD_LEN` bits
  wire [`ysyx_24070014_WORD_LEN-1:0] reg_data_rs1, reg_data_rs2, reg_data_write;
  ysyx_24070014_RegisterFile #(5, `ysyx_24070014_WORD_LEN) regfile (
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

  // ALU
  wire [`ysyx_24070014_WORD_LEN-1:0] alu_in_1, alu_in_2;
  ysyx_24070014_Mux21 #(`ysyx_24070014_WORD_LEN) mux_alu_in_1 (
    .sel(operand_a_sel),
    .in0(reg_data_rs1),   // Register rs1
    .in1(pc),             // PC
    .out(alu_in_1)
  );
  ysyx_24070014_Mux21 #(`ysyx_24070014_WORD_LEN) mux_alu_in_2 (
    .sel(operand_b_sel),
    .in0(reg_data_rs2),   // Register rs2
    .in1(imm),            // Immediate
    .out(alu_in_2)
  );
  ysyx_24070014_ALU #(`ysyx_24070014_WORD_LEN) alu (
    .in0(alu_in_1),
    .in1(alu_in_2),
    .alu_op(alu_sel),
    .out(alu_out)
  );

  // Memory
  // Currently use top signal to access
  wire [`ysyx_24070014_WORD_LEN-1:0] mem_addr, mem_data_read, mem_data_write;
  assign mem_addr = alu_out;
  assign top_signal_mem_addr = mem_addr;
  assign mem_data_read = top_signal_mem_data_read;
  assign top_signal_mem_data_write = mem_data_write;
  assign top_signal_mem_write_en = mem_write_en;

  // Register write-back
  ysyx_24070014_Mux31 #(`ysyx_24070014_WORD_LEN) mux_writeback (
    .sel(writeback_sel),
    .in0(mem_data_read),                    // Memory
    .in1(alu_out),                          // ALU
    .in2(pc_plus_4),                        // PC+4
    .out(reg_data_write)
  );

endmodule
