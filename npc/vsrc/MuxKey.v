module ysyx_24070014_MuxKeyInternal #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1, HAS_DEFAULT = 0) (
  output reg [DATA_LEN-1:0] out,
  input [KEY_LEN-1:0] key,
  input [DATA_LEN-1:0] default_out,
  input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
);

  localparam PAIR_LEN = KEY_LEN + DATA_LEN;
  wire [PAIR_LEN-1:0] pair_list [NR_KEY-1:0];
  wire [KEY_LEN-1:0] key_list [NR_KEY-1:0];
  wire [DATA_LEN-1:0] data_list [NR_KEY-1:0];

  genvar n;
  generate
    for (n = 0; n < NR_KEY; n = n + 1) begin
      assign pair_list[n] = lut[PAIR_LEN*(n+1)-1 : PAIR_LEN*n];
      assign data_list[n] = pair_list[n][DATA_LEN-1:0];
      assign key_list[n]  = pair_list[n][PAIR_LEN-1:DATA_LEN];
    end
  endgenerate

  reg [DATA_LEN-1 : 0] lut_out;
  reg hit;
  integer i;
  always @(*) begin
    lut_out = 0;
    hit = 0;
    for (i = 0; i < NR_KEY; i = i + 1) begin
      lut_out = lut_out | ({DATA_LEN{key == key_list[i]}} & data_list[i]);
      hit = hit | (key == key_list[i]);
    end
    if (!HAS_DEFAULT) out = lut_out;
    else out = (hit ? lut_out : default_out);
  end

endmodule


/**
  * Module `MuxKey`
  *
  * Template for a multiplexer with `NR_KEY` inputs, each with `KEY_LEN` bits of key and `DATA_LEN` bits of data.
  * The multiplexer selects the input with the key matching the input key.
  *
  * Ports:
  * - Output `out`: Output data.
  * - Input `key`: Key to select the input.
  * - Input `lut`: Lookup table. Each input is a consecutive pair of key and data.
  *
  * Example: A 4-to-1 multiplexer with 2-bit key `s` and four 1-bit data `a`.
  * ```verilog
  * module mux41(
  *   input  [3:0] a,
  *   input  [1:0] s,
  *   output y
  * );
  *   ysyx_24070014_MuxKey #(4, 2, 1) i0 (y, s, {
  *     2'b00, a[0],
  *     2'b01, a[1],
  *     2'b10, a[2],
  *     2'b11, a[3]
  *   });
  * endmodule
  * ```
  */
module ysyx_24070014_MuxKey #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1) (
  output [DATA_LEN-1:0] out,                    // Output
  input [KEY_LEN-1:0] key,                      // Key
  input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut   // Input
);
  ysyx_24070014_MuxKeyInternal #(NR_KEY, KEY_LEN, DATA_LEN, 0) i0 (out, key, {DATA_LEN{1'b0}}, lut);
endmodule


/**
  * Module `MuxKeyWithDefault`
  *
  * Template for a multiplexer with `NR_KEY` inputs, each with `KEY_LEN` bits of key and `DATA_LEN` bits of data.
  * The multiplexer selects the input with the key matching the input key.
  *
  * Ports:
  * - Output `out`: Output data.
  * - Input `key`: Key to select the input.
  * - Input `default_out`: Default output data if no key matches.
  * - Input `lut`: Lookup table. Each input is a consecutive pair of key and data.
  *
  * Example: A 4-to-1 multiplexer with 2-bit key `s` and four 1-bit data `a`. If no key matches, output `1'b0`.
  * ```verilog
  * module mux41(
  *   input  [3:0] a,
  *   input  [1:0] s,
  *   output y
  * );
  *   ysyx_24070014_MuxKeyWithDefault #(4, 2, 1) i0 (y, s, 1'b0 {
  *     2'b00, a[0],
  *     2'b01, a[1],
  *     2'b10, a[2],
  *     2'b11, a[3]
  *   });
  * endmodule
  * ```
  */
module ysyx_24070014_MuxKeyWithDefault #(NR_KEY = 2, KEY_LEN = 1, DATA_LEN = 1) (
  output [DATA_LEN-1:0] out,                    // Output
  input [KEY_LEN-1:0] key,                      // Key
  input [DATA_LEN-1:0] default_out,             // Default
  input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut   // Input
);
  ysyx_24070014_MuxKeyInternal #(NR_KEY, KEY_LEN, DATA_LEN, 1) i0 (out, key, default_out, lut);
endmodule

/**
  * Module `Mux21`
  *
  * Template for a 2-to-1 multiplexer with data width `DATA_LEN`.
  *
  * Ports:
  * - Input `sel`: Select signal. If high, output `in1`; otherwise, output `in0`.
  * - Input `in0`: Input 0.
  * - Input `in1`: Input 1.
  * - Output `out`: Output data.
  */
module ysyx_24070014_Mux21 #(DATA_LEN = 1) (
  input sel,                 // Select signal
  input [DATA_LEN-1:0] in0,  // Input 0
  input [DATA_LEN-1:0] in1,  // Input 1
  output [DATA_LEN-1:0] out  // Output
);
  ysyx_24070014_MuxKeyWithDefault #(2, 1, DATA_LEN) i0 (out, sel, 32'b0, {
    1'b0, in0,
    1'b1, in1
  });
endmodule

/**
  * Module `Mux31`
  *
  * Template for a 3-to-1 multiplexer with data width `DATA_LEN`.
  *
  * Ports:
  * - Input `sel`: Select signal. Select `in0`, `in1`, or `in2` input based on the value of `sel`.
  * - Input `in0`: Input 0.
  * - Input `in1`: Input 1.
  * - Input `in2`: Input 2.
  * - Output `out`: Output data.
  */
module ysyx_24070014_Mux31 #(DATA_LEN = 1) (
  input [1:0] sel,            // Select signal
  input [DATA_LEN-1:0] in0,   // Input 0
  input [DATA_LEN-1:0] in1,   // Input 1
  input [DATA_LEN-1:0] in2,   // Input 2
  output [DATA_LEN-1:0] out   // Output
);
  ysyx_24070014_MuxKeyWithDefault #(3, 2, DATA_LEN) i0 (out, sel, 32'b0, {
    2'b00, in0,
    2'b01, in1,
    2'b10, in2
  });
endmodule

/**
  * Module `Mux41`
  *
  * Template for a 4-to-1 multiplexer with data width `DATA_LEN`.
  *
  * Ports:
  * - Input `sel`: Select signal. Select `in0`, `in1`, `in2`, or `in3` input based on the value of `sel`.
  * - Input `in0`: Input 0.
  * - Input `in1`: Input 1.
  * - Input `in2`: Input 2.
  * - Input `in3`: Input 3.
  * - Output `out`: Output data.
  */
module ysyx_24070014_Mux41 #(DATA_LEN = 1) (
  input [1:0] sel,            // Select signal
  input [DATA_LEN-1:0] in0,   // Input 0
  input [DATA_LEN-1:0] in1,   // Input 1
  input [DATA_LEN-1:0] in2,   // Input 2
  input [DATA_LEN-1:0] in3,   // Input 3
  output [DATA_LEN-1:0] out   // Output
);
  ysyx_24070014_MuxKeyWithDefault #(4, 2, DATA_LEN) i0 (out, sel, 32'b0, {
    2'b00, in0,
    2'b01, in1,
    2'b10, in2,
    2'b11, in3
  });
endmodule