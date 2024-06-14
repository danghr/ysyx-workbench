module DecimalCounter (
    input clk,
    input reset,
    input en,
    output reg [7:0] count
);

    reg state, next_state;
    parameter IDLE = 1'b0, COUNTING = 1'b1;

    always @(*) begin
        case (state)
            IDLE : next_state = en ? COUNTING : IDLE;
            COUNTING : next_state = en ? COUNTING : IDLE;
            default : next_state = state;
        endcase
    end

    always @(posedge clk) begin
        if (reset) begin
            count <= 8'b0;
            state <= IDLE;
        end else begin
            if (state == IDLE & next_state == COUNTING) begin
                count <= count + 8'h1;
                if (count[3:0] == 4'h9) begin
                    count[3:0] <= 4'b0;
                    if (count[7:4] == 4'h9) begin
                        count[7:4] <= 4'b0;
                    end else count[7:4] <= count[7:4] + 4'b1;
                end
            end else count <= count;
            state <= next_state;
        end
    end

endmodule