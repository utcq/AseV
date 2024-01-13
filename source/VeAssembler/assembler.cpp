#include <VeAssembler/x86elf.h>
#include <VeAssembler/g_codes.h>
#include <elfio/elfio.hpp>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

namespace utils {
    uint8_t _rcalc(uint8_t reg, uint8_t mod=3, uint8_t rm=0) {
        return 0xc0 + reg;
    }
    uint8_t _cregs(uint8_t target, uint8_t source, uint8_t mod=3, uint8_t rm=0) {
        return 0xc0 + (8 * source) + target;
    }
}

void vAsm::sect_init() {
    ELFIO::section* text_sec = this->writer->sections.add( ".text" );
    text_sec->set_type( ELFIO::SHT_PROGBITS );
    text_sec->set_flags( ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR );
    text_sec->set_addr_align( SECT_TYPES::TEXT );
    text_sec->set_address(BASE_ADDR::B_TEXT);

    ELFIO::section* data_sec = this->writer->sections.add( ".data" );
    data_sec->set_type( ELFIO::SHT_PROGBITS );
    data_sec->set_flags( ELFIO::SHF_ALLOC | ELFIO::SHF_WRITE );
    data_sec->set_addr_align( SECT_TYPES::DATA );
    data_sec->set_address(BASE_ADDR::B_DATA);

    this->text = text_sec;
    this->data = data_sec;
}

void vAsm::write_text(char buffer[], size_t size) {
    ELFIO::section* str_sec = this->writer->sections.add( ".strtab" );
    str_sec->set_type( ELFIO::SHT_STRTAB );
    str_sec->set_addr_align( SECT_TYPES::STR );
    ELFIO::string_section_accessor str_writer( str_sec );

    ELFIO::section* sym_sec = this->writer->sections.add( ".symtab" );
    sym_sec->set_type( ELFIO::SHT_SYMTAB );
    sym_sec->set_info( 2 );
    sym_sec->set_link( str_sec->get_index() );
    sym_sec->set_addr_align( SECT_TYPES::DATA );
    sym_sec->set_entry_size( this->writer->get_default_entry_size( ELFIO::SHT_SYMTAB ) );

    ELFIO::symbol_section_accessor symbol_writer( *this->writer, sym_sec );
    ELFIO::Elf_Word                sym_index = symbol_writer.add_symbol(
                       this->nstr_index, 0, 0, ELFIO::STB_LOCAL, ELFIO::STT_NOTYPE, 0, this->data->get_index() );

    ELFIO::section* rel_sec_t = this->writer->sections.add( ".rel.text" );
    rel_sec_t->set_type( ELFIO::SHT_REL );
    rel_sec_t->set_info( this->text->get_index() );
    rel_sec_t->set_link( sym_sec->get_index() );
    rel_sec_t->set_addr_align( SECT_TYPES::DATA );
    rel_sec_t->set_entry_size( this->writer->get_default_entry_size( ELFIO::SHT_REL ) );

    ELFIO::section* rel_sec_d = this->writer->sections.add( ".rel.data" );
    rel_sec_d->set_type( ELFIO::SHT_REL );
    rel_sec_d->set_info( this->data->get_index() );
    rel_sec_d->set_link( sym_sec->get_index() );
    rel_sec_d->set_addr_align( SECT_TYPES::DATA );
    rel_sec_d->set_entry_size( this->writer->get_default_entry_size( ELFIO::SHT_REL ) );

    ELFIO::relocation_section_accessor rel_writer_t( *this->writer, rel_sec_t );
    ELFIO::relocation_section_accessor rel_writer_d( *this->writer, rel_sec_d );

    for (auto &label: this->dlabels) {
        const char *name = label.first.c_str();
        uint32_t addr = label.second.first;
        size_t size = label.second.second;
        sym_index = symbol_writer.add_symbol( str_writer, name, addr, size, ELFIO::STB_WEAK,
                        ELFIO::STT_FUNC, 0, this->data->get_index() );
        //rel_writer_d.add_entry( addr+BASE_ADDR::B_DATA, sym_index, ELFIO::R_X86_64_64 );
    }

    for (auto &label: this->labels) {
        const char *name = label.first.c_str();
        uint32_t addr = label.second.first;
        size_t size = label.second.second;
        sym_index = symbol_writer.add_symbol( str_writer, name, addr, size, ELFIO::STB_WEAK,
                        ELFIO::STT_FUNC, 0, this->text->get_index() );
        //rel_writer_t.add_entry( addr+BASE_ADDR::B_TEXT, sym_index, ELFIO::R_X86_64_64 );
    }


    this->text->set_data( buffer, size );
}

void vAsm::write_data(char buffer[], size_t size) {
    this->data->set_data( buffer, size );
}

void vAsm::save_obj() {
    this->writer->save(this->filename);
}

size_t vAsm::move_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_MOV_I32_OP;
    uint8_t regp = utils::_rcalc(reg);
    uint8_t val[4];

    for (int i = 0; i < 4; ++i) { val[i] = (value >> (8 * i)) & 0xFF; }

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp) + sizeof(val);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    memcpy(buffer + c_pos + 3, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}


size_t vAsm::add_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value){
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_ADD_I32_OP;
    uint8_t regp = utils::_rcalc(reg);
    uint8_t val[4];

    for (int i = 0; i < 4; ++i) { val[i] = (value >> (8 * i)) & 0xFF; }

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp) + sizeof(val);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    memcpy(buffer + c_pos + 3, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}


size_t vAsm::sub_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_SUB_I32_OP;
    uint8_t regp = utils::_rcalc(reg)|40; // REG /5
    int8_t val[4];

    for (int i = 0; i < 4; ++i) { val[i] = (value >> (8 * i)) & 0xFF; }

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp) + sizeof(val);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    memcpy(buffer + c_pos + 3, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}


