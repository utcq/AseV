#include <stdio.h>
#include <VeAssembler/x86elf.h>
#include <VeAssembler/g_codes.h>

int main() {
    vAsm *assembler = new vAsm("generated");
    char* text = static_cast<char*>(malloc(
        (14) // N. instructions
        *8 // Instruction max size
        *sizeof(char) // Char size
    ));


    size_t c_pos = 0;
    /*assembler->label("main");
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
    c_pos = assembler->ret(text, c_pos);
    assembler->label("_start");
    c_pos = assembler->jump(text, c_pos, assembler->label_resolve("main"));*/
    
    assembler->label("main");
    c_pos = assembler->move_i32(text, c_pos, R_RAX_OP, 10);
    c_pos = assembler->ret(text, c_pos);

    assembler->label("_start");
    c_pos = assembler->move_i32(text, c_pos, R_RAX_OP, 10);
    //c_pos = assembler->jump(text, c_pos, assembler->label_resolve("main"));

    assembler->write_text(text, c_pos);
    assembler->save_obj();
}