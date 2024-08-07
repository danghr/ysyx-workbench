# PA1 实验报告

## 实验进度

### 交叉编译时链接器找不到SDL2库

#### 问题描述

在Ubuntu 22.04（aarch64）上交叉编译RISC-V版本的测试用例时，遇到`riscv64-linux-gnu-ld: cannot find -lSDL2: no error`错误。

重新安装系统并逐步比对每次`apt install`之后的执行情况发现，问题产生的原因在于通过`apt`安装的SDL2库。详细原因如下：

- `abstract-machine`的Makefile会在`CFLAGS`上加上`$(shell sdl2-config --cflags)`的输出，在`LDFLAGS`上加上`$(shell sdl2-config --libs)`的输出
- 未通过`apt`安装SDL2库时，两者均会提示`sdl2-config: No such file or directory`，但不会在标准输出中产生任何结果
- 通过`apt`安装`libsdl2-dev`库后，执行`sdl2-config --cflags`会输出`-I/usr/include/SDL2 -D_REENTRANT`并被追加到`CFLAGS`中，执行`sdl2-config --libs`会输出`-lSDL2`并被追加到`LDFLAGS`中
- 执行`riscv64-linux-gnu-ld`时，提示`cannot find -lSDL2: no error`

运行`find /usr -name libSDL2.so`，发现该文件位于`/usr/lib/aarch64-linux-gnu/libSDL2.so`目录中；运行`riscv64-linux-gnu-gcc -print-search-dirs`，发现前述目录并不在`riscv64-linux-gnu-gcc`所查找动态链接库的目录中，该动态链接版本也无法被`riscv64-linux-gnu`的工具链所读取。

#### 解决方案1

- 通过`dpkg add-architecture riscv64`、`apt update`、`apt install libsdl2-dev:riscv64`安装`riscv64`版本的SDL2
- 在`abstract-machine/Makefile`的`LDFLAGS`中、`$(shell sdl2-config --libs)`之前增加`-L/usr/lib/riscv64-linux-gnu`以指定使用上一步所安装的SDL2
- 在编译命令中将`$ISA`指定为`riscv64`而非`riscv32`
  - 这会关闭`COMMON_CFLAGS += -march=rv32im_zicsr -mabi=ilp32`和`LDFLAGS += -melf32lriscv`两个指令，从而使`riscv64-linux-gnu-ld`能够正确连接`riscv64`版本的`libsdl2.so`。
  
##### 上述方案引入的风险

- 上述方案实际上将程序编译为`RV64G`而非`RV32I`，存在使用未定义寄存器和超过32为数据的风险。

#### 解决方案2

- 删除`abstract-machine/Makefile`中`CFLAGS`和`LDFLAGS`里有关`sdl2-config`的语句，使其不再链接SDL2库

##### 上述方案引入的风险

- 上述方案删除了对SDL2库的引用和链接，在后期需要音视频显示的应用里可能会工作不正常。

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