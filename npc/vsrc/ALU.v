/**
  * Module `ALU`
  *
  * Template for an arithmetic logic unit (ALU) for RISC-V instructions.
  * The ALU takes two inputs `in0` and `in1`, performs an operation based on `alu_op`, and outputs the
  * result `out`.
  *
  * Ports:
  * - Input `in0`: First input to the ALU.
  * - Input `in1`: Second input to the ALU.
  * - Input `alu_op`: Operation to perform.
  *   - 4'h0: Add
  * - Output: `out`: Result of the operation.
  */
module ysyx_24070014_ALU #(WORD_LEN = 32) (
  input [WORD_LEN-1:0] in0,
  input [WORD_LEN-1:0] in1,
  input [4:0] alu_op,
  output [WORD_LEN-1:0] out
)
  // Add
  wire [WORD_LEN-1:0] add_out;
  assign add_out = in0 + in1;

  // Subtract

  // Or

  // Xor

  // And

  // Shift left

  // Shift right logical

  // Shift right arithmetic

  // Set less than

  // Set less than unsigned

  // Multiply & Multiply_high

  // Divide & Remainder

  // Divide & Remainder unsigned

  // Output
  ysyx_24070014_MuxKey #(5, WORD_LEN) mux (out, alu_op, WORD_LEN'b0, {
    5'h0, add_out,
  });

endmodule