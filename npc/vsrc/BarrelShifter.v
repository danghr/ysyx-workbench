module Mux41 (
    input [3:0] in,
    input [1:0] sel,
    output out
);
    MuxKey #(4, 2, 1) mux(
        .out(out),
        .key(sel),
        .lut({
            {2'b00, in[0]},
            {2'b01, in[1]},
            {2'b10, in[2]},
            {2'b11, in[3]}
        })
    );
endmodule

module Mux21 (
    input [1:0] in,
    input sel,
    output out
);
    MuxKey #(2, 1, 1) mux(
        .out(out),
        .key(sel),
        .lut({
            {1'b0, in[0]},
            {1'b1, in[1]}
        })
    );
endmodule

module BarrelShifter (
    input [7:0] din,    // Input data
    input [2:0] shamt,  // Shift amount
    input lr,           // Left or right
    input al,           // Arithmetic or logical shift
    output [7:0] dout
);

    wire [7:0] stage1, stage2;

    wire leftmost_bit;
    Mux21 leftmost_bit_mux(
        .in({din[7], 1'b0}),
        .sel(al),
        .out(leftmost_bit)
    );

    wire rightmost_bit;
    assign rightmost_bit = 1'b0;

    // Stage 1: Move 1 bit
    Mux41 mux17(
        .in({din[6], din[7], leftmost_bit, din[7]}),
        .sel({lr, shamt[0]}),
        .out(stage1[7])
    );
    Mux41 mux16(
        .in({din[5], din[6], din[7], din[6]}),
        .sel({lr, shamt[0]}),
        .out(stage1[6])
    );
    Mux41 mux15(
        .in({din[4], din[5], din[6], din[5]}),
        .sel({lr, shamt[0]}),
        .out(stage1[5])
    );
    Mux41 mux14(
        .in({din[3], din[4], din[5], din[4]}),
        .sel({lr, shamt[0]}),
        .out(stage1[4])
    );
    Mux41 mux13(
        .in({din[2], din[3], din[4], din[3]}),
        .sel({lr, shamt[0]}),
        .out(stage1[3])
    );
    Mux41 mux12(
        .in({din[1], din[2], din[3], din[2]}),
        .sel({lr, shamt[0]}),
        .out(stage1[2])
    );
    Mux41 mux11(
        .in({din[0], din[1], din[2], din[1]}),
        .sel({lr, shamt[0]}),
        .out(stage1[1])
    );
    Mux41 mux10(
        .in({rightmost_bit, din[0], din[1], din[0]}),
        .sel({lr, shamt[0]}),
        .out(stage1[0])
    );

    // Stage 2: Move 2 bit
    Mux41 mux27(
        .in({stage1[5], stage1[7], leftmost_bit, stage1[7]}),
        .sel({lr, shamt[1]}),
        .out(stage2[7])
    );
    Mux41 mux26(
        .in({stage1[4], stage1[6], leftmost_bit, stage1[6]}),
        .sel({lr, shamt[1]}),
        .out(stage2[6])
    );
    Mux41 mux25(
        .in({stage1[3], stage1[5], stage1[7], stage1[5]}),
        .sel({lr, shamt[1]}),
        .out(stage2[5])
    );
    Mux41 mux24(
        .in({stage1[2], stage1[4], stage1[6], stage1[4]}),
        .sel({lr, shamt[1]}),
        .out(stage2[4])
    );
    Mux41 mux23(
        .in({stage1[1], stage1[3], stage1[5], stage1[3]}),
        .sel({lr, shamt[1]}),
        .out(stage2[3])
    );
    Mux41 mux22(
        .in({stage1[0], stage1[2], stage1[4], stage1[2]}),
        .sel({lr, shamt[1]}),
        .out(stage2[2])
    );
    Mux41 mux21(
        .in({rightmost_bit, stage1[1], stage1[3], stage1[1]}),
        .sel({lr, shamt[1]}),
        .out(stage2[1])
    );
    Mux41 mux20(
        .in({rightmost_bit, stage1[0], stage1[2], stage1[0]}),
        .sel({lr, shamt[1]}),
        .out(stage2[0])
    );
    
    // Stage 3: Move 4 bits
    Mux41 mux37(
        .in({stage2[3], stage2[7], leftmost_bit, stage2[7]}),
        .sel({lr, shamt[2]}),
        .out(dout[7])
    );
    Mux41 mux36(
        .in({stage2[2], stage2[6], leftmost_bit, stage2[6]}),
        .sel({lr, shamt[2]}),
        .out(dout[6])
    );
    Mux41 mux35(
        .in({stage2[1], stage2[5], leftmost_bit, stage2[5]}),
        .sel({lr, shamt[2]}),
        .out(dout[5])
    );
    Mux41 mux34(
        .in({stage2[0], stage2[4], leftmost_bit, stage2[4]}),
        .sel({lr, shamt[2]}),
        .out(dout[4])
    );
    Mux41 mux33(
        .in({rightmost_bit, stage2[3], stage2[7], stage2[3]}),
        .sel({lr, shamt[2]}),
        .out(dout[3])
    );
    Mux41 mux32(
        .in({rightmost_bit, stage2[2], stage2[6], stage2[2]}),
        .sel({lr, shamt[2]}),
        .out(dout[2])
    );
    Mux41 mux31(
        .in({rightmost_bit, stage2[1], stage2[5], stage2[1]}),
        .sel({lr, shamt[2]}),
        .out(dout[1])
    );
    Mux41 mux30(
        .in({rightmost_bit, stage2[0], stage2[4], stage2[0]}),
        .sel({lr, shamt[2]}),
        .out(dout[0])
    );
    
endmodule