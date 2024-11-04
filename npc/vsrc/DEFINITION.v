`define ysyx_24070014_WORD_LEN 32
`define ysyx_24070014_ADDR_LEN `ysyx_24070014_WORD_LEN
`define ysyx_24070014_DATA_LEN `ysyx_24070014_WORD_LEN
`define ysyx_24070014_INST_LEN 32

`define ysyx_24070014_REG_ADDR_WIDTH 5

// Initialize PC
`define ysyx_24070014_INIT_PC `ysyx_24070014_ADDR_LEN'h80000000
// Base address for memory
`define ysyx_24070014_MBASE `ysyx_24070014_ADDR_LEN'h80000000
// Size for memory
`define ysyx_24070014_MSIZE `ysyx_24070014_ADDR_LEN'h8000000
// Reset offset for the program counter
`define ysyx_24070014_PC_RESET_OFFSET `ysyx_24070014_ADDR_LEN'h0

`define ysyx_24070014_imm_I 3'd1
`define ysyx_24070014_imm_S 3'd2
`define ysyx_24070014_imm_B 3'd3
`define ysyx_24070014_imm_J 3'd4
`define ysyx_24070014_imm_U 3'd5
