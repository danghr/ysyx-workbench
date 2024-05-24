module top_module (
    input [7:0] x,
    input en,
    output reg [2:0] encode,
    output effect,
    output reg [7:0] digit_out
);

    assign effect = en & (|x);

    Encode83 encode83(.x(x), .en(en), .y(encode));
    Decode37digit decode37digit(.x({1'b0, encode}), .en(effect), .out(digit_out[7:1]));
    assign digit_out[0] = 1'b1;     // Decimal point

endmodule

module Encode83 (
    input  [7:0] x,
    input        en,
    output reg [2:0] y
);

    integer i;
    always @(x or en) begin
        if (en) begin
            casez (x)
                8'b1???????: y = 3'd7;
                8'b01??????: y = 3'd6;
                8'b001?????: y = 3'd5;
                8'b0001????: y = 3'd4;
                8'b00001???: y = 3'd3;
                8'b000001??: y = 3'd2;
                8'b0000001?: y = 3'd1;
                8'b00000001: y = 3'd0; 
                default: y = 3'd0;
            endcase
        end else y = 3'd0;
    end

endmodule

module Decode37digit (
    input  [3:0] x,
    input        en,
    output reg [6:0] out
);

    always @(*) begin
        if (en) begin
            case (x)
                4'h0:    out = 7'b0000001;
                4'h1:    out = 7'b1001111;
                4'h2:    out = 7'b0010010;
                4'h3:    out = 7'b0000110;
                4'h4:    out = 7'b1001100;
                4'h5:    out = 7'b0100100;
                4'h6:    out = 7'b0100000;
                4'h7:    out = 7'b0001111;
                4'h8:    out = 7'b0000000;
                4'h9:    out = 7'b0000100;
                4'ha:    out = 7'b0001000;
                4'hb:    out = 7'b1100000;
                4'hc:    out = 7'b0010001;
                4'hd:    out = 7'b1000010;
                4'he:    out = 7'b0110000;
                4'hf:    out = 7'b0111000;
                default: out = 7'b1111111; 
            endcase
        end else
            out = 7'b1111111;
    end

endmodule
