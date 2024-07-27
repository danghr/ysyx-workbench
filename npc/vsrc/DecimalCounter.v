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

    integer i;

    always @(posedge clk) begin
        if (reset) begin
            count <= 32'b0;
            state <= IDLE;
        end else begin
            if (state == IDLE & next_state == COUNTING) begin
                if (count[3:0] == 4'd9) begin
                    if (count[7:4] == 4'd9) begin
                        if (count[11:8] == 4'd9) begin
                            if (count[15:12] == 4'd9) begin
                                if (count[19:16] == 4'd9) begin
                                    if (count[23:20] == 4'd9) begin
                                        if (count[27:24] == 4'd9) begin
                                            if (count[31:28] == 4'd9) begin
                                                count <= 32'b0;
                                            end else begin
                                                count[31:28] <= count[31:28] + 4'd1;
                                                count[27:24] <= 4'd0;
                                            end
                                        end else begin
                                            count[27:24] <= count[27:24] + 4'd1;
                                            count[23:20] <= 4'd0;
                                        end
                                    end else begin
                                        count[23:20] <= count[23:20] + 4'd1;
                                        count[19:16] <= 4'd0;
                                    end
                                end else begin
                                    count[19:16] <= count[19:16] + 4'd1;
                                    count[15:12] <= 4'd0;
                                end
                            end else begin
                                count[15:12] <= count[15:12] + 4'd1;
                                count[11:8] <= 4'd0;
                            end
                        end else begin
                            count[11:8] <= count[11:8] + 4'd1;
                            count[7:4] <= 4'd0;
                        end
                    end else begin
                        count[7:4] <= count[7:4] + 4'd1;
                        count[3:0] <= 4'd0;
                    end         
                end else
                    count[3:0] <= count[3:0] + 4'd1;
            end else count <= count;
            state <= next_state;
        end
    end

endmodule