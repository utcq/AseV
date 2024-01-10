#include <VeAssembler/x86elf.h>
#include <VeAssembler/g_codes.h>
#include <elfio/elfio.hpp>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>


namespace utils {
    uint8_t _rcalc(uint8_t reg, uint8_t mod=3, uint8_t rm=0) {
        return _c_mrmb(mod,reg,rm);
    }
    uint8_t _cregs(uint8_t target, uint8_t source, uint8_t mod=3, uint8_t rm=0) {
        uint8_t reg1 = _rcalc(target, mod, rm);
        uint8_t reg2 = _rcalc(source, mod, rm);

        return (mod << 6) | (reg1 << 3) | reg2;
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
    uint8_t rex  = F_REX_OP;
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
    uint8_t rex  = F_REX_OP;
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
    uint8_t rex  = F_REX_OP;
    uint8_t inst = I_SUB_I32_OP;
    uint8_t regp = utils::_rcalc(reg+5);
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
    uint8_t rex  = F_REX_OP;
    uint8_t inst = I_IMUL_I32_OP;
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


size_t vAsm::move_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2) {
    uint8_t rex  = F_REX_OP;
    uint8_t inst = I_MOV_R64_OP;
    uint8_t regp = utils::_cregs(reg1, reg2);

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    return c_pos + size;
}

size_t vAsm::add_reg(char *buffer, size_t c_pos, uint8_t reg1, uint8_t reg2) {
    uint8_t rex  = F_REX_OP;
    uint8_t inst = I_ADD_R64_OP;
    uint8_t regp = utils::_cregs(reg1, reg2);

    size_t size = sizeof(rex) + sizeof(inst) + sizeof(regp);

    memcpy(buffer + c_pos, &rex, sizeof(rex));
    memcpy(buffer + c_pos + 1, &inst, sizeof(inst));
    memcpy(buffer + c_pos + 2, &regp, sizeof(regp));
    return c_pos + size;
}