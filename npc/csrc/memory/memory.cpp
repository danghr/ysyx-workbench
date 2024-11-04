#include <memory.h>

word_t memory(word_t addr)
{
	uint32_t insts[] = {
			0x00100093, // addi x1, x0, 1
			0x00108113, // addi x2, x1, 1
			0x00a10a13, // addi x20, x2, 10
			0x00108093, // addi x1, x1, 1
			0x00108093, // addi x1, x1, 1
			0x00100073, // ebreak
	};
	return insts[(addr - 0x80000000) / 4];
}