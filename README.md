# ASe V

A full C++ (forced by ELFIO) simple ELF64 assembler

## Usage/Examples

```cpp
#include <VeAssembler/x86elf.h>
#include <VeAssembler/g_codes.h>

int main() {
    vAsm *assembler = new vAsm("generated");
    char* text = static_cast<char*>(malloc(
        (6) // N. instructions
        *8 // Instruction max size
        *sizeof(char) // Char size
    ));

    size_t c_pos = 0;
    c_pos = assembler->move_i32(text, c_pos, R_RAX_OP, 10);
    c_pos = assembler->add_i32(text, c_pos, R_RAX_OP, 10);
    c_pos = assembler->sub_i32(text, c_pos, R_RAX_OP, 10);
    c_pos = assembler->imul_i32(text, c_pos, R_RAX_OP, 10);
    c_pos = assembler->move_reg(text, c_pos, R_RAX_OP, R_RBX_OP);
    c_pos = assembler->add_reg(text, c_pos, R_RBX_OP, R_RCX_OP);


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
804800e:       48 81 e8 0a 00 00 00    sub    rax,0xa
8048015:       48 69 c0 0a 00 00 00    imul   rax,rax,0xa
804801c:       48 89 d8                mov    rax,rbx
804801f:       48 01 c8                add    rax,rcx
```

# Getting Started

```sh
make build run dump clean
```

## Library

[Documentation](API.md)

[Dev Stack](tools.txt)
