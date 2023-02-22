# 0 "suma.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "suma.S"
# 1 "include/asm.h" 1
# 2 "suma.S" 2

.globl addASM; .type addASM, @function; .align 0; addASM:
    push %ebp
    mov %esp, %ebp
    mov 0x8(%ebp), %eax
    add 0xc(%ebp), %eax
    #mov %ebp, %esp innecesaria, ya que no hemos modificado %esp
    pop %ebp
    ret
