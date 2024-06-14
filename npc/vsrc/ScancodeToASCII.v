module ScancodeToASCII (
    input clk,
    input [7:0] scan_code,
    output reg [7:0] ascii_code
);

    reg [7:0] scan_to_ascii[0:255];
    
    initial begin
        $readmemh("/home/danghr/Desktop/YSYX/ysyx-workbench/npc/scancode_to_ascii.txt", scan_to_ascii);
        $display("Scancode test, should be 0x71: 0x%h", scan_to_ascii[21]);
    end

    always @(posedge clk ) begin
        ascii_code <= scan_to_ascii[scan_code];
    end

endmodule