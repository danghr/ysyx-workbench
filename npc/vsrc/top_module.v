module top_module (
    input clk,
    input reset,
    input ps2_clk,
    input ps2_data,
    output ready,
    output overflow,
    output reg [7:0] data
);

    reg nextdata_n;
    wire [7:0] ps2ctrl_data;

    ps2_keyboard ps2_keyboard_inst (
        .clk(clk),
        .resetn(~reset),
        .ps2_clk(ps2_clk),
        .ps2_data(ps2_data),
        .data(ps2ctrl_data),
        .ready(ready),
        .nextdata_n(nextdata_n),
        .overflow(overflow)
    );

    always @(posedge clk ) begin
        if (reset) begin
            data <= 8'b0;
            nextdata_n <= 1'b1;
        end else if (ready & ~nextdata_n) begin
            data <= ps2ctrl_data;
            nextdata_n <= 1'b0;
            $display("[PS/2 Keyboard Controller] Receive %x", data);
        end else begin
            data <= data;
            nextdata_n <= 1'b1;
        end
    end

endmodule