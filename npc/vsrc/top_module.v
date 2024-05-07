module top_module (
    input clk,
    input reset,   // Synchronous reset
    input s,
    input w,
    output z,
    output reg [1:0] state, next_state, counter
);

    parameter A = 2'd0, B0 = 2'd1, B1 = 2'd2, B2 = 2'd3;
    // reg [1:0] state, next_state;
    // reg [1:0] counter;

    always @(posedge clk ) begin
        case (state)
            A  : next_state = s ? B0 : A;
            B0 : next_state = B1;
            B1 : next_state = B2;
            B2 : next_state = B0;
        endcase
    end

    always @(posedge clk ) begin
        if (reset) state <= A;
        else state <= next_state;
    end

    always @(posedge clk ) begin
        if (state == B0) counter <= w ? 2'd1 : 2'd0;
        else counter <= w ? counter + 2'd1 : counter;
    end

    assign z = (state == B0) & (counter == 2'd2);

endmodule
