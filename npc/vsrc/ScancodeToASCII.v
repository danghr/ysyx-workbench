module ScancodeToASCII (
    input clk,
    input [7:0] scan_code,
    output reg [7:0] ascii_code
);

    reg [7:0] scan_to_ascii[0:255];
    
    initial begin
        $readmemh("keyboard_to_ascii", scan_to_ascii);
        $display("Scancode Test: %h", scan_to_ascii[21]);
    end

    always @(posedge clk ) begin
        
        ascii_code <= scan_to_ascii[scan_code];
    end

endmodule