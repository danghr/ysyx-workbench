module top_module (
    input clk,
    input reset,
    input ps2_clk,
    input ps2_data,
    output ready,
    output overflow,
    output reg [7:0] data,
    output reg [13:0] show_count,
    output reg [13:0] show_ascii,
    output reg [13:0] show_data
);

    keyboard_inst keyboard_inst_inst (
        .clk(clk),
        .reset(reset),
        .ps2_clk(ps2_clk),
        .ps2_data(ps2_data),
        .ready(ready),
        .overflow(overflow),
        .data(data)
    );

    reg [7:0] int_count, int_ascii, int_data;
    reg en_data;

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

    parameter RELEASED = 1'b0, PRESSING = 1'b1;
    reg state, next_state;

    always @(*) begin
        case (state)
            RELEASED : next_state = ready ? PRESSING : RELEASED;
            PRESSING : next_state = (ready & (data == 8'hF0)) ? RELEASED : PRESSING;
            default : next_state = RELEASED;
        endcase
    end

    always @(posedge clk ) begin
        if (reset) begin
            int_count <= 8'b0;
            int_ascii <= 8'b0;
            int_data <= 8'b0;
            en_data <= 1'b0;
            state <= RELEASED;
        end else begin
            int_count <= int_count + {7'b0, (state == RELEASED && next_state == PRESSING)};
            int_ascii <= data;
            int_data <= data;
            en_data <= (state == PRESSING);
            state <= next_state;
        end
    end

endmodule

module keyboard_inst (
    input clk,
    input reset,
    input ps2_clk,
    input ps2_data,
    output ready,
    output overflow,
    output reg [7:0] data
);

    reg nextdata_n;

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
        end else if (ready & nextdata_n) begin
            nextdata_n <= 1'b0;
            $display("[PS/2 Keyboard Controller] Receive %x", data);
        end else begin
            nextdata_n <= 1'b1;
        end
    end

endmodule