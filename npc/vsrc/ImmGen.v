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
module ysyx_24070014_ImmGen #(WORD_LEN = 32) (
  input [31:0] inst,
  input [2:0] imm_sel,
  output [WORD_LEN-1:0] imm
);
  // I-type
  wire [11:0] imm-I-inst = inst[31:20];
  wire [WORD_LEN-1:0] imm-I;
  ysyx_24070014_SignExtension #(WORD_LEN, 12) sext_i (imm-I, imm-I-inst);

  // S-type
  wire [11:0] imm-S-inst = {inst[31:25], inst[11:7]};
  wire [WORD_LEN-1:0] imm-S;
  ysyx_24070014_SignExtension #(WORD_LEN, 12) sext_s (imm-S, imm-S-inst);

  // B-type
  wire [12:0] imm-B-inst = {inst[31], inst[7], inst[30:25], inst[11:8], 1'b0};
  wire [WORD_LEN-1:0] imm-B;
  ysyx_24070014_SignExtension #(WORD_LEN, 13) sext_b (imm-B, imm-B-inst);

  // U-type
  // In `lui` and `auipc` of RV64I, the 32-bit immediate is sign-extended to 64 bits.
  wire [31:0] imm-U-inst = {inst[31:12], 12'b0};
  wire [WORD_LEN-1:0] imm-U;
  ysyx_24070014_SignExtension #(WORD_LEN, 32) sext_u (imm-U, imm-U-inst);

  // J-type
  wire [20:0] imm-J-inst = {inst[31], inst[19:12], inst[20], inst[30:21], 1'b0};
  wire [WORD_LEN-1:0] imm-J;
  ysyx_24070014_SignExtension #(WORD_LEN, 21) sext_j (imm-J, imm-J-inst);

  // Use a mux to select the correct immediate
  ysyx_24070014_MuxKeyWithDefault #(5, 3, WORD_LEN) mux (imm, imm_sel, WORD_LEN'b0, {
    // opcode, immediate
    3'b1, imm-I,
    3'b2, imm-S,
    3'b3, imm-B,
    3'b4, imm-J,  // for `jal`
    3'b5, imm-U   // for `lui`
  });
endmodule