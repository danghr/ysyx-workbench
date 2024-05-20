module top_module (
    input clk,
    input reset,
    input  [1:0] X0,
    input  [1:0] X1,
    input  [1:0] X2,
    input  [1:0] X3,
    input  [1:0] Y,     // Control of the mux
    output [1:0] F      // Output
);

    MuxKey #(4 /* NR_KEY */, 2 /* KEY_LEN */, 2 /* DATA_LEN */) i0 (
        .out(F),
        .key(Y),
        .lut({{2'b00, X0}, {2'b01, X1}, {2'b10, X2}, {2'b11, X3}})
    );

endmodule
