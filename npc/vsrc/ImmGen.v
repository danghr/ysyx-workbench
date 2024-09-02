`include "vsrc/DEFINITION.v"

/**
  * Module `ImmGen`
  *
  * Template for an immediate generator for RISC-V instructions.
  * The immediate generator takes an instruction `inst` and outputs the immediate `imm` based on the
  * instruction type.
  *
  * Ports:
  * - Input `inst`: Instruction to generate immediate from.
  * - Input `imm_sel`: Select signal to choose the immediate type.
  *   - 3'b1: I-type
  *   - 3'b2: S-type
  *   - 3'b3: B-type
  *   - 3'b4: J-type
  *   - 3'b5: U-type
  * - Output `imm`: Immediate generated from `inst`.
  */
module ysyx_24070014_ImmGen #(WORD_LEN = `ysyx_24070014_DATA_LEN) (
  input [`ysyx_24070014_INST_LEN-1:0] inst,
  input [2:0] imm_sel,
  output [WORD_LEN-1:0] imm
);
  // I-type
  wire [11:0] imm_I_inst = inst[31:20];
  wire [WORD_LEN-1:0] imm_I;
  ysyx_24070014_SignExtension #(WORD_LEN, 12) sext_i (imm_I_inst, imm_I);

  // S-type
  wire [11:0] imm_S_inst = {inst[31:25], inst[11:7]};
  wire [WORD_LEN-1:0] imm_S;
  ysyx_24070014_SignExtension #(WORD_LEN, 12) sext_s (imm_S_inst, imm_S);

  // B-type
  wire [12:0] imm_B_inst = {inst[31], inst[7], inst[30:25], inst[11:8], 1'b0};
  wire [WORD_LEN-1:0] imm_B;
  ysyx_24070014_SignExtension #(WORD_LEN, 13) sext_b (imm_B_inst, imm_B);

  // U-type
  // In `lui` and `auipc` of RV64I, the 32-bit immediate is sign-extended to 64 bits.
  wire [31:0] imm_U_inst = {inst[31:12], 12'b0};
  wire [WORD_LEN-1:0] imm_U;
  ysyx_24070014_SignExtension #(WORD_LEN, 32) sext_u (imm_U_inst, imm_U);

  // J-type
  wire [20:0] imm_J_inst = {inst[31], inst[19:12], inst[20], inst[30:21], 1'b0};
  wire [WORD_LEN-1:0] imm_J;
  ysyx_24070014_SignExtension #(WORD_LEN, 21) sext_j (imm_J_inst, imm_J);

  // Use a mux to select the correct immediate
  ysyx_24070014_MuxKeyWithDefault #(5, 3, WORD_LEN) mux (imm, imm_sel, 32'b0, {
    // opcode, immediate
    `ysyx_24070014_imm_I, imm_I,
    `ysyx_24070014_imm_S, imm_S,
    `ysyx_24070014_imm_B, imm_B,
    `ysyx_24070014_imm_J, imm_J,  // for `jal`
    `ysyx_24070014_imm_U, imm_U   // for `lui`
  });
endmodule