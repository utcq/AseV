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

void vAsm::new_section(const char *name, char content[], size_t size, size_t type) {
    ELFIO::section* sec = this->writer->sections.add( name );
    sec->set_type( ELFIO::SHT_PROGBITS );

    if (type == SECT_TYPES::TEXT) {
        sec->set_flags( ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR );
    }
    else if (type == SECT_TYPES::DATA) {
        sec->set_flags( ELFIO::SHF_ALLOC | ELFIO::SHF_WRITE );
    }

    sec->set_addr_align( type );


    sec->set_data(content, size);

    ELFIO::segment* seg = this->writer->segments.add();
    seg->set_type( ELFIO::PT_LOAD );

    if (type == SECT_TYPES::TEXT) {
        seg->set_virtual_address( 0x08048000 );
        seg->set_physical_address( 0x08048000 );
        seg->set_flags( ELFIO::PF_X | ELFIO::PF_R );
        seg->set_align( 0x1000 );
    }
    else if (type == SECT_TYPES::DATA) {
        seg->set_virtual_address( 0x08048020 );
        seg->set_physical_address( 0x08048020 );
        seg->set_flags( ELFIO::PF_W | ELFIO::PF_R );
        seg->set_align( 0x10 );
    }

    seg->add_section_index( sec->get_index(),
                            sec->get_addr_align() );
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
    return c_pos + size;
}


size_t vAsm::sub_i32(char* buffer, size_t c_pos, uint8_t reg, int32_t value) {
    uint8_t rex  = F_REX0_OP;
    uint8_t inst = I_SUB_I32_OP;
    uint8_t regp = utils::_rcalc(reg)|40; // IDK sub just likes reg+40
    int8_t val[4];

    for (int i = 0; i < 4; ++i) { val[i] = (value >> (8 * i)) & 0xFF; }

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp) + sizeof(val);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    memcpy(buffer + c_pos + 3, val, sizeof(val));
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
    return c_pos + size;
}


size_t vAsm::syscall(char *buffer, size_t c_pos) {
    uint8_t fnc  = F_SYSCALL_OP;
    uint8_t inst = I_SYSCALL_OP;

    size_t size = sizeof(fnc) + sizeof(inst);

    memcpy(buffer + c_pos, &fnc, sizeof(fnc));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    return c_pos + size;
}

size_t vAsm::interrupt(char *buffer, size_t c_pos, int8_t value) {
    uint8_t inst = I_INTERRUPT_OP;


    size_t size = sizeof(inst) + sizeof(value);

    memcpy(buffer + c_pos, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 1, &value, sizeof(value));
    return c_pos + size;
}