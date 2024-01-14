#include <elfio/elfio.hpp>
#include <cstdlib>
#include <map>

#ifndef _AS_x86ELF_H
#define _AS_x86ELF_H


enum SECT_TYPES {
    STR  = 0x01 ,
    TEXT = 0x10,
    DATA = 0x04
};

enum BASE_ADDR {
    B_TEXT = 0x401000,
    B_DATA = 0x402000
};

class vAsm {
    public:
        vAsm(const char *name) {
            this->writer = new ELFIO::elfio();
            this->writer->create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
            this->writer->set_os_abi(ELFIO::ELFOSABI_LINUX);
            this->writer->set_type(ELFIO::ET_REL);
            this->writer->set_machine(ELFIO::EM_X86_64);
            this->writer->set_entry( BASE_ADDR::B_TEXT );
            this->filename = std::string(name) + ".o";
            this->sect_init();
        }

        //void new_section(const char *name, char content[], size_t size, size_t type);
        void sect_init();

        size_t move_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value);
        size_t add_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value);
        size_t sub_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value);

        size_t imul_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value);
        size_t xor_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value);

        size_t move_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2);
        size_t add_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2);
        size_t sub_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2);
        size_t imul_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2);
        size_t xor_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2);

        size_t syscall(char *buffer, size_t c_pos);
        size_t interrupt(char *buffer, size_t c_pos, int8_t value);

        size_t ret(char *buffer, size_t c_pos);

        void label(const char* name, size_t size=0);
        void data_label(const char* name, size_t size=0);
        uint64_t label_resolve(const char *name);
        size_t jump(char *buffer, size_t c_pos, uint64_t addr);
        size_t call(char *buffer, size_t c_pos, uint64_t addr);

        size_t move_i64(char* buffer, size_t c_pos, uint8_t reg, uint64_t value);
        size_t ascii(char *buffer, size_t c_pos, const char *value);
        size_t resb(char *buffer, size_t c_pos, size_t value);
        size_t labsize(uint64_t value);
        size_t dbi32(char *buffer, size_t c_pos, int32_t value);

        void write_text(char buffer[], size_t size);
        void write_data(char buffer[], size_t size);
        void save_obj();

    private:
        ELFIO::elfio *writer;
        std::string filename;
        std::map<std::string, std::pair<uint64_t, size_t>> labels;
        std::map<std::string, std::pair<uint64_t, size_t>> dlabels;
        uint64_t current_address=0;
        uint64_t data_current_address=0;
        char *data_current_name;

        ELFIO::section *text;
        ELFIO::section *data;

        ELFIO::Elf_Word nstr_index = 0;
};

#endif
