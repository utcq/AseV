
## API Reference


## User

### Assembler

#### SECT_TYPES

Section types enum (val=align_addr)

```cpp
  - TEXT = 0x10
  - DATA = 0x4
```


#### vAsm

The assembler class

```cpp
vAsm *assembler = new vAsm("obj_name");
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `name` | `const char*` | **Required**. object name  |

---

#### vAsm::new_section

Create a new Section

`void`
```cpp
assembler->new_sction(".text", text, t_pos, SECT_TYPES::TEXT)
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `name` | `const char *` | **Required**. section name |
| `content` | `char[]` | **Required**. section bytes |
| `size` | `size_t` | **Required**. section bytes size |
| `type` | `size_t` | **Required**. section type (SECT_TYPES) |

---

#### vAsm::move_i32

Move integer 32bit to register

`size_t`
```cpp
assembler->move_i32(text, c_pos, R_RAX_OP, 1);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `buffer` | `char *` | **Required**. bytes buffer to append to |
| `c_pos` | `size_t` | **Required**. current buffer position |
| `reg` | `uint8_t` | **Required**. target register |
| `value` | `int32_t` | **Required**. source value |

---

#### vAsm::add_i32

Add integer 32bit to register

`size_t`
```cpp
assembler->add_i32(text, c_pos, R_RBX_OP, 5);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `buffer` | `char *` | **Required**. bytes buffer to append to |
| `c_pos` | `size_t` | **Required**. current buffer position |
| `reg` | `uint8_t` | **Required**. target register |
| `value` | `int32_t` | **Required**. source value |

---

#### vAsm::sub_i32

Subtract integer 32bit from register

`size_t`
```cpp
assembler->sub_i32(text, c_pos, R_RCX_OP, 2);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `buffer` | `char *` | **Required**. bytes buffer to append to |
| `c_pos` | `size_t` | **Required**. current buffer position |
| `reg` | `uint8_t` | **Required**. target register |
| `value` | `int32_t` | **Required**. source value |

---

#### vAsm::imul_i32

Multiply register by integer 32bit

__[ Result in target register ]__

`size_t`
```cpp
assembler->imul_i32(text, c_pos, R_RAX_OP, 3);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `buffer` | `char *` | **Required**. bytes buffer to append to |
| `c_pos` | `size_t` | **Required**. current buffer position |
| `reg` | `uint8_t` | **Required**. target register |
| `value` | `int32_t` | **Required**. source value |

---

#### vAsm::move_reg

Move source Register value to target Register

`size_t`
```cpp
assembler->move_reg(text, c_pos, R_RAX_OP, R_RBX_OP);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `buffer` | `char *` | **Required**. bytes buffer to append to |
| `c_pos` | `size_t` | **Required**. current buffer position |
| `reg1` | `uint8_t` | **Required**. target register |
| `reg2` | `int32_t` | **Required**. source register |

---

#### vAsm::add_reg

Add source Register value to target Register

`size_t`
```cpp
assembler->add_reg(text, c_pos, R_RCX_OP, R_RAX_OP);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `buffer` | `char *` | **Required**. bytes buffer to append to |
| `c_pos` | `size_t` | **Required**. current buffer position |
| `reg1` | `uint8_t` | **Required**. target register |
| `reg2` | `int32_t` | **Required**. source register |

---


### GENERAL CODES

```yml
- F_* = FLAG
- I_* = INSTRUCTION
- R_* = REGISTER

- *_I32 = integer 32bit source
- *_R64 = 64bit Register source
```


```hpp
#define F_REX_OP 0x48

#define I_MOV_I32_OP 0xC7
#define I_ADD_I32_OP 0x81
#define I_SUB_I32_OP 0x81
#define I_IMUL_I32_OP 0x69

#define I_MOV_R64_OP 0x89
#define I_ADD_R64_OP 0x01
#define I_SUB_R64_OP 0x81
#define I_IMUL_R64_OP 0x69

#define R_RAX_OP 0x00
#define R_RCX_OP 0x01
#define R_RDX_OP 0x02
#define R_RBX_OP 0x03
#define R_RSP_OP 0x04
#define R_RBP_OP 0x05
#define R_RSI_OP 0x06
#define R_RDI_OP 0x07
```


## Dev

### Utils

#### utils::_rcalc

Calculate ModR/M byte

`uint8_t`
```cpp
uint8_t rax = utils::_rcalc(R_RAX_OP);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `reg` | `uint8_t` | **Required**. Register index |
| `mod` | `uint8_t` | Mod field |
| `rm` | `uint8_t` | Register/Memory field |

---

#### utils::_cregs

Calculate Registers combination for operations

`uint8_t`
```cpp
uint8_t rax_rbx = utils::_rcalc(R_RAX_OP, R_RBX_OP);
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `target` | `uint8_t` | **Required**. Target register |
| `source` | `uint8_t` | **Required**. source register |
| `mod` | `uint8_t` | Mod field |
| `rm` | `uint8_t` | Register/Memory field |

---

