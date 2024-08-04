# PA1 实验报告

## 实验进度

## 实验必答题

### 1. YEMU上的程序运行

(pc=0x00, R[0]=?,  R[1]=?,  M[x]=16, M[y]=33, M[z]=0,  halt=0) ->
(pc=0x04, R[0]=33, R[1]=?,  M[x]=16, M[y]=33, M[z]=0,  halt=0) ->
(pc=0x08, R[0]=33, R[1]=33, M[x]=16, M[y]=33, M[z]=0,  halt=0) ->
(pc=0x0c, R[0]=16, R[1]=33, M[x]=16, M[y]=33, M[z]=0,  halt=0) ->
(pc=0x10, R[0]=49, R[1]=33, M[x]=16, M[y]=33, M[z]=0,  halt=0) ->
(pc=0x14, R[0]=49, R[1]=33, M[x]=16, M[y]=33, M[z]=49, halt=0) ->
(pc=0x14, R[0]=49, R[1]=33, M[x]=16, M[y]=33, M[z]=49, halt=1)

通过`union`把一个指令解释为不同的部分，再为不同类型制定专门的解码规则。主循环中根据不同的指令操作数判断使用的解码规则，并通过对内存和寄存器两个数组的操作模拟指令执行行为。


### 2. NEMU执行指令的过程

`exec_once(&s, cpu.pc)`首先将执行的PC记录在`Decode s`中，随后调用`isa_exec_once(s)`执行指令。

`isa_exec_once(s)`首先通过`s->isa.inst.val = inst_fetch(&s->snpc, 4);`从内存中取指，随后调用`decode_exec(s)`执行该代码。

`decode_exec`中通过宏`INSTPAT`匹配并解析指令，其中首先通过`pattern_decode`计算出需要匹配的内容，再通过`((uint64_t)INSTPAT_INST(s) >> shift) & mask) == key`判断当前指令与前述内容是否匹配，若匹配则通过`INSTPAT_MATCH`调用`decode_operand`从指令中取操作数并按照定义执行。