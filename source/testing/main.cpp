#include <stdio.h>
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