/**
  * Module `SignExtension`
  * 
  * Template for a sign extension module.
  * Extends the input data with width `IN_WIDTH` to the `TARGET_WIDTH` by repeating the sign bit.
  *
  * Ports:
  * - Input `in`: Input data.
  * - Output `out`: Output data.
  */
module ysyx_24070014_SignExtension #(TARGET_WIDTH = 32, IN_WIDTH = 12) (
  input [IN_WIDTH-1:0] in,
  output [TARGET_WIDTH-1:0] out
);

  assign out = {{TARGET_WIDTH-IN_WIDTH{in[IN_WIDTH-1]}}, in};

endmodule