module top_module (
    input [3:0] a,
    input [3:0] b,
    input [2:0] sel,
    output [3:0] y,
    output zero
);

    // Add
    wire [3:0] add_out;
    wire add_overflow, add_zero;
    Adder adder (a, b, 1'b1, add_out, add_overflow, add_zero);

    // Subtract
    wire [3:0] sub_out;
    wire sub_overflow, sub_zero;
    Adder subber (a, b, 1'b1, sub_out, sub_overflow, sub_zero);

    // Reverse
    wire [3:0] rev_out;
    assign rev_out = ~a;

    // And
    wire [3:0] and_out;
    assign and_out = a & b;

    // Or
    wire [3:0] or_out;
    assign or_out = a | b;

    // Xor
    wire [3:0] xor_out;
    assign xor_out = a ^ b;

    // Compare
    wire [3:0] cmp_out;
    assign cmp_out = {3'b000, sub_out[3] ^ sub_overflow}; 

    // Equal
    wire [3:0] eq_out;
    assign eq_out = {3'b000, ~sub_zero};

    MuxKey #(8, 3, 4) mux (y, sel, {
        {3'b000, add_out}, 
        {3'b001, sub_out}, 
        {3'b010, rev_out}, 
        {3'b011, and_out}, 
        {3'b100, or_out}, 
        {3'b101, xor_out}, 
        {3'b110, cmp_out}, 
        {3'b111, eq_out}
    });
    assign zero = ~(|y);

endmodule

module Adder (
    input [3:0] a,
    input [3:0] b,
    input rev,
    output [3:0] y,
    output overflow,
    output zero
);

    wire [3:0] b_rev;
    assign b_rev = (b ^ {4{rev}}) + {3'b000, rev};
    assign {overflow, y} = a + b_rev;
    assign zero = ~(|y);
    
endmodule
