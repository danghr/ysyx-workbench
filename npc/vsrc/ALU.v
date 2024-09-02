`include "vsrc/DEFINITION.v"

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
module ysyx_24070014_ALU (
  input [`ysyx_24070014_DATA_LEN-1:0] in0,
  input [`ysyx_24070014_DATA_LEN-1:0] in1,
  input [4:0] alu_op,
  output [`ysyx_24070014_DATA_LEN-1:0] out
);
  // Add
  wire [`ysyx_24070014_DATA_LEN-1:0] add_out;
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
  ysyx_24070014_MuxKeyWithDefault #(1, 5, `ysyx_24070014_DATA_LEN) mux (out, alu_op, 32'b0, {
    5'h0, add_out
  });

endmodule