// // Shift register
// module top_module (
//     input clk,
//     input reset,
//     input in,
//     input [7:0] init_val,
//     output reg [7:0] out
// );

//     ShiftReg sr (
//         .clk(clk),
//         .reset(reset),
//         .in(in),
//         .init_val(init_val),
//         .out(out)
//     );

// endmodule

// Linear Feedback Shift register
module top_module (
    input clk,
    input reset,
    input [7:0] init_val,
    output reg [7:0] out
);

    LFShiftReg lf (
        .clk(clk),
        .reset(reset),
        .init_val(init_val),
        .out(out)
    );

endmodule