size_t vAsm::imul_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_IMUL_I32_OP;
    uint8_t regp = utils::_rcalc(reg) | (1 << 5); // OR to make imul return into reg
    uint8_t val[4];

    for (int i = 0; i < 4; ++i) { val[i] = (value >> (8 * i)) & 0xFF; }

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp) + sizeof(val);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    memcpy(buffer + c_pos + 3, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}

size_t vAsm::xor_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_XOR_I32_OP;
    uint8_t regp = utils::_rcalc(reg)+48;
    uint8_t val[4];

    for (int i = 0; i < 4; ++i) { val[i] = (value >> (8 * i)) & 0xFF; }

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp) + sizeof(val);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    memcpy(buffer + c_pos + 3, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}


size_t vAsm::move_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2) {

    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_MOV_R64_OP;
    uint8_t regp = utils::_cregs(reg1, reg2);

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    this->current_address += size;
    return c_pos + size;
}

size_t vAsm::add_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_ADD_R64_OP;
    uint8_t regp = utils::_cregs(reg1, reg2);

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    this->current_address += size;
    return c_pos + size;
}


size_t vAsm::sub_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_SUB_R64_OP;
    uint8_t regp = utils::_cregs(reg1,reg2);

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    this->current_address += size;
    return c_pos + size;
}

size_t vAsm::imul_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst1 = I_IMUL_R64_OP_1;
    uint8_t inst2 = I_IMUL_R64_OP_2;
    uint8_t regp = utils::_cregs(reg2, reg1);

    size_t size = sizeof(rex) + sizeof(inst1) + sizeof(inst2) + sizeof(regp);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst1, sizeof(inst1));
    memcpy(buffer + c_pos + 2, &inst2, sizeof(inst2));
    memcpy(buffer + c_pos + 3, &regp, sizeof(regp));
    this->current_address += size;
    return c_pos + size;
}

size_t vAsm::xor_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_XOR_R64_OP;
    uint8_t regp = utils::_cregs(reg1,reg2);

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    this->current_address += size;
    return c_pos + size;
}


size_t vAsm::syscall(char *buffer, size_t c_pos) {
    uint8_t fnc  = F_SYSCALL_OP;
    uint8_t inst = I_SYSCALL_OP;

    size_t size = sizeof(fnc) + sizeof(inst);

    memcpy(buffer + c_pos, &fnc, sizeof(fnc));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    this->current_address += size;
    return c_pos + size;
}

size_t vAsm::interrupt(char *buffer, size_t c_pos, int8_t value) {
    uint8_t inst = I_INTERRUPT_OP;

    size_t size = sizeof(inst) + sizeof(value);

    memcpy(buffer + c_pos, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 1, &value, sizeof(value));
    this->current_address += size;
    return c_pos + size;
}

size_t vAsm::ret(char *buffer, size_t c_pos) {
    uint8_t inst1 = I_RET_OP;
    size_t size1 = sizeof(inst1);


    memcpy(buffer + c_pos, &inst1, sizeof(inst1));
    this->current_address += size1;
    return c_pos + size1;
}

void vAsm::label(const char* name, size_t size) {
    if (strcmp(name, "_start")==0) {
        this->writer->set_entry( this->current_address );
    }
    this->labels[name] = std::make_pair(this->current_address, size);
}

void vAsm::data_label(const char* name, size_t size) {
    this->dlabels[name] = std::make_pair(this->data_current_address, size);
}

uint32_t vAsm::label_resolve(const char *name) {
    auto it = this->labels.find(name);
    auto it1 = this->dlabels.find(name);
    if (it != this->labels.end()) {
        uint32_t val = it->second.first;
        val += BASE_ADDR::B_TEXT;
        printf("%#x\n", val);
        return val;
    }
    else if (it1 != this->dlabels.end()) {
        uint32_t val = it1->second.first;
        val += BASE_ADDR::B_DATA;
        return val;
    }
    else {
        printf("Label %s Not Found!\n", name);
        return 0;
    }
}

size_t vAsm::jump(char *buffer, size_t c_pos, uint32_t addr) {
    uint8_t inst = I_JMP_OP;
    uint32_t jmpof = addr-this->current_address -5;

    uint8_t val[4];
    
    for (int i = 0; i < 4; ++i) { val[i] = (jmpof >> (8 * i)) & 0xFF; }

    size_t size = sizeof(inst) + sizeof(val);

    memcpy(buffer + c_pos, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 1, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}

size_t vAsm::call(char *buffer, size_t c_pos, uint32_t addr) {
    uint8_t inst = I_CALL_OP;
    uint32_t jmpof = addr-this->current_address-BASE_ADDR::B_TEXT -5;

    uint8_t val[4];
    
    for (int i = 0; i < 4; ++i) { val[i] = (jmpof >> (8 * i)) & 0xFF; }

    size_t size = sizeof(inst) + sizeof(val);

    memcpy(buffer + c_pos, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 1, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}


size_t vAsm::ascii(char *buffer, size_t c_pos, const char *value) {
    size_t size = strlen(value);
    memcpy(buffer + c_pos, value, size);
    this->data_current_address += size;
    return c_pos + size;
}

size_t vAsm::move_i64(char* buffer, size_t c_pos, uint8_t reg, uint64_t value) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = utils::_rcalc(reg)-8;
    uint64_t dlab = value;

    uint8_t val[8];

    for (int i = 0; i < 8; ++i) {
        val[i] = static_cast<uint8_t>((dlab >> (8 * i)) & 0xFF);
    }

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(val);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, val, sizeof(val));
    this->current_address += size;
    return c_pos + size;
}
