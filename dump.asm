
.section .text
.global _start
    
    _start:
        mov $10, %rax
        mov $60, %rax
        mov $0, %rbx
        syscall