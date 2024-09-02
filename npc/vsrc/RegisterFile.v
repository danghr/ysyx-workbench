/**
  * Module `RegisterFile`
  *
  * Template for a register file with address width `ADDR_WIDTH` and data width `WORD_LEN`.
  * The register file has `2**ADDR_WIDTH` registers, each with `WORD_LEN` bits.
  *
  * Ports:
  * - Input `clk`: Clock signal.
  * - Input `reset`: Reset signal.
  * - Input `raddr1`: Read address 1. The register at this address is read and output as `rdata1`.
  * - Input `raddr2`: Read address 2. The register at this address is read and output as `rdata2`.
  * - Input `wdata`: Data to be written to the register file.
  * - Input `waddr`: Address to write `wdata` to.
  * - Input `wen`: Write enable signal. If high, write `wdata` to `waddr`.
  * - Output `rdata1`: Data read from the register at address `raddr1`.
  * - Output `rdata2`: Data read from the register at address `raddr2`.
  */
module ysyx_24070014_RegisterFile #(ADDR_WIDTH = 5, WORD_LEN = 32) (
  input clk,
  input reset,
  input [ADDR_WIDTH-1:0] raddr1,
  input [ADDR_WIDTH-1:0] raddr2,
  input [WORD_LEN-1:0] wdata,
  input [ADDR_WIDTH-1:0] waddr,
  input wen,
  output [WORD_LEN-1:0] rdata1,
  output [WORD_LEN-1:0] rdata2,

  // To inspect registers
  output [WORD_LEN-1:0] signal_rf [2**ADDR_WIDTH-1:0]
);
  
  // Register files
  reg [WORD_LEN-1:0] rf [2**ADDR_WIDTH-1:0];

  // Reset
  always @(posedge clk) begin
    if (reset) begin
      for (int i = 0; i < 2**ADDR_WIDTH; i = i + 1) begin : rf_reset
        rf[i] <= 0;
      end
    end
  end

  // Write on posedge
  always @(posedge clk) begin
    if (wen) rf[waddr] <= wdata;
    rf[0] = 0; // x0 is always 0
  end

  // Read
  assign rdata1 = rf[raddr1];
  assign rdata2 = rf[raddr2];

  // To inspect registers
  assign signal_rf = rf;
endmodule