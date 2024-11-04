`include "vsrc/DEFINITION.v"

import "DPI-C" function int ysyx_24070014_paddr_read(int addr);
import "DPI-C" function void ysyx_24070014_paddr_write(int addr, int data, byte mask);

/**
  * Module `MemoryRW`
  *
  * Template for a read-write memory access module with address width `ADDR_WIDTH` and data width `WORD_LEN`.
  */
module ysyx_24070014_MemoryRW #(ADDR_WIDTH = 32, WORD_LEN = 32) (
    input clk,
    input reset,
    input [ADDR_WIDTH-1:0] addr,
    input [WORD_LEN-1:0] wdata,
    input wen,
    input [3:0] mask,
    output [WORD_LEN-1:0] rdata
);

    // We only pass the arguments to DPI-C functions here
    // Note that we should exclude invalid access here
    // Do nothing on address smaller than ysyx_24070014_MBASE or larger than ysyx_24070014_MBASE + ysyx_24070014_MSIZE

    wire valid_access = (addr >= `ysyx_24070014_MBASE) && (addr < (`ysyx_24070014_MBASE + `ysyx_24070014_MSIZE));
    assign rdata = valid_access ? ysyx_24070014_paddr_read(addr) : {`ysyx_24070014_WORD_LEN{1'b0}};
    always @(posedge clk) begin
        if (reset) begin
            // Do nothing
        end else if (wen && valid_access) begin
            ysyx_24070014_paddr_write(addr, wdata, {4'b0, mask});
        end
    end

endmodule

/**
  * Module `MemoryR`
  *
  * Template for a read-only memory access module with address width `ADDR_WIDTH` and data width `WORD_LEN`.
  */
module ysyx_24070014_MemoryR #(ADDR_WIDTH = 32, WORD_LEN = 32) (
    input [ADDR_WIDTH-1:0] addr,
    output [WORD_LEN-1:0] rdata
);

    // We only pass the arguments to DPI-C functions here
    // Note that we should exclude invalid access here
    // Do nothing on address smaller than ysyx_24070014_MBASE or larger than ysyx_24070014_MBASE + ysyx_24070014_MSIZE

    wire valid_access = (addr >= `ysyx_24070014_MBASE) && (addr < (`ysyx_24070014_MBASE + `ysyx_24070014_MSIZE));
    assign rdata = valid_access ? ysyx_24070014_paddr_read(addr) : {`ysyx_24070014_WORD_LEN{1'b0}};
endmodule
