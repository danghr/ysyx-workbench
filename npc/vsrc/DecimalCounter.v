module DecimalCounter (
    input clk,
    input reset,
    input en,
    output reg [31:0] count
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
            count <= 32'b0;
            state <= IDLE;
        end else begin
            if (state == IDLE & next_state == COUNTING) begin
                integer i;
                for (i = 0; i < 8; i = i + 1) begin
                    if (count[i*4 +: 4] == 4'h9) begin
                        count[i*4 +: 4] <= 4'b0;
                        if (i != 7) begin
                            count[(i+1)*4 +: 4] <= count[(i+1)*4 +: 4] + 4'b1;
                        end
                    end else begin
                        count[i*4 +: 4] <= count[i*4 +: 4] + 4'b1;
                        break;
                    end
                end
            end else count <= count;
            state <= next_state;
        end
    end

endmodule