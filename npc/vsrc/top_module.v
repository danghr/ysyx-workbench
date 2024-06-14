module top_module (
    input clk,
    input reset,
    input ps2_clk,
    input ps2_data,
    output ready,
    output overflow,
    output [7:0] data,
    output [7:0] dots,
    output reg [27:0] show_count,
    output reg [13:0] show_ascii,
    output reg [13:0] show_data,
    output released,
    output pressing,
    output releasing
);

    assign dots = 8'b11101010;

    wire [15:0] int_count;
    reg [7:0] int_ascii, int_data;
    reg en_data, nextdata_n;

    wire [15:0] unused;

    wire count_en = (state == RELEASED && next_state == PRESSING); 
    DecimalCounter decimal_counter_inst (
        .clk(clk),
        .reset(reset),
        .en(count_en),
        .count({unused, int_count})
    );

    Decode47digit decode47digit_inst_11 (
        .x(int_count[15:12]),
        .en(1'b1),
        .out(show_count[27:21])
    );
    Decode47digit decode47digit_inst_12 (
        .x(int_count[11:8]),
        .en(1'b1),
        .out(show_count[20:14])
    );
    Decode47digit decode47digit_inst_1 (
        .x(int_count[7:4]),
        .en(1'b1),
        .out(show_count[13:7])
    );
    Decode47digit decode47digit_inst_2 (
        .x(int_count[3:0]),
        .en(1'b1),
        .out(show_count[6:0])
    );
    Decode47digit decode47digit_inst_3 (
        .x(int_ascii[7:4]),
        .en(en_data),
        .out(show_ascii[13:7])
    );
    Decode47digit decode47digit_inst_4 (
        .x(int_ascii[3:0]),
        .en(en_data),
        .out(show_ascii[6:0])
    );
    Decode47digit decode47digit_inst_5 (
        .x(int_data[7:4]),
        .en(en_data),
        .out(show_data[13:7])
    );
    Decode47digit decode47digit_inst_6 (
        .x(int_data[3:0]),
        .en(en_data),
        .out(show_data[6:0])
    );

    parameter RELEASED = 2'b00, PRESSING = 2'b01, RELEASING = 2'b10;
    reg [1:0] state, next_state;

    always @(*) begin
        case (state)
            RELEASED : next_state = (ready & nextdata_n) ? PRESSING : RELEASED;
            PRESSING : next_state = ((ready & nextdata_n) & (data == 8'hF0)) ? RELEASING : PRESSING;
            RELEASING : next_state = ((ready & nextdata_n) & (data != 8'hF0)) ? RELEASED : RELEASING;
            default : next_state = state;
        endcase
    end

    ps2_keyboard ps2_keyboard_inst (
        .clk(clk),
        .resetn(~reset),
        .ps2_clk(ps2_clk),
        .ps2_data(ps2_data),
        .data(data),
        .ready(ready),
        .nextdata_n(nextdata_n),
        .overflow(overflow)
    );

    always @(posedge clk ) begin
        if (reset) begin
            nextdata_n <= 1'b1;
            int_ascii <= 8'b0;
            int_data <= 8'b0;
            en_data <= 1'b0;
            state <= RELEASED;
        end else begin
            if (ready & nextdata_n) begin
                nextdata_n <= 1'b0;
                int_ascii <= (state == RELEASED && next_state == PRESSING) ? data : int_ascii;
                int_data <= (state == RELEASED && next_state == PRESSING) ? data : int_data;
                $display("[PS/2 Keyboard Controller] Receive %x", data);
            end else begin
                nextdata_n <= 1'b1;
                int_ascii <= int_ascii;
                int_data <= int_data;
                en_data <= en_data;
            end
            state <= next_state;
            en_data <= (next_state == PRESSING);
            if (state != next_state)
                $display("[Top Module] State: %b => %b", state, next_state);
        end
    end

    assign released = (state == RELEASED);
    assign pressing = (state == PRESSING);
    assign releasing = (state == RELEASING);

endmodule
