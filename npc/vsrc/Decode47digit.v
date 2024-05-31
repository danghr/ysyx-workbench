module Decode47digit (
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