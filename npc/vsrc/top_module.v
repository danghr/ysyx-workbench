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


// // Linear Feedback Shift register
// module top_module (
//     input clk,
//     input reset,
//     input [7:0] init_val,
//     output reg [7:0] out
// );

//     LFShiftReg lf (
//         .clk(clk),
//         .reset(reset),
//         .init_val(init_val),
//         .out(out)
//     );

// endmodule


// Barrel Shifter
module top_module (
    input [7:0] din,    // Input data
    input [2:0] shamt,  // Shift amount
    input lr,           // Left or right
    input al,           // Arithmetic or logical shift
    output [7:0] out
);

    BarrelShifter bs (
        .din(din),
        .shamt(shamt),
        .lr(lr),
        .al(al),
        .dout(out)
    );

endmodule