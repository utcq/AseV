section .data
    my_string db 'Hello, NASM!', 0

    my_string_length equ $ - my_string  ; Calculate the length using the $ symbol

section .text
    global _start

_start:
    mov rax, my_string_length
