module ysyx_24070014_Decode #(WORD_LEN = 32) (
  input [31:0] inst,
  input branch_equal,
  input branch_lessthan,
  output pc_sel,
  output [2:0] imm_sel,
  output reg_write_en,
  output branch_unsigned,
  output oprand_a_sel,
  output oprand_b_sel,
  output [4:0] alu_sel,
  output mem_write_en,
  output writeback_sel
);

  // pc_sel
  // TODO: Implement the logic for `pc_sel`
  assign pc_sel = 0;  // 0 for pc+4

  // imm_sel
  integer imm-I = 3'b1;
  integer imm-S = 3'b2;
  integer imm-B = 3'b3;
  integer imm-J = 3'b4;
  integer imm-U = 3'b5;
  assign imm_sel = imm-I;
  // ysyx_24070014_MuxKeyWithDefault #(8, 7, WORD_LEN) mux_imm_sel (inst[6:0], imm_sel, 3'b0, {
  //   7'0110011, 3'b0,  // Register-register arthimetic operations
  //   7'0010011, imm-I, // Register-immediate arthimetic operations
  //   7'0000011, imm-I, // load instructions
  //   7'0100011, imm-S, // store instructions
  //   7'1100011, imm-B, // branch instructions
  //   7'1101111, imm-J, // `jal`
  //   7'1100111, imm-I, // `jalr`
  //   7'0010111, imm-U, // `auipc`
  //   7'0110111, imm-U, // `lui`
  //   7'1110011, 3'b0   // System instructions, e.g., `ecall` and `ebreak`
  // });

  // reg_write_en
  assign reg_write_en = 1'b1;
  // ysyx_24070014_MuxKeyWithDefault #(8, 7, WORD_LEN) mux_reg_write_en_sel (inst[6:0], reg_write_en, 1'b0, {
  //   7'0110011, 1'b1,  // Register-register arthimetic operations
  //   7'0010011, 1'b1,  // Register-immediate arthimetic operations
  //   7'0000011, 1'b1,  // load instructions
  //   7'0100011, 1'b0,  // store instructions
  //   7'1100011, 1'b0,  // branch instructions
  //   7'1101111, 1'b1,  // `jal`
  //   7'1100111, 1'b1,  // `jalr`
  //   7'0010111, 1'b1,  // `auipc`
  //   7'0110111, 1'b1,  // `lui`
  //   7'1110011, 1'b0   // System instructions, e.g., `ecall` and `ebreak`
  // });

  // branch_unsigned
  assign branch_unsigned = 1'b0;
  // TODO: Implement the logic for `branch_unsigned`

  /** Select operands for ALU.
    *  - Note that in store instructions, the operands are `rs1` and `imm`, not `rs2` and `imm`.
    *    These instructions writes the value of `rs2` in the memory address `rs1 + imm`.
    *  - Note that in branch instructions, the operands are `pc` and `imm`, not `rs1` and `rs2`.
    *    This is because that ALU here needs to compute the address of the next instruction, instead of
    *    detecting the branch condition, which should be done by `branch_equal` and `branch_lessthan`. 
    */

  // operand_a_sel
  // Set to 0 for register `rs1`. Set to 1 for `pc`.
  assign operand_a_sel = 1'b0;
  // ysyx_24070014_MuxKeyWithDefault #(8, 7, WORD_LEN) mux_reg_operand_a_sel (inst[6:0], operand_a_sel, 1'b0, {
  //   7'0110011, 1'b0,  // Register-register arthimetic operations
  //   7'0010011, 1'b0,  // Register-immediate arthimetic operations
  //   7'0000011, 1'b0,  // Load instructions
  //   7'0100011, 1'b0,  // Store instructions
  //   7'1100011, 1'b1,  // Branch instructions.
  //   7'1101111, 1'b1,  // `jal`
  //   7'1100111, 1'b0,  // `jalr`
  //   7'0010111, 1'b1,  // `auipc`
  //   7'0110111, 1'b1,  // `lui`
  //   // 7'1110011, 1'b0   // System instructions, e.g., `ecall` and `ebreak`
  // });

  // operand_b_sel
  // Set to 0 for register `rs2`. Set to 1 for immediate.
  assign operand_b_sel = 1'b1;
  // ysyx_24070014_MuxKeyWithDefault #(8, 7, WORD_LEN) mux_reg_operand_a_sel (inst[6:0], operand_a_sel, 1'b0, {
  //   7'0110011, 1'b0,  // Register-register arthimetic operations
  //   7'0010011, 1'b1,  // Register-immediate arthimetic operations
  //   7'0000011, 1'b1,  // Load instructions
  //   7'0100011, 1'b1,  // Store instructions.
  //   7'1100011, 1'b1,  // Branch instructions.
  //   7'1101111, 1'b1,  // `jal`
  //   7'1100111, 1'b1,  // `jalr`
  //   7'0010111, 1'b1,  // `auipc`
  //   7'0110111, 1'b1,  // `lui`
  //   // 7'1110011, 1'b0   // System instructions, e.g., `ecall` and `ebreak`
  // });

  // alu_sel
  assign alu_sel = 5'b00000;  // 0 for add

  // mem_write_en
  assign mem_write_en = 1'b0;
  // Only enable for store instructions
  // ysyx_24070014_MuxKeyWithDefault #(8, 7, WORD_LEN) mux_mem_write_en (inst[6:0], mem_write_en, 1'b0, {
  //   7'0110011, 1'b0,  // Register-register arthimetic operations
  //   7'0010011, 1'b0,  // Register-immediate arthimetic operations
  //   7'0000011, 1'b0,  // Load instructions
  //   7'0100011, 1'b1,  // Store instructions
  //   7'1100011, 1'b0,  // Branch instructions.
  //   7'1101111, 1'b0,  // `jal`
  //   7'1100111, 1'b0,  // `jalr`
  //   7'0010111, 1'b0,  // `auipc`
  //   7'0110111, 1'b0,  // `lui`
  //   7'1110011, 1'b0   // System instructions, e.g., `ecall` and `ebreak`
  // });

  // writeback_sel
  // Set to 0 for `mem`, 1 for `alu_out`, 2 for `pc + 4`.
  assign writeback_sel = 2'b1;
  // ysyx_24070014_MuxKeyWithDefault #(8, 7, WORD_LEN) mux_writeback_sel (inst[6:0], writeback_sel, 2'b0, {
  //   7'0110011, 2'b1,  // Register-register arthimetic operations
  //   7'0010011, 2'b1,  // Register-immediate arthimetic operations
  //   7'0000011, 2'b0,  // Load instructions
  //   7'0100011, 2'b0,  // Store instructions
  //   7'1100011, 2'b0,  // Branch instructions.
  //   7'1101111, 2'b1,  // `jal`
  //   7'1100111, 2'b1,  // `jalr`
  //   7'0010111, 2'b1,  // `auipc`
  //   7'0110111, 2'b1,  // `lui`
  //   7'1110011, 2'b0   // System instructions, e.g., `ecall` and `ebreak`
  // });
    
endmodule