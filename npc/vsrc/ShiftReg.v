module ShiftReg (
    input clk,
    input reset,
    input in,
    input [7:0] init_val,
    output reg [7:0] out
);

    always @(posedge clk ) begin
        if (reset) out <= init_val;
        else out <= {in, out[7:1]};
    end

endmodule