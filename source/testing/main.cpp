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

    char* data = static_cast<char*>(malloc(
        32 // N. Bytes
        *sizeof(char) // Char size
    ));


    size_t t_pos = 0;
    size_t d_pos = 0;

    assembler->data_label("msg");
    d_pos = assembler->ascii(data, d_pos, "Hello, world!\n");

    assembler->label("main");
    t_pos = assembler->move_i32(text, t_pos, R_RAX_OP, 1);
    t_pos = assembler->move_i32(text, t_pos, R_RDI_OP, 1);
    t_pos = assembler->move_i64(text, t_pos, R_RSI_OP, assembler->label_resolve("msg"));
    t_pos = assembler->move_i32(text, t_pos, R_RDX_OP, 14);
    t_pos = assembler->syscall(text, t_pos);
    t_pos = assembler->ret(text, t_pos);

    assembler->label("_start");
    t_pos = assembler->call(text, t_pos, assembler->label_resolve("main"));
    t_pos = assembler->move_i32(text, t_pos, R_RAX_OP, 60);
    t_pos = assembler->move_i32(text, t_pos, R_RDI_OP, 0);
    t_pos = assembler->syscall(text, t_pos);

    assembler->write_data(data, d_pos);
    assembler->write_text(text, t_pos);
    assembler->save_obj();
}
