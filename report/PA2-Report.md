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

## 备注

### 手动编译RISC-V版本的SDL2以解决`riscv64-linux-gnu-ld: cannot find -lSDL2: no error`问题

编译：
```shell
./configure --host=riscv64-linux-gnu --prefix=/media/psf/YSYX/riscv-sdl2
make
make install
```

