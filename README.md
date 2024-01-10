# ASe V

A full C++ (forced by ELFIO) simple ELF64 assembler

## Usage/Examples

```cpp
#include <VeAssembler/x86elf.h>
#include <VeAssembler/g_codes.h>

int main() {
    vAsm *assembler = new vAsm("generated");
    char* text = static_cast<char*>(malloc(
        (8) // N. instructions
        *8 // Instruction max size
        *sizeof(char) // Char size
    ));


    size_t c_pos = 0;
    c_pos = assembler->move_i32(text, c_pos, R_RAX_OP, 10);
    c_pos = assembler->add_i32(text, c_pos, R_RAX_OP, 10);
    c_pos = assembler->sub_i32(text, c_pos, R_RSP_OP, 10);
    c_pos = assembler->imul_i32(text, c_pos, R_RSP_OP, 10);
    c_pos = assembler->xor_i32(text, c_pos, R_RDI_OP, 10);
    c_pos = assembler->move_reg(text, c_pos, R_RSP_OP, R_RDI_OP);
    c_pos = assembler->add_reg(text, c_pos, R_RAX_OP, R_RCX_OP);
    c_pos = assembler->sub_reg(text, c_pos, R_RBP_OP, R_RSI_OP);
    c_pos = assembler->imul_reg(text, c_pos, R_RSP_OP, R_RAX_OP);
    c_pos = assembler->xor_reg(text, c_pos, R_RDI_OP, R_RBX_OP);
    c_pos = assembler->syscall(text, c_pos);
    c_pos = assembler->interrupt(text, c_pos, 0x80);


    assembler->new_section(".text", text, c_pos, SECT_TYPES::TEXT);
    assembler->save_obj();
}
```
---
```asm
Disassembly of section .text:

0000000008048000 <.text>:
 8048000:       48 c7 c0 0a 00 00 00    mov    rax,0xa
 8048007:       48 81 c0 0a 00 00 00    add    rax,0xa
 804800e:       48 81 ec 0a 00 00 00    sub    rsp,0xa
 8048015:       48 69 e4 0a 00 00 00    imul   rsp,rsp,0xa
 804801c:       48 81 f7 0a 00 00 00    xor    rdi,0xa
 8048023:       48 89 fc                mov    rsp,rdi
 8048026:       48 01 c8                add    rax,rcx
 8048029:       48 29 f5                sub    rbp,rsi
 804802c:       48 0f af e0             imul   rsp,rax
 8048030:       48 31 df                xor    rdi,rbx
 8048033:       0f 05                   syscall
 8048035:       cd 80                   int    0x80
```

# Getting Started

```sh
make build run dump clean
```

## Library

[Documentation](API.md)

[Dev Stack](tools.txt)
