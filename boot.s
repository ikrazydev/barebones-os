/* Multiboot headear constants */
.set ALIGN, 1<<0
.set MEMINFO, 1<<1
.set FLAGS, ALIGN | MEMINFO
.set MAGIC, 0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

/* Multiboot section */
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/* Stack initialization */
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

/* Kernel entry */
.section .text
.global _start
.type _start, @function
_start:

    mov $stack_top, %esp

    call kernel_main

    cli /* Goodbye, interrupt flags */
1:  hlt /* Loop in halt? */
    jmp 1b

.size _start, . - _start /* Important for debugging */
