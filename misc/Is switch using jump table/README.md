# Is ``switch`` really using a jump table?

## Code

The four ``*.c`` files gives four simplest examples of using ``switch`` and ``if`` statements.
Compile them with ``-S -S0`` to output the assembly code without any optimization.

## Platform

I used a macOS 14.4.1 running on an MacBook Pro with M1 Pro so my native platform is arm64.
I compiled the code with ``gcc-13`` installed by Homebrew, which is GCC version 13.2.0.
Meanwhile, I installed ``x86_64-elf-gcc`` from Homebrew, which is also GCC version 13.2.0, to see what x86 assembly code will be like.

## Results

### arm64

The compiler gives the exact same way to direct the jump, using ``cmp`` and ``bne`` in all the three successfully compiled cases.
No address table or pre-calculated jump address found in the output file.

``` c
// if.s
cmp	    w0, 64
bne	    L2
mov	    w0, 60
```

``` c
// switch.s
cmp	    w0, 64
bne	    L2
mov	    w0, 60
```

``` c
// if_variable.s
cmp	    w1, w0
bne     L2
mov 	w0, 60
```


### x86_64

The compiler uses three slightly different ways to direct the jump, but all of them is branch-related.
None of them shows the existence of the pre-calculated address as shown in the book.

``` c
// if.s
cmpb	$64, -1(%rbp)
jne     .L2
movl	$60, %eax
```

``` c
// switch.s
movsbl	-1(%rbp), %eax
cmpl	$64, %eax
jne	    .L2
movl	$60, %eax
```

``` c
// if_variable.s
movzbl	-1(%rbp), %eax
cmpb	-2(%rbp), %al
jne	    .L2
movl	$60, %eax
```

Therefore, I further doubt the statement in the book.