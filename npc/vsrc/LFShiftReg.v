module LFShiftReg (
    input clk,
    input reset,
    input [7:0] init_val,
    output reg [7:0] out
);

    wire next_in;

    ShiftReg sr (
        .clk(clk),
        .reset(reset),
        .in(next_in),
        .init_val(init_val),
        .out(out)
    );

    assign next_in = out[4] ^ out[3] ^ out[2] ^ out[0];
    
endmodule